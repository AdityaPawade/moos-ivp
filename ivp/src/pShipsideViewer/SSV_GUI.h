/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_GUI.h                                            */
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

#ifndef SSV_GUI_HEADER
#define SSV_GUI_HEADER

#include "SSV_Viewer.h"
#include "MarineVehiGUI.h"
#include "MY_Output.h"
#include "MY_Button.h"
#include "FL/Fl_Box.H"

class SSV_GUI : public MarineVehiGUI {
public:
  SSV_GUI(int w, int h, const char *l=0);
  virtual ~SSV_GUI() {};

  void updateXY();
  int  handle(int);
  void augmentMenu();
  void addContactButton(int ix, std::string vname);

  SSV_Viewer *mviewer;

private:
  inline void cb_CentricToggle_i(int);
  static void cb_CentricToggle(Fl_Widget*, int);

  inline void cb_Radial_i(int);
  static void cb_Radial(Fl_Widget*, int);

  inline void cb_ButtonView_i(int);
  static void cb_ButtonView(Fl_Widget*, int);

protected:
  MY_Output  *v_nam;
  MY_Output  *time;
  MY_Output  *x_mtr;
  MY_Output  *y_mtr;
  MY_Output  *v_spd;
  MY_Output  *v_crs;
  MY_Output  *v_dep;
  MY_Output  *v_ais;

  Fl_Box     *m_deploy_box_text;
  Fl_Box     *m_deploy_box_body;
  Fl_Box     *m_station_box_body;

  Fl_Box     *m_cdeploy_box_text;
  Fl_Box     *m_cdeploy_box_body;
  Fl_Box     *m_cstation_box_body;

  MY_Button  *m_deploy_all_on;
  MY_Button  *m_deploy_all_off;
  MY_Button  *m_station_all_on;
  MY_Button  *m_station_all_off;
  MY_Button  *m_deploy_cur_on;
  MY_Button  *m_deploy_cur_off;
  MY_Button  *m_station_cur_on;
  MY_Button  *m_station_cur_off;

  MY_Button  *ownship_b0;
  MY_Button  *contact_b1;
  MY_Button  *contact_b2;
  MY_Button  *contact_b3;
  MY_Button  *contact_b4;
  MY_Button  *contact_b5;
  MY_Button  *contact_b6;
};
#endif

