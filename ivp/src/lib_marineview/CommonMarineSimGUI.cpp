/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CommonMarineSimGUI.cpp                               */
/*    DATE: November, 2004                                       */
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
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "CommonMarineSimGUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

CommonMarineSimGUI::CommonMarineSimGUI(int g_w, int g_h, const char *g_l)
  : CommonMarineGUI(g_w, g_h, g_l) 
{
  mbar->add("ForeView/Focus Vehicle 0", '0', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)0, 0);
  mbar->add("ForeView/Focus Vehicle 1", '1', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)1, 0);
  mbar->add("ForeView/Focus Vehicle 2", '2', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)2, 0);
  mbar->add("ForeView/Focus Vehicle 3", '3', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)3, 0);
  mbar->add("ForeView/Focus Vehicle 4", '4', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)4, 0);
  mbar->add("ForeView/Focus Vehicle 5", '5', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)5, 0);
  mbar->add("ForeView/Focus Vehicle 6", '6', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)6, 0);
  mbar->add("ForeView/Focus Vehicle 7", '7', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)7, 0);
  mbar->add("ForeView/Focus Vehicle 8", '8', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)8, 0);
  mbar->add("ForeView/Focus Vehicle 9", '9', (Fl_Callback*)CommonMarineSimGUI::cb_DataView, (void*)9, FL_MENU_DIVIDER);
  mbar->add("ForeView/Shape Larger",    '+', (Fl_Callback*)CommonMarineSimGUI::cb_AltShapeScale, (void*)+1, 0);
  mbar->add("ForeView/Shape Smaller",   '-', (Fl_Callback*)CommonMarineSimGUI::cb_AltShapeScale, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("ForeView/Toggle Trails",   't', (Fl_Callback*)CommonMarineSimGUI::cb_ToggleTrails, 0, 134);
  mbar->add("ForeView/Toggle TrailColor", 'T', (Fl_Callback*)CommonMarineSimGUI::cb_ToggleTrailColor, 0, 0);
  mbar->add("ForeView/More Trail Gap",  '}', (Fl_Callback*)CommonMarineSimGUI::cb_AltTrailGap, (void*)1,  0);
  mbar->add("ForeView/Less Trail Gap",  '{', (Fl_Callback*)CommonMarineSimGUI::cb_AltTrailGap, (void*)-1, 0);
  mbar->add("ForeView/More Trail Size", ']', (Fl_Callback*)CommonMarineSimGUI::cb_AltTrailSize, (void*)+1, 0);
  mbar->add("ForeView/Less Trail Size", '[', (Fl_Callback*)CommonMarineSimGUI::cb_AltTrailSize, (void*)-1, FL_MENU_DIVIDER);
}

//----------------------------------------- ToggleTrails
inline void CommonMarineSimGUI::cb_ToggleTrails_i() {
  cmviewer->toggleTrails();
}
void CommonMarineSimGUI::cb_ToggleTrails(Fl_Widget* o) {
  ((CommonMarineSimGUI*)(o->parent()->user_data()))->cb_ToggleTrails_i();
}

//----------------------------------------- ToggleTrailColor
inline void CommonMarineSimGUI::cb_ToggleTrailColor_i() {
  cmviewer->toggleTrailColor();
}
void CommonMarineSimGUI::cb_ToggleTrailColor(Fl_Widget* o) {
  ((CommonMarineSimGUI*)(o->parent()->user_data()))->cb_ToggleTrailColor_i();
}

//----------------------------------------- Alt Trail Gap
inline void CommonMarineSimGUI::cb_AltTrailGap_i(int delta) {
  cmviewer->alterTrailGap(delta);
}
void CommonMarineSimGUI::cb_AltTrailGap(Fl_Widget* o, int v) {
  ((CommonMarineSimGUI*)(o->parent()->user_data()))->cb_AltTrailGap_i(v);
}

//----------------------------------------- Alt Trail Size
inline void CommonMarineSimGUI::cb_AltTrailSize_i(int delta) {
  cmviewer->alterTrailSize((float)(delta/5.0));
}
void CommonMarineSimGUI::cb_AltTrailSize(Fl_Widget* o, int v) {
  ((CommonMarineSimGUI*)(o->parent()->user_data()))->cb_AltTrailSize_i(v);
}

//----------------------------------------- Alt Shape Scale
inline void CommonMarineSimGUI::cb_AltShapeScale_i(int delta) {
  if(delta > 0)
    cmviewer->alterShapeScale(1.25);
  if(delta < 0)
    cmviewer->alterShapeScale(0.80);
}
void CommonMarineSimGUI::cb_AltShapeScale(Fl_Widget* o, int v) {
  ((CommonMarineSimGUI*)(o->parent()->user_data()))->cb_AltShapeScale_i(v);
}

//----------------------------------------- DataView
inline void CommonMarineSimGUI::cb_DataView_i(int val) {
  cmviewer->setGlobalIndex(val);
  cmviewer->redraw();
  this->updateXY();
  //  this->updateInfoBHV();
}
void CommonMarineSimGUI::cb_DataView(Fl_Widget* o, int v) {
  ((CommonMarineSimGUI*)(o->parent()->user_data()))->cb_DataView_i(v);
}

