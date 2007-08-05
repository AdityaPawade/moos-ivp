/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_NULL3D.h                                         */
/*    DATE:                                                      */
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
 
#ifndef AOF_NULL3D_HEADER
#define AOF_NULL3D_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_NULL3D: public AOF {
public:
  AOF_NULL3D(IvPDomain gdomain) {
    universe = new Box(3); 
    universe->setPTS(0, 0, gdomain.get_dpoints(0)-1);
    universe->setPTS(1, 0, gdomain.get_dpoints(1)-1);
    universe->setPTS(2, 0, gdomain.get_dpoints(2)-1);
  };
  ~AOF_NULL3D() {};

public:    
  double evalBox(const IvPBox*) const {return(0.0);};    // virtual defined
};

#endif





