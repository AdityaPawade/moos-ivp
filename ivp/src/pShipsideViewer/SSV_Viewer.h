/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_Viewer.h                                         */
/*    DATE: September 11th, 2007                                 */
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

#ifndef SHORESIDE_VIEWER_HEADER
#define SHORESIDE_VIEWER_HEADER

#include <vector>
#include <string>
#include <map>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "ObjectPose.h"
#include "ColoredPoint.h"
#include "MarineViewer.h"
#include "XYCircle.h"
#include "MOOSGeodesy.h"
#include "MOOSLock.h"
#include "VMarkerSet.h"
#include "OpAreaSpec.h"

class SSV_Viewer : public MarineViewer
{
 public:
  SSV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);

  bool  setParam(std::string, std::string);
  bool  setParam(std::string, float);

public:
  void  updateVehiclePosition(std::string, float x, float y, 
			      float theta, float spd, float dep=0);  
  void  setVehicleBodyType(std::string, std::string);
  void  addStationCircle(const XYCircle&);
  bool  initGeodesy(double, double);

  void  resetVehicles();
  bool  getLatLon(int, double&, double&);
  float getVehicleInfo(int index, std::string info_type);
  float getRelativeInfo(int index, std::string info_type);

  bool  hasVehiName(std::string);
  std::string getVehiName(int);
  std::string getCurrVName()     {return(getVehiName(m_global_ix));};
  std::string getLeftClick()     {return(m_left_click);};
  std::string getRightClick()    {return(m_right_click);};
  std::string getRightClickRP()  {return(m_right_click_rp);};
  int   getLeftClickIX()         {return(m_left_click_ix);};
  int   getRightClickIX()        {return(m_right_click_ix);};
  float getRadialSize()          {return(m_radial_size);};

  void  clearTrails()            {m_pos_map.clear(); m_hist_map.clear();};
  void  setTime(float v)         {m_curr_time = v;};
  float getTime()                {return(m_curr_time);};
  int   getDataIndex()           {return(m_global_ix);};
  void  setCurrent(std::string);
  void  cycleIndex();
  
 public: // Geomarkers
  bool  addVMarker(const std::string&);

  void  mutexLock()   {m_mutex.Lock();};
  void  mutexUnLock() {m_mutex.UnLock();};

 protected:
  void  drawCommonMarker(double x, double y, double scale, 
			 const std::string& mtype, const std::string& label, 
			 const std::vector<double>& label_color, 
			 const std::vector<std::vector<double> >& color_vectors);
  void  drawVehicle(std::string, bool, std::string);
  void  drawPoints(CPList&);
  void  drawPoint(float, float, int color=0);
  void  handleLeftMouse(int, int);
  void  handleRightMouse(int, int);
  void  drawRadials();
  void  drawBearingLine(int);
  void  drawStationCircles();
  void  drawCirc(XYCircle, int, bool, double, double, double,
		 double=0, double=0, double=0);

  void  drawOpAreaGrid();
  void  drawGridBox(double, double, double, double, 
		    double, double, double, double);
  void  drawGridPN();
  void  drawMarkers();

  ObjectPose getObjectPoseByIndex(int);

 private:
  // Mapping from Vehicle Name to Vehicle Position
  std::map<std::string, ObjectPose>   m_pos_map;
  // Mapping from Vehicle Name to Vehicle Position History
  std::map<std::string, CPList>       m_hist_map;
  // Mapping from Vehicle Name to Vehicle Body Type
  std::map<std::string, std::string>  m_vbody_map;
  // Mapping from Vehicle Name to Time of last AIS report
  std::map<std::string, double>       m_ais_map;

  std::string            m_ownship_name;
  std::vector<XYCircle>  m_station_circ;
  std::string            m_default_vehibody;

  CMOOSGeodesy m_geodesy;

  // Member variables for holding/conveying mouse click info
  std::string m_left_click;
  std::string m_right_click;
  std::string m_right_click_rp;

  int    m_left_click_ix;
  int    m_right_click_ix;

  bool   m_centric_view;
  bool   m_draw_bearing_lines;
  int    m_radial_size;
  float  m_curr_time;

  VMarkerSet  m_vmarkers;
  OpAreaSpec  m_op_area;
  CMOOSLock   m_mutex;
};

#endif 

