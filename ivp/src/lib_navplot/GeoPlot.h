/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GeoPlot.h                                            */
/*    DATE: Aug 9th, 2009                                        */
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

#ifndef GEO_PLOT_HEADER
#define GEO_PLOT_HEADER

#include <string>
#include <vector>
#include <list>
#include "VPlug_GeoShapes.h"

class GeoPlot
{
public:
  GeoPlot() {};
  ~GeoPlot() {};

  bool            addEvent(std::string var, std::string val, double time);
  void            setVehiName(std::string s) {m_vehi_name = s;};
  double          getTimeByIndex(unsigned int index) const;
  VPlug_GeoShapes getVPlugByIndex(unsigned int index) const;
  VPlug_GeoShapes getVPlugByTime(double gtime) const;
  std::string     getVehiName() const   {return(m_vehi_name);};
  unsigned int    size() const          {return(m_time.size());};
  void            print() const;
  double          getMinTime() const;
  double          getMaxTime() const;

 protected:  
  unsigned int getIndexByTime(double) const;

protected:
  std::string                  m_vehi_name;  // Name of the platform
  std::vector<double>          m_time;
  std::vector<VPlug_GeoShapes> m_vplugs;
};
#endif 








