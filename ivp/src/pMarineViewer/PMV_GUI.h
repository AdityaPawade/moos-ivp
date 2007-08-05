/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_GUI.h                                            */
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

#ifndef PMV_GUI_HEADER
#define PMV_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Hold_Browser.H>
#include "MarineViewer.h"
#include "CommonMarineSimGUI.h"
#include "MY_Output.h"
#include "MY_Hold_Browser.h"

class PMV_GUI : public CommonMarineSimGUI {
public:
  PMV_GUI(int w, int h, const char *l=0);
  virtual ~PMV_GUI() {};

  void updateXY();
  int  handle(int);
  void setCurrTime(double v) {curr_time = v;};
  
  MarineViewer *mviewer;

protected:
  void augmentMenu();

  MY_Output   *v_nam;
  MY_Output   *time;
  MY_Output   *x_mtr;
  MY_Output   *y_mtr;
  MY_Output   *x_lon;
  MY_Output   *y_lat;
  MY_Output   *v_spd;
  MY_Output   *v_crs;
  MY_Output   *v_dep;

  MY_Hold_Browser *b_items;

  double curr_time;

protected:
  inline void cb_ToggleGridsTop_i();
  static void cb_ToggleGridsTop(Fl_Widget*);

  inline void cb_ItemSelect_i();
  static void cb_ItemSelect(Fl_Widget*);

};
#endif

