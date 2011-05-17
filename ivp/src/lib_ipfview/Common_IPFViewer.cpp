/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Common_IPFViewer.cpp                                 */
/*    DATE: Feb 13, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstring>
#include "Common_IPFViewer.h"
#include "GeomUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

Common_IPFViewer::Common_IPFViewer(int g_x, int g_y, int g_width, 
		     int g_height, const char *g_l)
 : Fl_Gl_Window(g_x, g_y, g_width, g_height, g_l)
{
  m_xRot         = -72;
  m_zRot         = 40;
  m_zoom         = 1;
  m_rad_extra    = 8;
  m_draw_pin     = true;
  m_draw_frame   = true;
  m_draw_base    = true;
  m_polar        = 1; 

  setParam("clear_color", "white");
  setParam("frame_color", "dark_red");

  m_frame_height = 250;
}

//-------------------------------------------------------------
// Procedure: handle()

int Common_IPFViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));  // temp measure??

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
// Procedure: setParam

bool Common_IPFViewer::setParam(string param, string value)
{
  value = tolower(stripBlankEnds(value));

  if((param == "draw_frame") && (value == "toggle")) {
    if(!m_draw_frame) {
      m_draw_frame = true;
      m_draw_base  = true;
    }
    else if(m_draw_frame && m_draw_base)
      m_draw_base = false;
    else if (m_draw_frame && !m_draw_base)
      m_draw_frame = false;
  }
  else if((param == "draw_frame") && (value == "true"))
    m_draw_frame = true;
  else if((param == "draw_frame") && (value == "false"))
    m_draw_frame = false;
  else if((param == "draw_base") && (value == "true"))
    m_draw_base = true;
  else if((param == "draw_base") && (value == "false"))
    m_draw_base = false;
  else if(param == "draw_pin")
    setBooleanOnString(m_draw_pin, value);
  else if(param == "reset_view") {
    if(value=="1")
      {m_xRot=-78; m_zRot=40;}
    else if(value=="2")
      {m_xRot=0;   m_zRot=0;}
    else if(value=="3")
      {m_xRot=-53; m_zRot=0;}
    else if(value=="4")
      {m_xRot=-72; m_zRot=122;}
  }
  else if(param == "frame_color")
    m_frame_color.setColor(value);
  else if((param == "clear_color") || (param == "back_color"))
    m_clear_color.setColor(value);
  else
    return(false);

  redraw();
  return(true);
}

//-------------------------------------------------------------
// Procedure: setParam

bool Common_IPFViewer::setParam(string param, double value)
{
  if(param == "mod_frame_height")
    m_frame_height += value;
  else if(param == "set_frame_height")
    m_frame_height = value;
  else if(param == "mod_zoom")
    m_zoom *= value;
  else if(param == "set_zoom")
    m_zoom = value;
  else if(param == "mod_radius")
    m_rad_extra *= value;
  else if(param == "set_radius")
    m_rad_extra = value;
  else if(param == "mod_x_rotation")
    m_xRot += value;
  else if(param == "set_x_rotation")
    m_xRot = value;
  else if(param == "mod_z_rotation")
    m_zRot += value;
  else if(param == "set_z_rotation")
    m_zRot = value;
  else
    return(false);

  redraw();
  return(true);
}

//-------------------------------------------------------------
// Procedure: printParams

void Common_IPFViewer::printParams()
{
  cout << "# lib_ipfview Common_IPFViewer -----------------" << endl;
  cout << "set_frame_height=" << m_frame_height << endl;
  cout << "set_zoom="         << m_zoom         << endl;
  cout << "set_radius="       << m_rad_extra    << endl;
  cout << "set_x_rotation="   << m_xRot         << endl;
  cout << "set_z_rotation="   << m_zRot         << endl;
  if(m_draw_frame)
    cout << "draw_frame=true"  << endl;
  else
    cout << "draw_frame=false" << endl;
  if(m_draw_base)
    cout << "draw_base=true"   << endl;
  else
    cout << "draw_base=false"  << endl;
}

//-------------------------------------------------------------
// Procedure: draw()

void Common_IPFViewer::draw()
{
  // Clear the window and the depth buffer
  
  double clear_red = m_clear_color.red();
  double clear_grn = m_clear_color.grn();
  double clear_blu = m_clear_color.blu();
  glClearColor(clear_red, clear_grn, clear_blu, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0,0,w(),h());

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(0.0, 0.25, 0.0);

  glScalef(m_zoom, m_zoom, m_zoom);

  //GLfloat nRange = 450.0f;
  GLfloat nRange = 650.0f;

  // Reset projection matrix stack
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // Establish clipping volume (left, right, bottom, top, near, far)
  if(w() <= h()) 
    glOrtho (-nRange, nRange, -nRange*h()/w(), nRange*h()/w(), -nRange, nRange);
  else 
    glOrtho (-nRange*w()/h(), nRange*w()/h(), -nRange, nRange, -nRange, nRange);
  //glOrtho (-nRange*w()/h()-100, nRange*w()/h()+100, -nRange-100, 
  //     nRange+100, -nRange-100, nRange+100);

  // Set color shading model to flat
  glShadeModel(GL_FLAT);
  
  // Clock wise wound polygons are front facing, this is reversed
  // because we are using triangle fans
  glFrontFace(GL_CW);

  bool iCull=false;
  bool iDepth=true;
  //bool iOutline=false;

  // Turn culling on if flag is set
  if(iCull)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
  
  // Enable depth testing if flag is set
  if(iDepth)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}


//-------------------------------------------------------------
// Procedure: drawIvPFunction

bool Common_IPFViewer::drawIvPFunction()
{
  unsigned int i, quad_cnt = m_quadset.size();
  if(quad_cnt == 0)
    return(false);

  for(i=0; i<quad_cnt; i++) {
    Quad3D quad = m_quadset.getQuad(i);
    drawQuad(quad);
  }
  return(true);
}

//-------------------------------------------------------------
// Procedure: drawQuad

void Common_IPFViewer::drawQuad(Quad3D &q)

{
  double m_intensity = 1.0;

  if(m_polar == 2) {
    q.xl *= m_rad_extra;
    q.xh *= m_rad_extra;
  }
  else if(m_polar == 1) {
    q.yl *= m_rad_extra;
    q.yh *= m_rad_extra;
  }

  q.llval_r *= m_intensity;   q.hlval_r *= m_intensity;
  q.llval_g *= m_intensity;   q.hlval_g *= m_intensity;
  q.llval_b *= m_intensity;   q.hlval_b *= m_intensity;

  q.hhval_r *= m_intensity;   q.lhval_r *= m_intensity;
  q.hhval_g *= m_intensity;   q.lhval_g *= m_intensity;
  q.hhval_b *= m_intensity;   q.lhval_b *= m_intensity;

  q.llval = (q.llval * q.scale) + q.base; 
  q.hlval = (q.hlval * q.scale) + q.base; 
  q.hhval = (q.hhval * q.scale) + q.base; 
  q.lhval = (q.lhval * q.scale) + q.base; 

  double x0,x1,x2,x3,y0,y1,y2,y3;
  if(m_polar == 1) {
    double delta = 360.0 / q.xpts;
    projectPoint(q.xl*delta, q.yl, 0, 0, x0, y0);
    projectPoint(q.xh*delta, q.yl, 0, 0, x1, y1);
    projectPoint(q.xh*delta, q.yh, 0, 0, x2, y2);
    projectPoint(q.xl*delta, q.yh, 0, 0, x3, y3);
  }
  else if(m_polar == 2) {
    double delta = 360.0 / q.ypts;
    projectPoint(q.yl*delta, q.xl, 0, 0, y0, x0);
    projectPoint(q.yh*delta, q.xl, 0, 0, y1, x1);
    projectPoint(q.yh*delta, q.xh, 0, 0, y2, x2);
    projectPoint(q.yl*delta, q.xh, 0, 0, y3, x3);
  }      
  else {
    q.xl -= 250;  q.xh -= 250; q.yl -= 250;  q.yh -= 250;
    x0=q.xl; x1=q.xh; x2=q.xh; x3=q.xl;
    y0=q.yl; y1=q.yl; y2=q.yh; y3=q.yh;
  }

  glShadeModel(GL_SMOOTH);
  glBegin(GL_TRIANGLE_FAN);
  glColor3f(q.llval_r, q.llval_g, q.llval_b);
  glVertex3f(x0, y0, q.llval);
  
  glColor3f(q.hlval_r, q.hlval_g, q.hlval_b);
  glVertex3f(x1, y1, q.hlval);
  
  glColor3f(q.hhval_r, q.hhval_g, q.hhval_b);
  glVertex3f(x2, y2, q.hhval);
  
  glColor3f(q.lhval_r, q.lhval_g, q.lhval_b);
  glVertex3f(x3, y3, q.lhval);
  glEnd();

  if(q.lines) {
    glLineWidth(0.5);
    glColor3f(1.0, 1.0, 1.0);
    //glColor3f(0.4, 0.4, 0.4);
    //glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_LINE_STRIP);
    glVertex3f(x0, y0, q.llval+1);
    glVertex3f(x1, y1, q.hlval+1);
    glVertex3f(x2, y2, q.hhval+1);
    glVertex3f(x3, y3, q.lhval+1);
    glEnd();
    glLineWidth(1.0);
  }
}

//-------------------------------------------------------------
// Procedure: drawFrame

void Common_IPFViewer::drawFrame()
{
  double w = 250;

  double b = -250;
  double t = -250 + (m_frame_height);

  double frame_red = m_frame_color.red();
  double frame_grn = m_frame_color.grn();
  double frame_blu = m_frame_color.blu();

  glColor3f(frame_red/2, frame_grn/2, frame_blu/2);
  //Color3f(0.6f, 0.4f, 0.6f);
  //glColor3f(cvect[0]/2, cvect[1]/2, cvect[2]/2);
  glShadeModel(GL_FLAT);
  
  // Either draw a full base or just the frame
  if(m_draw_base) {
    glBegin(GL_TRIANGLES);
    glVertex3f(-w,-w, b); 
    glVertex3f( w,-w, b); 
    glVertex3f( w, w, b);
    glVertex3f( w, w, b); 
    glVertex3f(-w, w, b); 
    glVertex3f(-w,-w, b);
    glEnd();
  }
  
  glColor3f(frame_red, frame_grn, frame_blu);

  if(!m_draw_base) {
    glBegin(GL_LINE_STRIP);
    glVertex3f(-w,-w, b);  
    glVertex3f(w, -w, b);  
    glVertex3f(w,  w, b);
    glVertex3f(-w, w, b);  
    glVertex3f(-w,-w, b);
    glEnd();
  }

  //glColor3f(0.8f, 0.3f, 0.8f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(-w,-w, t);  
  glVertex3f(w, -w, t);  
  glVertex3f(w,  w, t);
  glVertex3f(-w, w, t);  
  glVertex3f(-w,-w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(-w,-w, b);  
  glVertex3f(-w,-w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(-w, w, b);  
  glVertex3f(-w, w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(w, w, b);     
  glVertex3f(w, w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(w, -w, b);  
  glVertex3f(w, -w, t);
  glEnd();

  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawOwnPoint

void Common_IPFViewer::drawOwnPoint()
{
  if((m_xRot != 0) || (m_zRot != 0))
    return;
  if(m_quadset.size() == 0)
    return;

  double w = 250;

  glPointSize(3.0 * m_zoom);

  glColor3f(1.0f, 1.0f, 1.0f);
  glShadeModel(GL_FLAT);
  
  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  glVertex3f(0, 0, w); 
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawMaxPoint

void Common_IPFViewer::drawMaxPoint(double crs, double spd)
{
  if(m_quadset.size() == 0)
    return;

  spd *= m_rad_extra;
  double x,y,z=250;
  projectPoint(crs, spd, 0, 0, x, y);
    
  glPointSize(3.0 * m_zoom);

  glColor3f(1.0f, 0.5, 1.0f);
  glShadeModel(GL_FLAT);

  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  glVertex3f(x, y, z); 
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glBegin(GL_LINE_STRIP);
  glVertex3f(x, y, z); 
  glVertex3f(x, y, -z); 
  glEnd();

  glFlush();
}

