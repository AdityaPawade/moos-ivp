/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RealmSummary.cpp                                     */
/*    DATE: Dec 9th 2020                                         */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <iterator>
#include "MBUtils.h"
#include "RealmSummary.h"

using namespace std;

//---------------------------------------------------------
// Procedure: addProc()

void RealmSummary::addProc(string proc_str)
{
  if(!vectorContains(m_procs, proc_str))
    m_procs.push_back(proc_str);
}

//---------------------------------------------------------
// Procedure: valid()

bool RealmSummary::valid() const
{
  if((m_node == "") || (m_procs.size() == 0))
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: get_spec()

string RealmSummary::get_spec() const
{
  if((m_node == "") || (m_procs.size() == 0))
    return("realm_summary_err");
  
  string msg = "node=" + m_node + "#channels=";
  string channels;

  for(unsigned int i=0; i<m_procs.size(); i++) {
    if(channels != "")
      channels += ",";
    channels += m_procs[i];
  }
  msg += channels;

  return(msg);
}


//---------------------------------------------------------
// Procedure: string2RealmSummary()
//   Example: node=alpha # channels=pHelmIvP,pNodeReporter,...

RealmSummary string2RealmSummary(string str)
{
  RealmSummary null_summary;
  RealmSummary good_summary;

  vector<string> parts = parseString(str, '#');
  for(unsigned int i=0; i<parts.size(); i++) {
    string param = biteStringX(parts[i], '=');
    string value = parts[i];

    if(param == "node") 
      good_summary.setNode(value);
    else if(param == "channels") {
      vector<string> procs = parseString(value, ',');
      for(unsigned int j=0; j<procs.size(); j++) 
	good_summary.addProc(procs[j]);
    }
    else
      return(null_summary);
  }

  return(good_summary);
}



