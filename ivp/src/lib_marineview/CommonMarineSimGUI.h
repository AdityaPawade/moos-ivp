/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CommonMarineSimGUI.h                                 */
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

#ifndef COMMON_MARINE_SIM_GUI_HEADER
#define COMMON_MARINE_SIM_GUI_HEADER

#include "CommonMarineGUI.h"

class CommonMarineSimGUI : public CommonMarineGUI {
public:
  CommonMarineSimGUI(int w, int h, const char *l=0);
  virtual ~CommonMarineSimGUI() {};

  //  virtual void updateInfoBHV() {};

protected:
  inline void cb_ToggleTrails_i();
  static void cb_ToggleTrails(Fl_Widget*);

  inline void cb_ToggleTrailColor_i();
  static void cb_ToggleTrailColor(Fl_Widget*);

  inline void cb_AltTrailGap_i(int);
  static void cb_AltTrailGap(Fl_Widget*, int);

  inline void cb_AltTrailSize_i(int);
  static void cb_AltTrailSize(Fl_Widget*, int);

  inline void cb_AltShapeScale_i(int);
  static void cb_AltShapeScale(Fl_Widget*, int);

  inline void cb_DataView_i(int);
  static void cb_DataView(Fl_Widget*, int);
};
#endif

