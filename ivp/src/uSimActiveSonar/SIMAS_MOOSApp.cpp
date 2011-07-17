/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SIMAS_MOOSApp.cpp                                    */
/*    DATE: June 25th 2011                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iterator>
#include "SIMAS_MOOSApp.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool SIMAS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  m_model.setCurrTime(MOOSTime());
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    bool ok;
    ok = m_model.handleMsg(msg.GetKey(),msg.GetDouble(), msg.GetString(),
			   msg.GetTime(), msg.IsDouble(), msg.IsString(),
			   msg.GetSource());
    if(!ok) {
      cout << termColor("magenta") << endl;
      cout << "Unhandled Message: " << msg.GetKey() << endl;
      cout << termColor() << flush;
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SIMAS_MOOSApp::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}

//------------------------------------------------------------
// Procedure: RegisterVariables

void SIMAS_MOOSApp::RegisterVariables()
{
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("SIMAS_RANGE_REQUEST", 0);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SIMAS_MOOSApp::Iterate()
{
  m_model.setCurrTime(MOOSTime());
  m_model.iterate();
  postMessages(m_model.getMessages());
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SIMAS_MOOSApp::OnStartUp()
{
  cout << termColor();
  cout << endl << "Simulated Active Sonar starting..." << endl;
  m_model.setCurrTime(MOOSTime());
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin(); p!=sParams.end(); p++) {
    string sLine  = *p;
    string param  = stripBlankEnds(MOOSChomp(sLine, "="));
    string value  = stripBlankEnds(sLine);
    m_model.setParam(param, value);
  }
  
  RegisterVariables();

  m_model.print();

  cout << termColor() << flush;
  cout << "Simulated Active Sonar started." << endl << endl;
  return(true);
}


//---------------------------------------------------------
// Procedure: postMessages()

void SIMAS_MOOSApp::postMessages(vector<VarDataPair> msgs)
{
  bool verbose = m_model.verbose();

  unsigned int i, vsize = msgs.size();
  for(i=0; i<vsize; i++) {
    string varname = msgs[i].get_var();
    if(verbose)
      cout << "DBPost: " << varname << "  ";
    if(msgs[i].is_string()) {
      m_Comms.Notify(varname, msgs[i].get_sdata());
      if(verbose)
	cout << msgs[i].get_sdata() << endl;
    }
    else {
      m_Comms.Notify(varname, msgs[i].get_ddata());
      if(verbose)
	cout << msgs[i].get_ddata() << endl;
    }
  }
}

