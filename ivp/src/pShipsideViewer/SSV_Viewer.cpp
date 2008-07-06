/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_Viewer.cpp                                       */
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

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
#include "SSV_Viewer.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "XYBuildUtils.h"
#include "Shape_Gateway.h"
#include "Shape_EField.h"
#include "Shape_Kelp.h"

using namespace std;

#define HISTORY_SIZE  1000
#define MAX_VEHICLES    20

SSV_Viewer::SSV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_curr_time   = 0;
  m_tiff_offon  = 1;

  m_default_vehibody   = "kayak";
  m_left_click_ix      = 0;
  m_right_click_ix     = 0;
  m_radial_size        = 100;
  m_trail_size         = 0.1;
  m_centric_view       = true;
  m_draw_bearing_lines = true;

  setParam("bearing_color", "orange");
}

//-------------------------------------------------------------
// Procedure: draw()

void SSV_Viewer::draw()
{
  mutexLock();
  MarineViewer::draw();

  drawOpAreaGrid();
  if(m_vmarkers.viewable())
    drawMarkers();

  if(m_grid_offon)
    drawGrids();

  if(m_poly_offon)
    drawPolys();

  drawSegLists();
  drawStationCircles();
  drawCircles();
  drawRadials();
  if(m_draw_bearing_lines)
    drawBearingLine(m_global_ix);

  // Next draw the ownship vehicle shape. If the vehicle 
  // index is the one "active", draw it in a different color.
  unsigned int ix = 0;
  map<string,ObjectPose>::iterator p1;
  for(p1=m_pos_map.begin(); p1!=m_pos_map.end(); p1++) {
    string vname = p1->first;
    if(vname == m_ownship_name) {
      bool active = (ix == m_global_ix);
      string vehibody = m_vbody_map[vname];
      if(vehibody == "")
	vehibody = m_default_vehibody;
      drawVehicle(vname, active, vehibody);
    }
    ix++;
  }

  // Next draw the non-ownship vehicle shapes. If the vehicle 
  // index is the one "active", draw it in a different color.
  ix = 0;
  for(p1=m_pos_map.begin(); p1!=m_pos_map.end(); p1++) {
    string vname = p1->first;
    if(vname != m_ownship_name) {
      bool active = (ix == m_global_ix);
      string vehibody = m_vbody_map[vname];
      if(vehibody == "")
	vehibody = m_default_vehibody;
      drawVehicle(vname, active, vehibody);
    }
    ix++;
  }

  // Finally, draw the history points for each vehicle.
  map<string,CPList>::iterator p2;
  for(p2=m_hist_map.begin(); p2!=m_hist_map.end(); p2++)
    drawPoints(p2->second);

  glFlush();
  mutexUnLock();
}

//-------------------------------------------------------------
// Procedure: handle

int SSV_Viewer::handle(int event)
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
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: updateVehiclePosition
//      Note: We don't redraw or call "redraw" in this method since
//            if there are several updates backlogged that need 
//            to be applied, we want to apply them all to have 
//            the history, but only really want to redraw the 
//            vehicles after the last update is done.

void SSV_Viewer::updateVehiclePosition(string vname, float x, 
				       float y, float theta, 
				       float speed, float depth)
{
  vname = toupper(vname);

  // Handle updating the ObjectPose with the new information
  ObjectPose opose(x,y,theta,speed,depth);

  m_pos_map[vname] = opose;
  m_ais_map[vname] = m_curr_time;
 
  ColoredPoint point(x,y,0,0,255);
  map<string,CPList>::iterator p2;
  p2 = m_hist_map.find(vname);
  if(p2 != m_hist_map.end()) {
    p2->second.push_back(point);
    if(p2->second.size() > HISTORY_SIZE)
      p2->second.pop_front();
  }
  else {
    list<ColoredPoint> newlist;
    newlist.push_back(point);
    m_hist_map[vname] = newlist;
  }
  
  if((m_centric_view) && (toupper(vname) == m_ownship_name)) {

    // First determine how much we're off in terms of meters
    double delta_x = x - m_back_img.get_x_at_img_ctr();
    double delta_y = y - m_back_img.get_y_at_img_ctr();

    // Next determine how much in terms of pixels
    double pix_per_mtr = m_back_img.get_pix_per_mtr();
    double x_pixels = pix_per_mtr * delta_x;
    double y_pixels = pix_per_mtr * delta_y;

    setParam("set_pan_x", -x_pixels);
    setParam("set_pan_y", -y_pixels);
  }
}

