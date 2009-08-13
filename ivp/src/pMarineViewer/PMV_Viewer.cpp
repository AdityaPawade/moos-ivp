/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_Viewer.cpp                                       */
/*    DATE: Nov 11th 2004                                        */
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
#include <math.h>
#include "PMV_Viewer.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "ColorParse.h"

using namespace std;

PMV_Viewer::PMV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_var_index = -1;
  m_var_index_prev = -1;
  m_centric_view = "";
  m_centric_view_sticky = true;
  m_reference_point   = "datum";
  m_reference_bearing = "relative";
  m_mouse_x   = 0;
  m_mouse_y   = 0;
  m_mouse_lat = 0;
  m_mouse_lon = 0;
}

//-------------------------------------------------------------
// Procedure: draw()

void PMV_Viewer::draw()
{
  MarineViewer::draw();

  vector<XYPolygon> polys   = m_geoshapes.getPolygons();
  vector<XYGrid>    grids   = m_geoshapes.getGrids();
  vector<XYPoint>   points  = m_geoshapes.getPoints();
  vector<XYSegList> segls   = m_geoshapes.getSegLists();
  vector<XYCircle>  circles = m_geoshapes.getCircles();

  drawPolygons(polys);
  drawGrids(grids);
  drawSegLists(segls);
  drawCircles(circles);
  drawPoints(points);
  drawDropPoints();


  // Draw Mouse position
  if(Fl::event_state(FL_SHIFT)) {
    string str = "(" + intToString(m_mouse_x) + ", " +
      intToString(m_mouse_y) + ")";
    ColorPack cpack("yellow");
    drawText(m_mouse_x, m_mouse_y, str, cpack, 12);
  }
  else if(Fl::event_state(FL_CTRL)) {
    string str = "(" + doubleToString(m_mouse_lat,6) + ", " +
      doubleToString(m_mouse_lon,6) + ")";    
    ColorPack cpack("yellow");
    drawText(m_mouse_x, m_mouse_y, str, cpack, 12);
  }
  // End Draw Mouse position

  if(m_vehi_settings.isViewableVehicles()) {
    vector<string> svector = m_vehiset.getVehiNames();
    int vsize = svector.size();
    for(int i=0; i<vsize; i++) {
      string vehiname = svector[i];
      bool   isactive = (vehiname == m_vehiset.getActiveVehicle());
      string vehibody = m_vehiset.getStringInfo(vehiname, "body");
      
      // Perhaps draw the history points for each vehicle.
      if(m_vehi_settings.isViewableTrails()) {
	CPList point_list = m_vehiset.getVehiHist(vehiname);
	double trails_length = m_vehi_settings.getTrailsLength();
	cout << "vehiname:" << vehiname << endl;
	cout << "trails_length:" << trails_length << endl;
	cout << "list_length:" << point_list.size() << endl;
	drawTrailPoints(point_list, trails_length);
      }
      else
	cout << "NOT drawing trails" << endl;

      // Next draw the vehicle shapes. If the vehicle index is the 
      // one "active", draw it in a different color.
      drawVehicle(vehiname, isactive, vehibody);
    }
  }

  glFlush();
}

//-------------------------------------------------------------
// Procedure: handle

