/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FV_Viewer.cpp                                        */
/*    DATE: May 12th 2006                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include "FV_Viewer.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

FV_Viewer::FV_Viewer(int x, int y, int wid, int hgt, const char *l)
  : Common_IPFViewer(x, y, wid, hgt, l)
{
  m_model = 0;

  m_polar = 1;
  m_draw_pclines = false;
  
  m_clear_color.setColor("macbeige");
}

//-------------------------------------------------------------
// Procedure: resetQuadSet

void FV_Viewer::resetQuadSet()
{
  if(!m_model)
    return;

  m_quadset = m_model->getQuadSet();

  resetRadVisuals();

  m_draw_pclines = true;
  m_quadset.normalize(0, 100);
  m_quadset.applyColorMap(m_color_map);	
  m_quadset.applyColorIntensity(m_intensity);
  m_quadset.interpolate(1);
  
  if(m_polar == 0)
    m_quadset.applyTranslation(-250, -250);
  else if(m_polar == 1)
    m_quadset.applyPolar(m_rad_ratio, 1);
  else if(m_polar == 2)
    m_quadset.applyPolar(m_rad_ratio, 2);
}


//-------------------------------------------------------------
// Procedure: draw

void FV_Viewer::draw()
{
  Common_IPFViewer::draw();
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);
  
  if(m_draw_ipf)
    Common_IPFViewer::drawQuadSet(m_quadset);
 
  if(m_draw_frame && (m_polar==0))
    drawFrame();
  if(m_draw_frame && (m_polar==1)) 
    drawPolarFrame();

  double heading = 0;
  if(m_model)
    heading = m_model->getNavHeading();
  
  if(m_draw_ship && (m_polar==1)) 
    drawCenteredShip(heading);

  drawOwnPoint();
    
  if(m_draw_pin) {
    unsigned int max_crs_qix = m_quadset.getMaxPointQIX("course");
    unsigned int max_spd_qix = m_quadset.getMaxPointQIX("speed");
    drawMaxPoint(max_crs_qix, max_spd_qix);
  }

  glPopMatrix();
  
  glFlush();  
}
