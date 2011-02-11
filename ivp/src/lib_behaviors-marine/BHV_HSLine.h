/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HSLine.h                                         */
/*    DATE: Feb 8th 2009                                         */
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
 
#ifndef BHV_HSLINE_HEADER
#define BHV_HSLINE_HEADER

#include "IvPBehavior.h"
#include "XYSegList.h"

class BHV_HSLine : public IvPBehavior {
public:
  BHV_HSLine(IvPDomain);
  ~BHV_HSLine() {};
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleState()      {postErasableSegList();};
  void         onCompleteState()  {postErasableSegList();};

 protected: 
  void      postErasableSegList();

 protected:  // configuration parameters
  double    m_time_on_leg;               // seconds
  double    m_max_des_speed_observed;    // meters/second

  XYSegList m_seglist;
};
#endif