int PMV_Viewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handleLeftMouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handleRightMouse(vx, vy);
    }
    return(1);
    break;
  case FL_ENTER:
    return(1);
    break;
  case FL_MOVE:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    handleMoveMouse(vx, vy);
    return(1);
    break;
  default:
    return(MarineViewer::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: setParam

bool PMV_Viewer::setParam(string param, string value)
{
  if(MarineViewer::setParam(param, value))
    return(true);

  param = tolower(stripBlankEnds(param));
  value = stripBlankEnds(value);
  
  bool handled = false;
  bool center_needs_adjusting = false;
  
  if(param == "center_view") {
    if((value=="average") || (value=="active") || (value=="reference")) {
      center_needs_adjusting = true;
      m_centric_view = value;
      handled = true;
    }
  }
  else if(param == "reference_tag") {
    if(value == "bearing-absolute")
      m_reference_bearing = "absolute";
    else if(value == "bearing-relative")
      m_reference_bearing = "relative";
    else if(value == "datum")
      m_reference_point = "datum";
    else if(!strContainsWhite(value)) {
      m_vehiset.setParam("center_vehicle_name", value);
      m_reference_point = value;
    }
  }
  else if(param == "new_report_variable") {
    handled = m_vehiset.setParam(param, value);
  }
  else if(param == "view_marker") {
    handled = m_vmarkers.addVMarker(value, m_geodesy);
  }
  else if((param == "node_report") || (param == "node_report_local")){
    handled = m_vehiset.setParam(param, value);
    if(handled && (m_centric_view != "") && m_centric_view_sticky) {
      center_needs_adjusting = true;
    }
  }
  else {
    handled = handled || m_vehi_settings.setParam(param, value);
    handled = handled || m_vehiset.setParam(param, value);
  }

  if(center_needs_adjusting)
    setWeightedCenterView();

  return(handled);
}


//-------------------------------------------------------------
// Procedure: setParam

bool PMV_Viewer::setParam(string param, double value)
{
  // Intercept and disable the centric mode if user pans
  if((param == "pan_x") || (param == "pan_y")) {
    m_centric_view = "";
  }

  bool handled = MarineViewer::setParam(param, value);

  handled = handled || m_vehi_settings.setParam(param, value);
  handled = handled || m_vehiset.setParam(param, value);

  return(handled);
}

//-------------------------------------------------------------
// Procedure: drawVehicle

void PMV_Viewer::drawVehicle(string vname, bool active, string vehibody)
{
  ObjectPose opose = m_vehiset.getObjectPose(vname);
  if(!opose.isValid())
    return;

  // If there has been no explicit mapping of color to the given vehicle
  // name then the "inactive_vehicle_color" will be returned below.
  ColorPack vehi_color;
  if(active)
    vehi_color = m_vehi_settings.getColorActiveVehicle();
  else
    vehi_color = m_vehi_settings.getColorInactiveVehicle();

  ColorPack vname_color = m_vehi_settings.getColorVehicleName();  
  string vnames_mode = m_vehi_settings.getVehiclesNameMode();
  
  double shape_scale  = m_vehi_settings.getVehiclesShapeScale();
  double shape_length = m_vehiset.getDoubleInfo(vname, "vlength") * shape_scale;
  double age_report   = m_vehiset.getDoubleInfo(vname, "age_ais");

  string vname_aug = vname;
  bool  vname_draw = true;
  if(vnames_mode == "off")
    vname_draw = false;
  else if(vnames_mode == "names+mode") {
    string helm_mode = m_vehiset.getStringInfo(vname, "helm_mode");
    if((helm_mode != "none") && (helm_mode != "unknown-mode"))
      vname_aug += " (" + helm_mode + ")";
  }
  else if(vnames_mode == "names+depth") {
    string str_depth = dstringCompact(doubleToString(opose.getDepth(),1));
    vname_aug += " (depth=" + str_depth + ")";
  }

  // If the NODE_REPORT is old, disregard the vname_mode and indicated
  // staleness
  if(age_report > 3) {
    string age_str = doubleToString(age_report,0);
    vname_aug = vname + "(Stale Report - " + age_str + ")";
  } 

  drawCommonVehicle(vname_aug, opose, vehi_color, vname_color, vehibody, 
		    shape_length, vname_draw, 1);
}

//-------------------------------------------------------------
// Procedure: drawTrailPoints

void PMV_Viewer::drawTrailPoints(CPList &cps, int trail_length)
{
  if(!m_vehi_settings.isViewableTrails())
    return;

  vector<double> xvect;
  vector<double> yvect;

  list<ColoredPoint>::reverse_iterator p;
  int i=0;
  for(p=cps.rbegin(); (p!=cps.rend() && (i<trail_length)); p++) {
    if(p->isValid()) {
      xvect.push_back(p->m_x);
      yvect.push_back(p->m_y);
    }
    i++;
  }

  ColorPack   cpack = m_vehi_settings.getColorTrails();
  double    pt_size = m_vehi_settings.getTrailsPointSize();
  bool    connected = m_vehi_settings.isViewableTrailsConnect();
  
  cout << "calling drawpointlist" << xvect.size() << endl;
  drawPointList(xvect, yvect, pt_size, cpack, connected);
}

//-------------------------------------------------------------
// Procedure: handleMoveMouse

void PMV_Viewer::handleMoveMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  m_mouse_x = snapToStep(mx, 0.1);
  m_mouse_y = snapToStep(my, 0.1);
  m_geodesy.LocalGrid2LatLong(m_mouse_x, m_mouse_y, 
			      m_mouse_lat, m_mouse_lon);
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse                       

void PMV_Viewer::handleLeftMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);

  double dlat, dlon;
  m_geodesy.LocalGrid2LatLong(sx, sy, dlat, dlon);
  string slat = doubleToString(dlat, 8);
  string slon = doubleToString(dlon, 8);

  if((Fl::event_state(FL_SHIFT)) || (Fl::event_state(FL_CTRL))) {
    XYPoint dpt(mx, my);
    string latlon, localg, native;
    localg = "(" + intToString(mx) + ", " + intToString(my) + ")";
    latlon = "("  + slat + ", " + slon + ")";
    if(Fl::event_state(FL_SHIFT))
      native = localg;
    else 
      native = latlon;
    dpt.set_label(native);
    m_drop_points.addPoint(dpt, latlon, localg, native);
  }
  else {
    m_left_click =  "x=" + doubleToString(sx,1) + ",";
    m_left_click += "y=" + doubleToString(sy,1);
    m_left_click += ",lat=" + dstringCompact(doubleToString(dlat, 8));
    m_left_click += ",lon=" + dstringCompact(doubleToString(dlon, 8));
    if(m_left_click_context != "")
      m_left_click += (",context=" + m_left_click_context);
  }
}

