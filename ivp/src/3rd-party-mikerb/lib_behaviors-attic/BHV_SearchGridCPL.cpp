/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_SearchGrid.cpp                                   */
/*    DATE: Sep 3rd, 2005                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "BHV_SearchGrid.h"
#include "MBUtils.h"
#include "IO_GeomUtils.h"
#include "AOF_SearchGrid.h"
#include "AOF_SDY.h"
#include "AOF_Aggregate.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_SearchGrid::BHV_SearchGrid(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_search_grid");
  this->setParam("unifbox", "course=6,  speed=4");
  this->setParam("gridbox", "course=12, speed=8");

  m_domain = subDomain(m_domain, "course,speed");

  has_fired = false;
  osX   = 0;
  osY   = 0;
  osSPD = 0;

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_SearchGrid::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "searchgrid") {
    XYGrid new_search_grid;
    bool ok = new_search_grid.initialize(val);
    if(!ok) 
      return(false);
    search_grid = new_search_grid;
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_SearchGrid::onRunState() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  IvPFunction *of = 0;

  bool ok = updateFromInfoBuffer();
  if(!ok)
    return(0);

  bool os_in_grid = updateSearchGrid(prev_osX, prev_osY, osX, osY);

  AOF_SDY        *aof_sdy = 0;
  AOF_SearchGrid *aof_sgrid = 0;
  AOF_Aggregate  *aof = 0;

  aof       = new AOF_Aggregate(m_domain);
  aof_sgrid = new AOF_SearchGrid(m_domain, &search_grid);

  aof_sgrid->setParam("os_lat", osY);
  aof_sgrid->setParam("os_lon", osX);
  aof_sgrid->setParam("time_horizon", 45);
  aof_sgrid->initialize();

  aof->addAOF(aof_sgrid, 100);

  if(os_in_grid) {
    aof_sdy = new AOF_SDY(m_domain);
    aof_sdy->setParam("desired_course", osCRS);
    aof->addAOF(aof_sdy, 150);
  }

  OF_Reflector reflector(aof, 1);
  
  reflector.createUniform(unif_box, grid_box);
  of = reflector.extractOF();

  delete(aof);
  return(of);
}

//-----------------------------------------------------------
// Procedure: updateFromInfoBuffer
//

bool BHV_SearchGrid::updateFromInfoBuffer()
{
  cout << "config: " << search_grid.getConfigString() << endl;
    
  // If we have ever fired before, set the "previous" values.
  if(has_fired) {
    prev_osX = osX;
    prev_osY = osY;
  }
  else {
    has_fired = true;
    postMessage("GRID_CONFIG", search_grid.getConfigString());
    return(false);
  }

  bool ok1, ok2, ok3, ok4;
  osX   = getBufferDoubleVal("NAV_X",     &ok1);
  osY   = getBufferDoubleVal("NAV_Y",     &ok2);
  osSPD = getBufferDoubleVal("NAV_SPEED", &ok3);
  osCRS = getBufferDoubleVal("NAV_HEADING", &ok4);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2)
    postEMessage("No ownship X/Y info in info_buffer.");
  if(!ok3)
    postWMessage("No ownship speed info in info_buffer.");
  if(!ok4)
    postWMessage("No ownship heading info in info_buffer.");
  if(!ok1 || !ok2 || !ok3 || !ok4)
    return(false);


  return(true);
}

//-----------------------------------------------------------
// Procedure: updateSearchGrid
//

bool BHV_SearchGrid::updateSearchGrid(double x1, double y1, 
				      double x2, double y2) 
{
  //cout << " x1:" << x1 << " y1:" << y1;
  //cout << " x2:" << x2 << " y2:" << y2 << endl;

  //cout << "grid_min: " << search_grid.getMinVal() << endl;
  //cout << "grid_max: " << search_grid.getMaxVal() << endl;

  bool os_in_grid = false;

  int gsize = search_grid.size();
  for(int i=0; i<gsize; i++) {
    XYSquare square = search_grid.getElement(i);
    os_in_grid = os_in_grid || square.containsPoint(x2,y2);
    double length = square.segIntersectLength(x1,y1,x2,y2);
    if(length > 0) {
      cout << "(" << i << "): length:" << length << endl;
      double time_in_square = 0;
      if(osSPD > 0)
	time_in_square = length / osSPD;
      string glabel  = search_grid.getLabel();
      string gvar1 = "GRID_DELTA";
      string gvar2 = "GRID_DELTA_" + glabel;
      double cur_val = search_grid.getVal(i);
      double new_val = cur_val + time_in_square;
      if(new_val > 100)
	new_val = 100;
      search_grid.setVal(i, new_val);
      string gdelta = glabel + "@" + 
	intToString(i) + "," +
	dstringCompact(doubleToString(cur_val)) + "," +
	dstringCompact(doubleToString(new_val));
      postMessage(gvar1, gdelta);
      postMessage(gvar2, gdelta);
    }
  }

  search_grid.resetFromMin();
  return(os_in_grid);
}








