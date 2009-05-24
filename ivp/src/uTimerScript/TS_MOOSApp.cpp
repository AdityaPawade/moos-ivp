/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TS_MOOSApp.cpp                                  */
/*    DATE: May 21st 2009                                   */
/************************************************************/

#include <iterator>
#include "TS_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TS_MOOSApp::TS_MOOSApp()
{
  m_elapsed_time  = 0;
  m_previous_time = -1;
  m_start_time    = 0;
  m_skip_time     = 0;
  m_pause_time    = 0;
  m_paused        = false;
  m_posted_count  = 0;
  
  m_var_next_event = "TIMER_SCRIPT_NEXT";
  m_var_forward    = "TIMER_SCRIPT_FORWARD";
  m_var_pause      = "TIMER_SCRIPT_PAUSE";
  m_var_status     = "TIMER_SCRIPT_STATUS";
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();

    if((key == m_var_next_event) && (tolower(sval) == "next"))
      jumpToNextPostingTime();
    else if((key == m_var_forward) && !mstr && (dval>0))
      m_skip_time += dval;
    else if(key == m_var_pause) {
      string pause_val = tolower(sval);
      if(pause_val == "true")
	m_paused = true;
      else if(pause_val == "false")
	m_paused = false;
      else if(pause_val == "toggle")
	m_paused = !m_paused;
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool TS_MOOSApp::Iterate()
{
  double curr_time = MOOSTime();
  if(m_start_time == 0)
    m_start_time = curr_time;  

  if(m_paused) {
    double delta_time = 0;
    if(m_previous_time != -1)
      delta_time = curr_time - m_previous_time;
    m_pause_time += delta_time;
  }

  m_elapsed_time = (curr_time - m_start_time) + m_skip_time - m_pause_time;

  checkForReadyPostings();

  m_previous_time = curr_time;
  postStatus();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TS_MOOSApp::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  RegisterVariables();  

  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()

bool TS_MOOSApp::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("pTimerScript starting....\n");

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string line  = stripBlankEnds(*p);
      string param = tolower(stripBlankEnds(biteString(line, '=')));
      string value = stripBlankEnds(line);

      if(param == "event")
	addNewEvent(value);
      else if(param == "paused")
	setBooleanOnString(m_paused, value);
      else if((param == "jump_var") || (param == "jump_variable")) {
	if(!strContainsWhite(value)) {
	  m_var_next_event = value;
	  m_Comms.Register(m_var_next_event, 0);
	}
      }
      else if((param == "forward_var") || (param == "forward_variable")) {
	if(!strContainsWhite(value)) {
	  m_var_forward = value;
	  m_Comms.Register(m_var_forward, 0);
	}
      }
      else if((param == "pause_var") || (param == "pause_variable")) {
	if(!strContainsWhite(value)) {
	  m_var_pause = value;
	  m_Comms.Register(m_var_pause, 0);
	}
      }
      else if(param == "status_var") {
	if(!strContainsWhite(value))
	  m_var_status = value;
      }
    }
  }

  printScript();
  RegisterVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: RegisterVariables

void TS_MOOSApp::RegisterVariables()
{
  m_Comms.Register(m_var_next_event, 0);
  m_Comms.Register(m_var_forward, 0);
  m_Comms.Register(m_var_pause, 0);
}

//------------------------------------------------------------
// Procedure: addNewEvent()
// EVENT = var=FOOBAR, val=true, time=45.0

bool TS_MOOSApp::addNewEvent(string event_str)
{
  string new_var;
  string new_val;
  double new_time_of_event = -1;

  vector<string> svector = parseStringQ(event_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    if(param == "var")
      new_var = value;
    else if(param == "val")
      new_val = value;
    else if((param == "time") && isNumber(value)) {
      double dval = atof(value.c_str());
      if(dval > 0)
	new_time_of_event = dval;
    }
  }
  
  if((new_var=="") || (new_val=="") || (new_time_of_event <=0))
    return(false);

  VarDataPair new_pair(new_var, new_val, "auto");
  m_pairs.push_back(new_pair);
  m_ptime.push_back(new_time_of_event);
  m_poked.push_back(false);

  sortEvents();
  return(true);
}

//------------------------------------------------------------
// Procedure: sortEvents
// 

void TS_MOOSApp::sortEvents()
{
  unsigned int count = 0;
  unsigned int i, vsize = m_pairs.size();
  vector<bool> sorted(vsize,false);
  
  vector<VarDataPair> new_pairs;
  vector<double>      new_ptime;
  vector<bool>        new_poked;

  while(count < vsize) {
    unsigned int oldest_index = 0;
    double       oldest_ptime = -1;
    for(i=0; i<vsize; i++) {
      if((m_ptime[i] > oldest_ptime) && !sorted[i]) {
	oldest_ptime = m_ptime[i];
	oldest_index = i;
      }
    }
    sorted[oldest_index] = true;
    new_pairs.push_back(m_pairs[oldest_index]);
    new_ptime.push_back(m_ptime[oldest_index]);
    new_poked.push_back(m_poked[oldest_index]);
    count++;
  }

  m_pairs = new_pairs;
  m_ptime = new_ptime;
  m_poked = new_poked;
  std::reverse(m_pairs.begin(), m_pairs.end());
  std::reverse(m_ptime.begin(), m_ptime.end());
  std::reverse(m_poked.begin(), m_poked.end());
}
    
//------------------------------------------------------------
// Procedure: printScript
// 

void TS_MOOSApp::printScript()
{
  unsigned int i, vsize = m_pairs.size();
  
  cout << "The Script: ========================================" << endl;
  cout << "Total Elements: " << vsize << endl;
  for(i=0; i<vsize; i++) {
    string vdpair_str = m_pairs[i].getPrintable();
    double ptime  = m_ptime[i];
    bool   poked  = m_poked[i];

    cout << "[" << i << "] " << vdpair_str << ", TIME:" << ptime;
    cout << ", POSTED=" << boolToString(poked) << endl;
  }
  cout << "====================================================" << endl;
}
    
//----------------------------------------------------------------
// Procedure: checkForReadyPostings()
//   Purpose: Go through the list of events and possibly post the
//            event if the elapsed time is greater or equal to the
//            post time, and if not posted previously.

void TS_MOOSApp::checkForReadyPostings()
{
  unsigned int i, vsize = m_pairs.size();
  for(i=0; i<vsize; i++) {
    // First check to see if the pair is ready, and not poked already
    if((m_elapsed_time >= m_ptime[i]) && !m_poked[i]) {
      string variable = m_pairs[i].get_var();
      if(m_pairs[i].is_string()) 
	m_Comms.Notify(variable, m_pairs[i].get_sdata());
      else
	m_Comms.Notify(variable, m_pairs[i].get_ddata());
      // If just now poked, note it, so it won't be poked again
      m_posted_count++;
      m_poked[i] = true;
    }
  }
}


//----------------------------------------------------------------
// Procedure: jumpToNextPosting()
//   Purpose: Go through the list of events and find the next un-poked
//            var-data pair and jump forward in time to the time at 
//            which the next unpoked var-data pair is ready.
//            The member variable "m_skip_time" is incremented to hold
//            the cumulative time jumped forward.

void TS_MOOSApp::jumpToNextPostingTime()
{
  double skip_amt = 0;
  unsigned int i, vsize = m_pairs.size();
  for(i=0; ((i<vsize)&&(skip_amt==0)); i++) {
    if(!m_poked[i])
      skip_amt = m_ptime[i] - m_elapsed_time;
  }
   
  m_skip_time += skip_amt;
}


//----------------------------------------------------------------
// Procedure: postStatus

void TS_MOOSApp::postStatus()
{
  if((m_var_status == "") || (m_var_status == "silent"))
    return;

  string status = "elapsed_time=" + doubleToString(m_elapsed_time,2);
  status += ", posted=" + intToString(m_posted_count);
  
  int pending = (m_pairs.size() - m_posted_count);
  status += ", pending=" + intToString(pending);

  status += ", paused=" + boolToString(m_paused);
  
  m_Comms.Notify(m_var_status, status);
}
  
  