//-------------------------------------------------------------
// Procedure: setVehicleBodyType
//      Note: 

void SSV_Viewer::setVehicleBodyType(string vname, string vbody)
{
  vname = toupper(vname);
  m_vbody_map[vname] = tolower(stripBlankEnds(vbody));
}  

//-------------------------------------------------------------
// Procedure: resetVehicles()

void SSV_Viewer::resetVehicles()
{
  m_pos_map.clear();
  m_hist_map.clear();
  m_vbody_map.clear();
  m_ais_map.clear();
}
      
// ----------------------------------------------------------
// Procedure: getLatLon
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. 

bool SSV_Viewer::getLatLon(int index, double& rlat, double& rlon)
{
  ObjectPose opose = getObjectPoseByIndex(index);
  return(m_geodesy.LocalGrid2LatLong(opose.getX(), opose.getY(), rlat, rlon));
  
  if(m_cross_offon)
    return(0.0);
}

// ----------------------------------------------------------
// Procedure: getVehicleInfo
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. 

float SSV_Viewer::getVehicleInfo(int index, string info_type)
{
  if(info_type == "age_ais") {
    if(m_cross_offon)
      return(-1);

    string vname = getVehiName(index);
    map<string,double>::iterator p1;
    p1 = m_ais_map.find(vname);
    if(p1 != m_ais_map.end())
      return(m_curr_time - p1->second);
    return(-1);
  }
  else if((info_type == "heading") || (info_type == "course")) {
    if(m_cross_offon)
      return(0.0);
    
    ObjectPose opose = getObjectPoseByIndex(index);
    return(opose.getTheta());
  }
  else if((info_type == "xpos") || (info_type == "meters_x")) {
    if(m_cross_offon) {
      int iwidth = m_back_img.get_img_width();
      float x_pos = ((float)(iwidth) / 2.0) - (float)(m_vshift_x);
      float x_pct = m_back_img.pixToPctX(x_pos);
      float x_pct_cent = m_back_img.get_img_centx();
      float x_pct_mtrs = m_back_img.get_img_meters();
      float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
      return(meters);
    }
    ObjectPose opose = getObjectPoseByIndex(index);
    return(opose.getX());
  }
  else if((info_type == "ypos") || (info_type == "meters_y")) {
    if(m_cross_offon) {
      int iheight = m_back_img.get_img_height();
      float y_pos = ((float)(iheight) / 2.0) - (float)(m_vshift_y);
      float y_pct = m_back_img.pixToPctY(y_pos);
      float y_pct_cent = m_back_img.get_img_centy();
      float y_pct_mtrs = m_back_img.get_img_meters();
      float meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
      return(meters);
    }
    ObjectPose opose = getObjectPoseByIndex(index);
    return(opose.getY());
  }
  else if(info_type == "speed") {
    if(m_cross_offon)
      return(0.0);
    
    ObjectPose opose = getObjectPoseByIndex(index);
    return(opose.getSpeed());
  }
  else if(info_type == "depth") {
    if(m_cross_offon)
      return(0.0);
    
    ObjectPose opose = getObjectPoseByIndex(index);
    return(opose.getDepth());
  }
  return(0);
}


// ----------------------------------------------------------
// Procedure: getRelativeInfo
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. 

float SSV_Viewer::getRelativeInfo(int index, string info_type)
{
  if(m_cross_offon)
    return(-1);

  string vname = getVehiName(index);
  if((vname=="") || (vname == m_ownship_name) || (m_ownship_name==""))
    return(-1);
  
  map<string,ObjectPose>::iterator p;
  p = m_pos_map.find(vname);
  if(p == m_pos_map.end())
    return(-1);
  ObjectPose pose_vh = p->second;

  p = m_pos_map.find(m_ownship_name);
  if(p == m_pos_map.end())
    return(-1);
  ObjectPose pose_os = p->second;

  double os_x = pose_os.getX();
  double os_y = pose_os.getY();
  double vh_x = pose_vh.getX();
  double vh_y = pose_vh.getY();

  double bearing = relAng(os_x, os_y, vh_x, vh_y);

  info_type = tolower(info_type);
  if(info_type == "bearing")
    return(angle360(bearing));
  
  double rbearing = bearing - pose_os.getTheta();
  if(info_type == "relative_bearing")
    return(angle360(rbearing));
  
  if(info_type == "range")
    return(hypot((os_x - vh_x), (os_y - vh_y)));
}

