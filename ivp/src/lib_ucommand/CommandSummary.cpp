/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CommandSummary.cpp                                   */
/*    DATE: July 8th, 2016                                       */
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
#include "CommandSummary.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

CommandSummary::CommandSummary() 
{
  m_posting_count       = 0;
  m_report_pending = false;
}

//----------------------------------------------------------------------
// Procedure: addPosting()

void CommandSummary::addPosting(string post_var, string post_val)
{
  // Part 3: Increment the counter and store the history entry
  string post_pid = uintToString(m_posting_count);
  
  m_post_vars.push_front(post_var);
  m_post_vals.push_front(post_val);
  m_post_pids.push_front(post_pid);
  m_post_acks[post_pid] = false;
  m_posting_count++;
  
  // Part 4: Truncate the history to maintain maximum length
  if(m_post_vars.size() > 20) {
    m_post_vars.pop_back();
    m_post_vals.pop_back();
    string post_pid = m_post_pids.back();
    m_post_pids.pop_back();
    m_post_acks.erase(post_pid);
  }
  
  m_report_pending = true;
}


//----------------------------------------------------------------------
// Procedure: getCommandReport()

vector<string> CommandSummary::getCommandReport() 
{
  // New ACTable with 3 columns and 3 blanks separating columns
  ACTable actab(3, 3);

  actab.setColumnJustify(1, "right");
  actab << "MOOSVar | PID  | Value ";
  actab.addHeaderLines();
  
  list<string>::iterator p;
  list<string>::iterator q = m_post_vals.begin();
  list<string>::iterator r = m_post_pids.begin();
  
  for(p=m_post_vars.begin(); p!=m_post_vars.end(); p++) {
    string post_var = *p;
    string post_val = *q;
    string post_pid = *r;
    actab << post_var << post_pid << post_val;
    q++;
    r++;
  }

  vector<string> command_report = actab.getTableOutput();
  m_report_pending = false;

  return(command_report);
}