//-------------------------------------------------------------
// Procedure: handleRightMouse

void PMV_Viewer::handleRightMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);
  
  m_right_click =  "x=" + doubleToString(sx,1) + ",";
  m_right_click += "y=" + doubleToString(sy,1);

  double dlat, dlon;
  m_geodesy.LocalGrid2LatLong(sx, sy, dlat, dlon);
  m_right_click += ",lat=" + dstringCompact(doubleToString(dlat, 8));
  m_right_click += ",lon=" + dstringCompact(doubleToString(dlon, 8));

  if(m_right_click_context != "")
    m_right_click += (",context=" + m_right_click_context);

  //cout << "Right Mouse click at [" << m_right_click << "] meters." << endl;
}

//-------------------------------------------------------------
// Procedure: setWeightedCenterView()

void PMV_Viewer::setWeightedCenterView()
{
  if(m_centric_view == "")
    return;

  double pos_x, pos_y;
  bool ok1 = false;
  bool ok2 = false;
  if(m_centric_view == "average") {
    ok1 = m_vehiset.getWeightedCenter(pos_x, pos_y);
    ok2 = true;
  }
  else if(m_centric_view == "active") {
    ok1 = m_vehiset.getDoubleInfo("active", "xpos", pos_x);
    ok2 = m_vehiset.getDoubleInfo("active", "ypos", pos_y);
  }
  else if(m_centric_view == "reference") {
    ok1 = m_vehiset.getDoubleInfo("center_vehicle", "xpos", pos_x);
    ok2 = m_vehiset.getDoubleInfo("center_vehicle", "ypos", pos_y);
  }

  if(!ok1 || !ok2)
    return;

  // First determine how much we're off in terms of meters
  double delta_x = pos_x - m_back_img.get_x_at_img_ctr();
  double delta_y = pos_y - m_back_img.get_y_at_img_ctr();
  
  // Next determine how much in terms of pixels
  double pix_per_mtr = m_back_img.get_pix_per_mtr();

  double x_pixels = pix_per_mtr * delta_x;
  double y_pixels = pix_per_mtr * delta_y;
  
  m_vshift_x = -x_pixels;
  m_vshift_y = -y_pixels;
}


//-------------------------------------------------------------
// Procedure: addScopeVariable
//      Note: 

bool PMV_Viewer::addScopeVariable(string varname)
{
  varname = stripBlankEnds(varname);
  if(strContainsWhite(varname))
    return(false);
  
  bool scoping_already = false;
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++) 
    if(m_var_names[i] == varname)
      scoping_already = true;

  if(scoping_already)
    return(false);

  m_var_names.push_back(varname);
  m_var_vals.push_back("");
  m_var_source.push_back("");
  m_var_time.push_back("");
  if(m_var_index == -1) {
    m_var_index = 0;
    m_var_index_prev = 0;
  }
  return(true);    
}


//-------------------------------------------------------------
// Procedure: updateScopeVariable
//      Note: 