// ----------------------------------------------------------
// Procedure: getVehiName
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. Anything outside this range 
//            results in an empty string being returned.

string SSV_Viewer::getVehiName(int index)
{
  if((m_cross_offon) || (index == -1))
    return("cross-hairs");

  int ix = index;
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    if(ix==0)
      return(p->first);
    else
      ix--;
  }
  return("");
}

//-------------------------------------------------------------
// Procedure: drawCommonMarker

void SSV_Viewer::drawCommonMarker(double x, double y, double shape_scale, 
				  const string& mtype, const string& label, 
				  const vector<double>& label_color,
				  const vector<vector<double> >& color_vectors)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  // Determine position in terms of image percentage
  float marker_ix = meters2img('x', x);
  float marker_iy = meters2img('y', y);

  // Determine position in terms of view percentage
  float marker_vx = img2view('x', marker_ix);
  float marker_vy = img2view('y', marker_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(marker_vx, marker_vy, 0); // theses are in pixel units

  double mz = sqrt(m_zoom);
  glScalef(mz*shape_scale, mz*shape_scale, mz*shape_scale);

  if(mtype == "gateway") {
    double r1, r2, g1, g2, b1, b2;
    r1=1.0, g1=1.0, b1=0.0, r2=0, g2=0, b2=0;

    if(color_vectors.size() >= 1) {
      r1 = color_vectors[0][0]; 
      g1 = color_vectors[0][1]; 
      b1 = color_vectors[0][2];
    }
    if(color_vectors.size() >= 2) {
      r2 = color_vectors[1][0];
      g2 = color_vectors[1][1];
      b2 = color_vectors[1][2];
    }
    glTranslatef(-g_gatewayCtrX, -g_gatewayCtrY, 0);
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, r1, g1, b1);    
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, 0,0,0, 1);    
    drawGLPoly(g_gatewayMidBody, g_gatewayMidBodySize, r2, g2, b2);
    glTranslatef(g_gatewayCtrX, g_gatewayCtrY, 0);
  }

  else if(mtype == "efield") {
    double r1, r2, g1, g2, b1, b2;
    r1=0, g1=0, b1=0, r2=1, g2=0.843, b2=0;

    if(color_vectors.size() >= 1) {
      r1 = color_vectors[0][0];
      g1 = color_vectors[0][1];
      b1 = color_vectors[0][2];
    }
    if(color_vectors.size() >= 2) {
      r2 = color_vectors[1][0];
      g2 = color_vectors[1][1];
      b2 = color_vectors[1][2];
    }
    glTranslatef(-g_efieldCtrX, -g_efieldCtrY, 0);
    drawGLPoly(g_efieldBody, g_efieldBodySize, r1, g1, b1);    
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, r2, g2, b2);
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, 0,0,0, 1);
    glTranslatef(g_efieldCtrX, g_efieldCtrY, 0);
  }

  if(label != "") {
      glColor3f(label_color[0], label_color[1], label_color[2]);
    gl_font(1, 12);
    if(m_zoom > 4)
      gl_font(1, 14);
    double offset = 5.0;
    offset = offset * (1/m_zoom);

    int slen = label.length();
    char *buff = new char(slen+1);
    //glRasterPos3f(offset, offset, 0);
    glRasterPos3f(offset, offset, 0);
    strncpy(buff, label.c_str(), slen);
    buff[slen] = '\0';
    gl_draw(buff, slen);
    delete(buff);
  }

  glPopMatrix();
}


//-------------------------------------------------------------
// Procedure: drawVehicle(ObjectPose)

void SSV_Viewer::drawVehicle(string vname, bool active, string vehibody)
{
  ObjectPose opose;
  map<string,ObjectPose>::iterator p1;
  p1 = m_pos_map.find(vname);
  if(p1 != m_pos_map.end())
    opose = p1->second;
  else 
    return;

  vector<double> cvect;
  if(active)
    cvect = getColorMapping("active_vcolor", "red");
  else {
    if(hasColorMapping(vname))
      cvect = getColorMapping(vname, "1.0, 0.906, 0.243");
    else
      cvect = getColorMapping("inactive_vcolor", "1.0, 0.906, 0.243");
  }

  drawCommonVehicle(vname, opose, cvect[0], cvect[1], cvect[1], vehibody, 1);
}

