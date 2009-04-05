/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: EFlipper.cpp                                         */
/*    DATE: April 4th 2009                                       */
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

#include <iostream>
#include "EFlipper.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

EFlipper::EFlipper()
{
  m_source_separator = ",";
  m_dest_separator   = ",";
}


//----------------------------------------------------------------
// Procedure: setParam

bool EFlipper::setParam(string param, string value)
{
  param = stripBlankEnds(param);
  value = stripBlankEnds(value);
  if(tolower(param) == "key")
    m_key = value;
  else if(tolower(param) == "source_variable")
    m_source_variable = value;
  else if(tolower(param) == "dest_variable")
    m_dest_variable = value;
  else if(tolower(param) == "source_separator")
    m_source_separator = value;
  else if(tolower(param) == "dest_separator")
    m_dest_separator = value;
  else if(tolower(param) == "component") {
    vector<string> svector = parseString(value, "->");
    unsigned int vsize = svector.size();
    if(vsize == 2) {
      string left = stripBlankEnds(svector[0]);
      string right = stripBlankEnds(svector[1]);
      m_cmap[left] = right;
      return(true);
    }
    else
      return(false);
  }
  else if(tolower(param) == "filter") {
    vector<string> svector = parseString(value, "==");
    unsigned int vsize = svector.size();
    if(vsize == 2) {
      string left = stripBlankEnds(svector[0]);
      string right = stripBlankEnds(svector[1]);
      m_fmap[left] = right;
      return(true);
    }
    else
      return(false);
  }
  return(true);
}

//----------------------------------------------------------------
// Procedure: valid
//      Note: Returns true if the EFipper is able to be used. Checks
//            for necessary conditions. 
//
//  FLIP:1    = source_variable  = MVIEWER_LCLICK
//  FLIP:1    = dest_variable    = UP_LOITERA
//  FLIP:1    = source_separator = ,
//  FLIP:1    = dest_separator   = #
//  FLIP:1    = x -> xcenter_assign
//  FLIP:1    = y -> xcenter_assign


bool EFlipper::valid()
{
  if((m_key=="") || (m_source_variable=="") || (m_dest_variable==""))
    return(false);
  if(m_cmap.size() == 0)
    return(false);
  return(true);
}


//----------------------------------------------------------------
// Procedure: print

void EFlipper::print()
{
  cout << "EFlipper: " << endl;
  cout << "  Key:       " << m_key<< endl;
  cout << "  SourceVar: " << m_source_variable << endl;
  cout << "  DestVar:   " << m_dest_variable << endl;
  cout << "  SourceSep: " << m_source_separator << endl;
  cout << "  DestSep:   " << m_dest_separator << endl;

  cout << "  Components: " << endl;
  map<string,string>::iterator p;
  for(p=m_cmap.begin(); p!=m_cmap.end(); p++) {
    string left  = p->first;
    string right = p->second;
    cout << "    " << left << " -> " << right << endl;
  }

  cout << "  Filters: " << endl;
  for(p=m_fmap.begin(); p!=m_fmap.end(); p++) {
    string left  = p->first;
    string right = p->second;
    cout << "    " << left << " <-> " << right << endl;
  }
}
