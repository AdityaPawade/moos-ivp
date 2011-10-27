/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: InfoBuffer.H                                         */
/*    DATE: Oct 12th 2004 Thanksgiving in Waterloo               */
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

#ifdef _WIN32 //MSVC pragma added by PMN in FAF '05
#pragma warning(disable : 4786)
#endif

#ifndef INFO_BUFFER_IVP_HEADER
#define INFO_BUFFER_IVP_HEADER

#include <string>
#include <vector>
#include <map>
#include "InfoBuffer.h"

class InfoBuffer {
public:
  InfoBuffer()  {m_curr_time_utc=0;};
  ~InfoBuffer() {};

public:
  std::string sQuery(std::string, bool&) const;

  double dQuery(std::string, bool&) const;
  double tQuery(std::string, bool elapsed=true) const;

  std::vector<std::string> sQueryDeltas(std::string, bool&) const;
  
  std::vector<double> dQueryDeltas(std::string, bool&) const;

  bool   isKnown(std::string);
  void   print() const;

public:
  bool   setValue(std::string, double);
  bool   setValue(std::string, std::string);
  void   clearDeltaVectors();
  void   setCurrTime(double t)         {m_curr_time_utc = t;};
  double getCurrTime() const           {return(m_curr_time_utc);};

protected:
  std::map<std::string, std::string> smap;
  std::map<std::string, double>      dmap;
  std::map<std::string, double>      tmap;

  std::map<std::string, std::vector<std::string> >  vsmap;
  std::map<std::string, std::vector<double> > vdmap;

  double m_curr_time_utc;
};
#endif