//-------------------------------------------------------------
// Procedure: drawPoints

void SSV_Viewer::drawPoints(CPList &cps)
{
  if(!m_trails) 
    return;

  list<ColoredPoint>::iterator p, o;

  // Line colors
  double r=1.0;
  double g=0.0;
  double b=0.0;

  int i=0;
  double prev_x = 0;
  double prev_y = 0;
  for(p=cps.begin(); p!=cps.end(); p++) {
    if(m_trail_connect && (i!=0))
      drawSegment(prev_x, prev_y, p->m_x, p->m_y, r, g, b);
    if((i % m_trail_gap) == 0) {
      if(p->isValid()) 
	drawPoint(p->m_x, p->m_y, m_trail_color);
    }
    prev_x = p->m_x;
    prev_y = p->m_y;
    i++;
  }
}

//-------------------------------------------------------------
// Procedure: drawPoint

void SSV_Viewer::drawPoint(float px, float py, int color)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Determine position in terms of image percentage
  float pt_ix = meters2img('x', px);
  float pt_iy = meters2img('y', py);

  // Determine position in terms of view percentage
  float pt_vx = img2view('x', pt_ix);
  float pt_vy = img2view('y', pt_iy);

  glTranslatef(pt_vx, pt_vy, 0); // theses are in pixel units

  float pt_size = ((float)(m_trail_size) / 1.0) * m_zoom;

  glEnable(GL_POINT_SMOOTH);
  glPointSize(pt_size);
  
  if(color==0)  glColor3f(0, 0, 1);
  if(color==1)  glColor3f(0, 1, 0);
  if(color==2)  glColor3f(1, 0, 0);
  
  glBegin(GL_POINTS);
  glVertex2f(0, 0);
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: getObjectPoseByIndex

ObjectPose SSV_Viewer::getObjectPoseByIndex(int index)
{
  int ix = index;
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    if(ix==0)
      return(p->second);
    else
      ix--;
  }

  ObjectPose op;
  return(op);
}


//-------------------------------------------------------------
// Procedure: handleLeftMouse

void SSV_Viewer::handleLeftMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);

  m_left_click =  "x=" + doubleToString(sx,1) + ",";
  m_left_click += "y=" + doubleToString(sy,1);

  m_left_click_ix++;
  
  // cout << "Left Mouse click at [" << m_left_click << "] meters." << endl;
}

//-------------------------------------------------------------
// Procedure: handleRightMouse

void SSV_Viewer::handleRightMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);
  
  m_right_click =  "x=" + doubleToString(sx,1) + ",";
  m_right_click += "y=" + doubleToString(sy,1);

  bool ownship_position_known = false;
  ObjectPose opose;
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    string map_vname = tolower(p->first);
    if(map_vname == tolower(m_ownship_name)) {
      ownship_position_known = true;
      opose = p->second;
    }
  }
  
  cout << endl;
  if(ownship_position_known) {
    double osx = opose.getX();
    double osy = opose.getY();
    double osh = opose.getTheta();
    
    double range = hypot((osx-sx),(osy-sy));
    double relang = relAng(osx, osy, sx, sy);

    relang = angle360(relang - osh);
    m_right_click_rp  = "range=" + doubleToString(range,1) + ",";
    m_right_click_rp += "bearing=" + doubleToString(relang) + ",";
    m_right_click_rp += "contact=" + m_ownship_name;

    cout << "Right Mouse click at [" << m_right_click_rp << "] relpos." << endl;
    cout << "Ownship-X: " << osx << "  Ownship-Y: " << osy << endl;
  }
  m_right_click_ix++;

  
  cout << "Right Mouse click at [" << m_right_click << "] meters." << endl;
}


//-------------------------------------------------------------
// Procedure: setParam
//      Note: setBooleanOnString defined in MarineViewer.cpp

