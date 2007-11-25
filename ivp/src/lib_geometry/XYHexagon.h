/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYHexagon.h                                          */
/*    DATE: Oct 30, 2005                                         */
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
 
#ifndef XY_HEXAGON_HEADER
#define XY_HEXAGON_HEADER

#include "XYPolygon.h"

class XYHexagon : public XYPolygon {
public:
  XYHexagon() {cx=0;cy=0;dist=0;};
  ~XYHexagon() {};

  // The only two ways to build a pentagon
  bool   initialize(double, double, double);
  bool   initialize(std::string);       

  double get_cx()   {return(cx);};
  double get_cy()   {return(cy);};
  double get_dist() {return(dist);};

  XYHexagon addNeighbor(int);

  // We disable these operations allowable on the superclass
  bool   add_vertex(double, double) {};   
  bool   alter_vertex(double, double) {};
  bool   delete_vertex(double, double) {};
  bool   insert_vertex(double, double) {};

protected:
  double cx;
  double cy;
  double dist;
};

#endif









