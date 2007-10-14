/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SimEngine.h                                          */
/*    DATE: Mar 8th, 2005 just another day at CSAIL              */
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

#ifndef SIM_ENGINE_HEADER
#define SIM_ENGINE_HEADER

#include "VState.h"

class SimEngine
{
public:
  SimEngine(double=0, double=0, double=0);
  virtual ~SimEngine() {};
  
public:
  void   propagate(VState&, double, double, double, double=0);
  void   setPushXY(double x, double y) {push_x=x; push_y=y;};
  void   setPushTheta(double deg_per_sec) {push_theta=deg_per_sec;};

  bool   setParam(const std::string&, double);

protected:
  double push_x;
  double push_y;
  double push_theta;

  double top_turn_speed;
  double float_rate;
};

#endif