bool SSV_Viewer::setParam(string param, string value)
{
  if(MarineViewer::setCommonParam(param, value))
    return(true);
  
  param = tolower(stripBlankEnds(param));
  value = tolower(stripBlankEnds(value));
  
  if(param == "centric_view")
    return(setBooleanOnString(m_centric_view, value));
  else if(param == "bearing_lines")
    return(setBooleanOnString(m_draw_bearing_lines, value));
  else if(param == "bearing_color")
    return(setColorMapping("bearing_color", value));
  else if(param == "draw_marker_labels")
    m_vmarkers.setParam("viewable_labels", "toggle");
  else if(param == "marker_label_color")
    m_vmarkers.setParam("label_color", value);
  else if(param == "draw_markers")
    m_vmarkers.setParam("viewable_all", "toggle");
  else if(param == "marker_scale_all") {
    if(value == "smaller")
      m_vmarkers.setParam("mod_scale_all", 0.8);
    else if(value == "bigger")
      m_vmarkers.setParam("mod_scale_all", 1.2);
    else if(value == "reset")
      m_vmarkers.setParam("set_scale_all", 1.0);
    else
      return(false);
  }
  else if(param == "ownship_name")
    m_ownship_name = toupper(value);
  else if(param == "op_area")
    m_op_area = value;
  else
    return(false);

  redraw();
  return(true);

}

//-------------------------------------------------------------
// Procedure: setParam

bool SSV_Viewer::setParam(string param, float v)
{
  // Intercept and disallow any panning if in "centric" mode.
  if((param == "pan_x") || (param == "pan_y")) {
    m_centric_view = false;
    return(setCommonParam(param, v));
  }

  if(MarineViewer::setCommonParam(param, v))
    return(true);
  
  param = tolower(stripBlankEnds(param));
  
  if(param == "radial_size") {
    m_radial_size = (int)(v);
    if(m_radial_size < 0)
      m_radial_size = 0;
    if(m_radial_size > 2000)
       m_radial_size = 2000;
  }
  else if(param == "radial_increment") {
    m_radial_size += 100;
    if(m_radial_size > 2000)
      m_radial_size = 0;
  }
  else 
    return(false);
  
  redraw();
  return(true);

}

//-------------------------------------------------------------
// Procedure: drawRadials

