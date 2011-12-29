/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: UFS_MOOSApp.cpp                                      */
/*   DATE: Nov 23rd 2011                                        */
/****************************************************************/

#include <iterator>
#include "UFS_MOOSApp.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

UFS_MOOSApp::UFS_MOOSApp()
{
  m_total_reports    = 0;
  m_refresh_mode     = "streaming";
  m_update_requested = true;

  m_layout_index     = 0;
  m_layout_applied   = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool UFS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string moos_var   = msg.GetKey();
    //double dval  = msg.GetDouble();
    //string sval  = msg.GetString(); 
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();
    
    if(m_map_varkeys.count(moos_var)) {
      m_map_latest[moos_var] = msg.GetString();
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool UFS_MOOSApp::OnConnectToServer()
{
  registerVariables();  
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool UFS_MOOSApp::Iterate()
{
  if((m_refresh_mode == "help") && m_update_requested)
    printHelp();
  else if((m_refresh_mode == "streaming") || m_update_requested) {
    makeReportRaw();
    //outputRawReport();
    //outputRawColInfo();
    printReport();
    m_total_reports++;
  }

  m_update_requested = false;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool UFS_MOOSApp::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::reverse_iterator p;
  for(p = sParams.rbegin();p!=sParams.rend();p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);

    if(param == "SCOPE") {
      bool ok = addScope(value);
      if(!ok) 
	MOOSTrace("Bad SCOPE parameter: %s\n", param.c_str());
    }
  }

  // Go through parameters twice so we can ensure that the LAYOUT
  // configurations make sense w.r.t. to the SCOPE configurations.
  for(p = sParams.rbegin();p!=sParams.rend();p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);

    if(param == "LAYOUT") {
      bool ok = addLayout(value);
      if(!ok) 
	MOOSTrace("Bad LAYOUT parameter: %s\n", param.c_str());
    }
  }

  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: handleCommand

void UFS_MOOSApp::handleCommand(char c)
{
  switch(c) {
  case 'l':
  case 'L':
    m_update_requested = true;
    if(!m_layout_applied)
      m_layout_applied = true;
    else {
      m_layout_index++;
      if(m_layout_index >= m_layouts.size()) {
	m_layout_applied = false;
	m_layout_index = 0;
      }
    }
    break;
  case 'a':
  case 'A':
    m_layout_applied = !m_layout_applied;
    m_update_requested = true;
    break;
  case 'r':
  case 'R':
    m_refresh_mode = "streaming";
    break;
  case ' ':
    m_refresh_mode = "paused";
    m_update_requested = true;
    break;
  case 'h':
  case 'H':
    if(m_refresh_mode != "help")
      m_refresh_mode = "help";
    else
      m_refresh_mode = "paused";
    m_update_requested = true;
    break;
  }
}
  
//------------------------------------------------------------
// Procedure: registerVariables

void UFS_MOOSApp::registerVariables()
{
  map<string,string>::iterator p = m_map_varkeys.begin();
  while(p != m_map_varkeys.end()) {
    string moos_var = p->first;
    m_Comms.Register(moos_var, 0);
    p++;
  }
}
  
//------------------------------------------------------------
// Procedure: addScope

bool UFS_MOOSApp::addScope(string config_str)
{
  UFS_Config ufs_config(config_str);

  bool valid = ufs_config.valid();
  if(!valid)
    return(false);

  m_config.push_back(ufs_config);

  string moos_var = ufs_config.getMOOSVar();
  string key = ufs_config.getKey();
  m_map_varkeys[moos_var] = key; 

  return(true);
}

//------------------------------------------------------------
// Procedure: addLayout
//   Example: "Time, Speed, Distance"
//   Example: "Time, Speed"
//   Example: "Speed, Distance"
//
//      Note: A check is performed to see if all the fields in the
//            given layout match one of the SCOPE configurations
//            in either the "fld" or "alias". Return true if so.
//      Note: If the check fails we accept the layout anyway, but 
//            the user may get less than what is expected in the 
//            output since mismatches are just dropped.

bool UFS_MOOSApp::addLayout(string config_str)
{
  // Part 1: Add the new layout after parsing and stripping blank
  // ends from each component
  vector<string> new_layout;
  vector<string> svector = parseString(config_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++)
    new_layout.push_back(stripBlankEnds(tolower(svector[i])));
  m_layouts.push_back(new_layout);
			 
  // Part 2: Perform an error check on the new layout w.r.t. all
  // the scope configurations currently in effect.
  bool all_match = true;
  for(i=0; i<vsize; i++) {
    string this_field = tolower(new_layout[i]);

    bool jmatch = false;
    unsigned int j, jsize = m_config.size();
    for(j=0; j<jsize; j++) {
      string alias = tolower(m_config[j].getAlias());
      string fld = tolower(m_config[j].getField());
      if((alias == this_field) || (fld == this_field))
	jmatch = true;
    }
    all_match = all_match && jmatch;
  }

  return(all_match);
}


//------------------------------------------------------------
// Procedure: makeReportRaw
//
//
// VName    Speed  Distance  Battery  DCharges  Range
// -----    -----  --------  -------  --------  -----
// charlie   1.21     410.5      72%     8        132
// berta     1.19     388.2      64%     2        180
// alpha     1.12     412.9      79%     4      11600
//
// m_raw_report:
// VName    Speed  Distance  Battery  DCharges  Range
// charlie   1.21     410.5      72%     8        132
// berta     1.19     388.2      64%     2        180
// alpha     1.12     412.9      79%     4      11600
//
// m_raw_report_maxlens:
// 6         4        5          3       1        5

void UFS_MOOSApp::makeReportRaw()
{
  // Part 1: Get a list of Vehicle Names
  set<string> vnames;
  map<string, string>::iterator p=m_map_latest.begin();
  while(p != m_map_latest.end()) {
    string moos_var = p->first;
    string contents = p->second;
    string key = m_map_varkeys[moos_var];
    string vname = tokStringParse(contents, key, ',', '=');
    if(vname != "")
      vnames.insert(vname);
    p++;
  }
  //cout << "Total # of VNames: " << vnames.size() << endl;

  // Part 2: Build a the first row of the table containing all the
  // headers.
  vector<string> headers;
  headers.push_back("VName");
  unsigned int h, hsize = m_config.size();
  for(h=0; h<hsize; h++) {
    if(configInLayout(m_config[h])) {
	string hdr = m_config[h].getField();
	string alias = m_config[h].getAlias();
	if(alias != "")
	  hdr = alias;
	headers.push_back(hdr);
    }
  }

  // Part 3: For each Vehicle, For each config entry, fill in value
  //
  // In this loop a vector of vectors will be created. One for each
  // row in the table. This table is stored as a member variable so 
  // must be cleared at the outset.
  m_raw_report.clear();
  m_raw_report.push_back(headers);
  set<string>::iterator p2 = vnames.begin();
  while(p2 != vnames.end()) {
    string vname = *p2;
    // In this loop a vector of length equal to the number of configs
    // will be built. One entry for each column of the table.
    vector<string> row;
    row.push_back(vname);
    unsigned int i, vsize = m_config.size();
    for(i=0; i<vsize; i++) {
      if(configInLayout(m_config[i])) {
	string moos_var  = m_config[i].getMOOSVar();
	string fld       = m_config[i].getField();
	string latest    = m_map_latest[moos_var];
	string value     = tokStringParse(latest, fld, ',', '=');
	row.push_back(value);
      }
    }
    m_raw_report.push_back(row);
    p2++;
  }

  // Part 4: For each column in the raw report table, determine the
  // Maximum length for each column by examining the entries for each
  // row.

  // First create a vector of length equal to the number or columns. 
  // We assume the number of columns is the same for each row based on
  // the way we constructed things in Part 2.
  unsigned int columns = headers.size();
  vector<unsigned int> tmp(columns, 0);
  m_raw_report_maxlens = tmp;

  unsigned int i, j, rows = m_raw_report.size();
  for(i=0; i<rows; i++) {
    for(j=0; j<columns; j++) {
      unsigned int len = m_raw_report[i][j].length();
      if(len > m_raw_report_maxlens[j])
	m_raw_report_maxlens[j] = len;
    }
  }
      
}


//------------------------------------------------------------
// Procedure: printReport

void UFS_MOOSApp::printReport() const
{
  // Part 1: First ensure we have a non-empty properly formatted raw table.
  unsigned int i, rows = m_raw_report.size();
  if(rows == 0) {
    cout << "Empty Results" << endl;
    return;
  }
  for(i=0; i<rows; i++) {
    if(m_raw_report[i].size() != m_raw_report_maxlens.size()) {
      cout << "uFldReport - Table MisMatch!!!" << endl;
      return;
    }
  }

  // Part 2: Flush the screen a bit with newlines
  cout << endl << endl << endl << endl << endl << endl;


  // Part 3A: Output the Table Header Labels
  string column_sep = "  ";
  unsigned j, columns = m_raw_report[0].size();
  for(j=0; j<columns; j++) {
    unsigned int len = m_raw_report_maxlens[j];
    string entry = padString(m_raw_report[0][j], len);
    cout << entry << column_sep;
  }
  if(m_refresh_mode == "paused")
    cout << termColor("reversered") << "PAUSED";
  else if(m_refresh_mode == "streaming")
    cout << termColor("reversegreen") << "STREAMING";
  if(m_layout_applied)
    cout << termColor("reverseblue") << uintToString(m_layout_index);
  cout << termColor() << endl;

  // Part 3B: Output the Table Header Separators
  for(j=0; j<columns; j++) {
    unsigned int len = m_raw_report_maxlens[j];
    string entry(len, '=');
    cout << entry << column_sep;
  }
  cout << "(" << uintToString(m_total_reports) << ")" << endl;

  
  // Part 4: Output the body of the table
  for(i=1; i<rows; i++) {   
    columns = m_raw_report[i].size();
    for(j=0; j<columns; j++) {
      unsigned int len = m_raw_report_maxlens[j];
      string entry = padString(m_raw_report[i][j], len);
      cout << entry << column_sep;
    }
    cout << endl;
  }  
}

//------------------------------------------------------------
// Procedure: printHelp()

void UFS_MOOSApp::printHelp() const
{
  string refstr = termColor("reversered") + "HELP" + termColor();

  printf("\n\n");
  printf("KeyStroke    Function         %s      \n", refstr.c_str());
  printf("---------    ---------------------------          \n");
  printf("  SPACE      Update info once-now, then Pause     \n");
  printf("   a/A       Show ALL columns - no layout filters \n");
  printf("   l/L       Use/Change the layout filter(s)      \n");
  printf("   p/P       Pause information refresh            \n");
  printf("   r/R       Resume information refresh           \n");
  printf("   h/H       Show this Help msg - 'R' to resume   \n");
}


//------------------------------------------------------------
// Procedure: outputRawReport
//   Purpose: Purely for debugging.

void UFS_MOOSApp::outputRawReport() const
{
  unsigned int i, rows = m_raw_report.size();
  for(i=0; i<rows; i++) {   
    unsigned j, columns = m_raw_report[i].size();
    for(j=0; j<columns; j++)
      cout << m_raw_report[i][j] << "  ";
    cout << endl;
  }  
}

//------------------------------------------------------------
// Procedure: outputRawColInfo
//   Purpose: Purely for debugging.

void UFS_MOOSApp::outputRawColInfo() const
{
  unsigned j, columns = m_raw_report_maxlens.size();
  for(j=0; j<columns; j++)
    cout << m_raw_report_maxlens[j] << "  ";
  cout << endl;
}


//------------------------------------------------------------
// Procedure: configInLayout
//   Purpose: Determine if the given Scope Config is to be included
//            a report based on: 
//            (a) Whether layouts generally are being applied presently.
//            (b) If so, which layout is being applied
//            (c) Whether the config is included in the current layout 

bool UFS_MOOSApp::configInLayout(const UFS_Config& config) const
{
  cout << "configInLayout:" << config.getField() << endl;
  // Part 1: If layouts are not being applied, the ALL Scope Configs
  // are included in all reports. Just return true.
  if(!m_layout_applied) {
    cout << "yes1" << endl;
    return(true);
  }

  // Part 2: Check the layout index. If out of bounds, just return
  // true indicating that the current config passes. This will be true
  // when no layouts have been provided by the user.
  if(m_layout_index >= m_layouts.size()) {
    cout << "yes2" << m_layout_index << ":" << m_layouts.size() << endl;
    return(true);
  }
  // Part 3: Check the config's "fld" against a layout
  string fld = tolower(config.getField());
  if(vectorContains(m_layouts[m_layout_index], fld)) {
    cout << "yes3" << endl;
    return(true);
  }

  // Part 4: Check the config's "alias" against a layout
  string alias = tolower(config.getAlias());
  if(vectorContains(m_layouts[m_layout_index], alias)) {
    cout << "yes4" << endl;
    return(true);
  }

  cout << "NO!!!" << endl;
  return(false);
}