bool PMV_Viewer::updateScopeVariable(string varname, string value, 
				     string vtime, string vsource)
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++) {
    if(m_var_names[i] == varname) {
      m_var_vals[i] = value;
      m_var_source[i] = vsource;
      m_var_time[i] = vtime;
      return(true);
    }
  }
  return(false);
}

//-------------------------------------------------------------
// Procedure: setActiveScope
//      Note: 

void PMV_Viewer::setActiveScope(string varname)
{
  if(varname == "_previous_scope_var_") {
    int tmp = m_var_index;
    m_var_index = m_var_index_prev;
    m_var_index_prev = tmp;
    return;
  }

  unsigned int i, vsize = m_var_names.size();
  if(varname == "_cycle_scope_var_") {
    m_var_index_prev = m_var_index;
    m_var_index++;
    if(m_var_index >= vsize) {
      m_var_index_prev = vsize-1;
      m_var_index = 0;
    }
    return;
  }

  for(i=0; i<vsize; i++) {
    if(m_var_names[i] == varname) {
      m_var_index_prev = m_var_index;
      m_var_index = i;
      return;
    }
  }
}


// ----------------------------------------------------------
// Procedure: getStringInfo

string PMV_Viewer::getStringInfo(const string& info_type, int precision)
{
  string result = "error";

  if(info_type == "scope_var") {
    if(m_var_index != -1)
      return(m_var_names[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "scope_val") {
    if(m_var_index != -1)
      return(m_var_vals[m_var_index]);
    else
      return("To add Scope Variables: SCOPE=VARNAME in the MOOS config block");
  }
  else if(info_type == "scope_time") {
    if(m_var_index != -1)
      return(m_var_time[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "scope_source") {
    if(m_var_index != -1)
      return(m_var_source[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "left_click_info")
    result = m_left_click;
  else if(info_type == "right_click_info")
    result = m_right_click;
  else if(info_type == "range") {
    double xpos, ypos;
    bool   dhandled1 = m_vehiset.getDoubleInfo("active", "xpos", xpos);
    bool   dhandled2 = m_vehiset.getDoubleInfo("active", "ypos", ypos);
    if(dhandled1 && dhandled2) {
      double x_center = 0;
      double y_center = 0;
      if(m_reference_point != "datum") {
	double cxpos, cypos;
	dhandled1 = m_vehiset.getDoubleInfo("center_vehicle", "xpos", cxpos);
	dhandled2 = m_vehiset.getDoubleInfo("center_vehicle", "ypos", cypos);
	if(dhandled1 && dhandled2) {
	  x_center = cxpos;
	  y_center = cypos;
	}
      }
      double range = hypot((xpos-x_center), (ypos-y_center));
      result = doubleToString(range, precision);
    }
  }
  else if(info_type == "bearing") {
    double xpos, ypos;
    bool   dhandled1 = m_vehiset.getDoubleInfo("active", "xpos", xpos);
    bool   dhandled2 = m_vehiset.getDoubleInfo("active", "ypos", ypos);
    if(dhandled1 && dhandled2) {
      double x_center = 0;
      double y_center = 0;
      double h_heading = 0;
      if(m_reference_point != "datum") {
	double cxpos, cypos, heading;
	bool ok1 = m_vehiset.getDoubleInfo("center_vehicle", "xpos", cxpos);
	bool ok2 = m_vehiset.getDoubleInfo("center_vehicle", "ypos", cypos);
	bool ok3 = m_vehiset.getDoubleInfo("center_vehicle", "heading", heading);
	if(ok1 && ok2 && ok3) {
	  x_center = cxpos;
	  y_center = cypos;
	  h_heading = heading;
	}
      }
      double bearing = 0;
      if((m_reference_bearing == "absolute") || (m_reference_point == "datum"))
	bearing = relAng(x_center, y_center, xpos, ypos);
      else if(m_reference_bearing == "relative") {
	bearing = relAng(x_center, y_center, xpos, ypos);
	bearing = angle360(bearing - h_heading);
      }
      result = doubleToString(bearing, precision);
    }
  }
  else {
    string sresult;
    bool   shandled = m_vehiset.getStringInfo("active", info_type, sresult);
    if(shandled) {
      result = sresult;
    }
    else {
      double dresult;
      bool   dhandled = m_vehiset.getDoubleInfo("active", info_type, dresult);
      if(dhandled)
	result = doubleToString(dresult, precision);
    }
  }
  
  //cout << "GSI type:" << info_type << " result:[" << result << "]" << endl;
  return(result);
}
  