void SSV_Viewer::drawRadials()
{
  // First determine the position of "ownship". If this isn't 
  // known we return right away.

  ObjectPose opose;
  map<string,ObjectPose>::iterator p1;
  p1 = m_pos_map.find(m_ownship_name);
  if(p1 != m_pos_map.end())
    opose = p1->second;
  else
    return;

  double px = opose.getX();
  double py = opose.getY();
  int    psize = 60;

  bool   dashed = false;

  string poly_str = "radial:";
  poly_str += doubleToString(px,2) + ",";
  poly_str += doubleToString(py,2) + ",";
  poly_str += doubleToString(m_radial_size,2) + ",";
  poly_str += intToString(psize);
  
  XYPolygon poly = stringToPoly(poly_str);

  float *points = new float[2*psize];
  
  int pindex = 0;
  for(int i=0; i<psize; i++) {
    points[pindex]   = poly.get_vx(i);
    points[pindex+1] = poly.get_vy(i);

    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();

    pindex += 2;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  // Draw the edges 
  glLineWidth(1.0);
  if(dashed) {
    glEnable(GL_LINE_STIPPLE);
    GLushort pattern = 0x5555;
    GLint factor = 5;
    glLineStipple(factor, pattern);
  }

  vector<double> cvect = getColorMapping("radial_color", "1,0,0");
  glColor3f(cvect[0], cvect[1], cvect[2]);

  glBegin(GL_LINE_LOOP);
  for(int j=0; j<psize*2; j=j+2) 
    glVertex2f(points[j], points[j+1]);

  glEnd();
  if(dashed)
    glDisable(GL_LINE_STIPPLE);

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawBearingLine

void SSV_Viewer::drawBearingLine(int index)
{
  string vname = getVehiName(index);
  if((vname=="") || (vname == m_ownship_name) || (m_ownship_name==""))
    return;
  
  map<string,ObjectPose>::iterator p;
  p = m_pos_map.find(vname);
  if(p == m_pos_map.end())
    return;
  ObjectPose pose_vh = p->second;

  p = m_pos_map.find(m_ownship_name);
  if(p == m_pos_map.end())
    return;
  ObjectPose pose_os = p->second;

  float pt_0 = pose_os.getX() * m_back_img.get_pix_per_mtr();
  float pt_1 = pose_os.getY() * m_back_img.get_pix_per_mtr();
  float pt_2 = pose_vh.getX() * m_back_img.get_pix_per_mtr();
  float pt_3 = pose_vh.getY() * m_back_img.get_pix_per_mtr();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  vector<double> cvect = getColorMapping("bearing_color", "orange");

  // Draw the edges 
  glLineWidth(1.0);
  glColor3f(cvect[0], cvect[1], cvect[2]);
  glBegin(GL_LINE_STRIP);

  glBegin(GL_LINE_STRIP);
  glVertex2f(pt_0, pt_1);
  glVertex2f(pt_2, pt_3);
  glEnd();

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawOpAreaGrid

void SSV_Viewer::drawOpAreaGrid()
{
  if(m_op_area == "dabob_bay")
    drawGridPN();
  else if(m_op_area == "tellaro")
    drawGridBox(0.0,    1542.6,	  1510.2,  2690.7,
		2839.8, 1227.0,	  1382.2,     0.0);
  else if((m_op_area == "palmara") || (m_op_area == "palmaria"))
    drawGridBox(0.0,       0.0,      0.0,  1347.0,
		1071.3, 1347.0,   1071.3,     0.0);
  else if(m_op_area == "lotti")
    drawGridBox(926.7,   749.9,    1048.1,  642.4,
		467.0,     0.0,       0.0,  342.0);
}


//-------------------------------------------------------------
// Procedure: drawGridBox

void SSV_Viewer::drawGridBox(double p0x, double p0y,
			     double p1x, double p1y,
			     double p2x, double p2y,
			     double p3x, double p3y)
{
  unsigned int i;

  unsigned int    vsize  = 4;
  float *points = new float[2*4];

  points[0]  = p0x;    points[1] = p0y;
  points[2]  = p1x;    points[3] = p1y;
  points[4]  = p2x;    points[5] = p2y;
  points[6]  = p3x;    points[7] = p3y;

  unsigned int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();
    pindex+=2;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  // Draw the edges 
  glLineWidth(2.0);
  glColor3f(0.0, 0.7, 0.5);
  glBegin(GL_LINE_STRIP);

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*0],  points[2*0+1]);
  glVertex2f(points[2*1],  points[2*1+1]);
  glVertex2f(points[2*2],  points[2*2+1]);
  glVertex2f(points[2*3],  points[2*3+1]);
  glVertex2f(points[2*0],  points[2*0+1]);
  glEnd();

  glFlush();
  glPopMatrix();
}


//-------------------------------------------------------------
// Procedure: drawGridPN

void SSV_Viewer::drawGridPN()
{
  unsigned int i;

  unsigned int vsize     = 18;
  float *points = new float[2*18];

  points[0]  =   932;    points[1]  =  4339;
  points[2]  =  1905;    points[3]  =  4122;
  points[4]  =  2879;    points[5]  =  3905;
  points[6]  =   501;    points[7]  =  2385;
  points[8]  =  1475;    points[9]  =  2170;
  points[10] =  2449;    points[11] =  1953;
  points[12] =    72;    points[13] =   433;
  points[14] =  1046;    points[15] =   218;
  points[16] =  2019;    points[17] =     1;
  points[18] =  -357;    points[19] = -1518;
  points[20] =   615;    points[21] = -1698;
  points[22] =  1588;    points[23] = -1950;
  points[24] =  -787;    points[25] = -3472;
  points[26] =   185;    points[27] = -3687;
  points[28] =  1159;    points[29] = -3902;
  points[30] = -1216;    points[31] = -5424;
  points[32] =  -243;    points[33] = -5639;
  points[34] =   729;    points[35] = -5855;

  unsigned int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();
    pindex+=2;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  // Draw the edges 
  glLineWidth(1.0);

  glColor3f(0.0, 0.7, 0.5);

  glBegin(GL_LINE_STRIP);

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*0],  points[2*0+1]);
  glVertex2f(points[2*3],  points[2*3+1]);
  glVertex2f(points[2*6],  points[2*6+1]);
  glVertex2f(points[2*9],  points[2*9+1]);
  glVertex2f(points[2*12], points[2*12+1]);
  glVertex2f(points[2*15], points[2*15+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*1],  points[2*1+1]);
  glVertex2f(points[2*4],  points[2*4+1]);
  glVertex2f(points[2*7],  points[2*7+1]);
  glVertex2f(points[2*10], points[2*10+1]);
  glVertex2f(points[2*13], points[2*13+1]);
  glVertex2f(points[2*16], points[2*16+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*2],  points[2*2+1]);
  glVertex2f(points[2*5],  points[2*5+1]);
  glVertex2f(points[2*8],  points[2*8+1]);
  glVertex2f(points[2*11], points[2*11+1]);
  glVertex2f(points[2*14], points[2*14+1]);
  glVertex2f(points[2*17], points[2*17+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*0], points[2*0+1]);
  glVertex2f(points[2*2], points[2*2+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*3], points[2*3+1]);
  glVertex2f(points[2*5], points[2*5+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*6], points[2*6+1]);
  glVertex2f(points[2*8], points[2*8+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*9],  points[2*9+1]);
  glVertex2f(points[2*11], points[2*11+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*12], points[2*12+1]);
  glVertex2f(points[2*14], points[2*14+1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[2*15], points[2*15+1]);
  glVertex2f(points[2*17], points[2*17+1]);
  glEnd();

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: setCurrent
//            Given a vehicle name string, compare it to the map
//            of known vehicles and, if found, let the 
//            "global_index" point to that index. The global_index
//            indicates which vehicle is "active". 

void SSV_Viewer::setCurrent(string vname)
{
  vname = tolower(vname);

  // Special case: the alias "ownship" can be used even if the value
  // of m_ownship_name is something completely different
  if(vname == "ownship")
    vname = tolower(m_ownship_name);

  int the_index = -1;
  int cur_index = 0;
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    string map_vname = tolower(p->first);
    if(map_vname == vname)
      the_index = cur_index;
    cur_index++;
  }
  
  if(the_index != -1)
    m_global_ix = the_index;
}

//-------------------------------------------------------------
// Procedure: cycleIndex

void SSV_Viewer::cycleIndex()
{
  m_global_ix += 1;
  if(m_global_ix >= m_pos_map.size())
    m_global_ix = 0;
}

//-------------------------------------------------------------
// Procedure: addVMarker()


bool SSV_Viewer::addVMarker(const std::string& mline)
{
  vector<string> svector = parseString(mline, ',');
  unsigned int vsize = svector.size();

  string mtype, xpos, ypos, lat, lon, scale, label, colors;
  for(unsigned int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> ivector = parseString(svector[i], '=');
    if(ivector.size() != 2)
      return(false);
    string left  = tolower(stripBlankEnds(ivector[0]));
    string right = stripBlankEnds(ivector[1]);
    if(left == "type")        mtype = right;
    else if(left == "xpos")   xpos = right;
    else if(left == "ypos")   ypos = right;
    else if(left == "scale")  scale = right;
    else if(left == "lat")    lat = right;
    else if(left == "lon")    lon = right;
    else if(left == "label")  label = right;
    else if(left == "colors") colors = right;
  }

  if((mtype==""))
    return(false);
  
  // The position has to be fully specified in terms of either lat/lon
  // of the x-y position in local coords. Otherwise return(false);
  if((lat=="")||(lon=="")||(!isNumber(lat))||(!isNumber(lon)))
    if((xpos=="")||(ypos=="")||(!isNumber(xpos))||(!isNumber(ypos)))
      return(false);

  double xpos_d, ypos_d;
  if((lat=="")||(lon=="")||(!isNumber(lat))||(!isNumber(lon))) {
    xpos_d  = atof(xpos.c_str());
    ypos_d  = atof(ypos.c_str());
  }
  else {
    double lat_d = atof(lat.c_str());
    double lon_d = atof(lon.c_str());
    m_geodesy.LatLong2LocalGrid(lat_d, lon_d, ypos_d, xpos_d);
  }

  double scale_d = atof(scale.c_str());
  if(scale_d < 0)
    scale_d = 0;
  
  m_vmarkers.addVMarker(mtype, xpos_d, ypos_d, scale_d, label, colors);
  return(true);
}

//-------------------------------------------------------------
// Procedure: hasVehiName
//            Given a vehicle name string, compare it to the map
//            of known vehicles and, if found, return true.

bool SSV_Viewer::hasVehiName(string vname)
{
  vname = tolower(vname);

  // Special case: the alias "ownship" can be used even if the value
  // of m_ownship_name is something completely different
  if(vname == "ownship")
    vname = tolower(m_ownship_name);
  
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    string map_vname = tolower(p->first);
    if(map_vname == vname)
      return(true);
  }
  return(false);
}

//-------------------------------------------------------------
// Procedure: addStationCircle

void SSV_Viewer::addStationCircle(const XYCircle& new_circ)
{
  string new_label = new_circ.getLabel();
  
  // Station Keeping Circles must have a label or else disregard
  if(new_label == "")
    return;
  
  bool prior_existed = false;
  
  int vsize = m_station_circ.size();
  for(int i=0; i<vsize; i++) {
    if(m_station_circ[i].getLabel() == new_label) {
      m_station_circ[i] = new_circ;
      prior_existed = true;
    }
  }
  
  if(!prior_existed)
    m_station_circ.push_back(new_circ);
}


//-------------------------------------------------------------
// Procedure: initGeodesy

bool SSV_Viewer::initGeodesy(double lat, double lon)
{
  return(m_geodesy.Initialise(lat, lon));
}


//-------------------------------------------------------------
// Procedure: drawStationCircles

void SSV_Viewer::drawStationCircles()
{
  int vsize = m_station_circ.size();
  for(int i=0; i<vsize; i++)
    drawCirc(m_station_circ[i], 30, true, 0,0,0.6, 0,0,0.4);
}

//-------------------------------------------------------------
// Procedure: drawCircle

void SSV_Viewer::drawCirc(XYCircle dcircle, int pts, bool filled,
			  double l_red, double l_grn, double l_blu,
			  double f_red, double f_grn, double f_blu)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glLineWidth(1.0);  // added dec1306
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  double px  = dcircle.getX();
  double py  = dcircle.getY();
  double rad = dcircle.getRad();

  string poly_str = "radial:";
  poly_str += doubleToString(px,2) + ",";
  poly_str += doubleToString(py,2) + ",";
  poly_str += doubleToString(rad,2) + ",";
  poly_str += intToString(pts);
  
  XYPolygon poly = stringToPoly(poly_str);

  // Now set points to the actual size vs. the requested size
  unsigned int actual_pts = poly.size();

  if(actual_pts == 0)
    return;

  unsigned int i;
  float *points = new float[2 * actual_pts];
  unsigned int pindex = 0;
  for(i=0; i<actual_pts; i++) {
    points[pindex]   = poly.get_vx(i);
    points[pindex+1] = poly.get_vy(i);

    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();
    pindex += 2;
  }

  glColor3f(l_red, l_grn, l_blu);
  glBegin(GL_LINE_LOOP);
  for(i=0; i<actual_pts*2; i=i+2) {
    glVertex2f(points[i], points[i+1]);
  }
  glEnd();

  // If filled option is on, draw the interior of the circle
  if(filled) {
    glEnable(GL_BLEND);
    glColor4f(f_red,f_grn,f_blu,0.1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    for(i=0; i<actual_pts*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
    glDisable(GL_BLEND);
  }
  

  delete [] points;
  glFlush();
  glPopMatrix();

}

//-------------------------------------------------------------
// Procedure: drawMarkers

void SSV_Viewer::drawMarkers()
{
  double gscale = m_vmarkers.getMarkerGScale(); 

  vector<vector<double> > color_vectors;
  vector<double> label_color = m_vmarkers.getLabelColor();
  
  unsigned int vsize = m_vmarkers.size();
  for(unsigned int i=0; i<vsize; i++) {
    string mtype = m_vmarkers.getMarkerType(i);
    string label;
    if(m_vmarkers.viewable("labels"))
      label = m_vmarkers.getMarkerLabel(i);
    double xpos  = m_vmarkers.getMarkerXPos(i);
    double ypos  = m_vmarkers.getMarkerYPos(i);
    double scale = m_vmarkers.getMarkerScale(i) * gscale;
    color_vectors = m_vmarkers.getMarkerColorVectors(i);
    drawCommonMarker(xpos, ypos, scale, mtype, label, label_color, color_vectors);
  }
}

