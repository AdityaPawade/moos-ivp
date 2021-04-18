/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_GUI.h                                       */
/*    DATE: June 2nd, 2015                                       */
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

#ifndef ZAIC_SPD_GUI_HEADER
#define ZAIC_SPD_GUI_HEADER

#include "ZAIC_GUI.h"

class ZAIC_SPD_GUI : public ZAIC_GUI {
 public:
  ZAIC_SPD_GUI(int w, int h, const char *label=0);
  ~ZAIC_SPD_GUI() {};
  
  void updateOutput(); // overloading virtual function of ZAIC_GUI
  void augmentMenu();

 protected:
  void initWidgets();
  void updateFieldColors();
  
 public:
  Fl_Output   *m_fld_medval;
  Fl_Output   *m_fld_lowval;
  Fl_Output   *m_fld_hghval;
  Fl_Output   *m_fld_lowval_util;
  Fl_Output   *m_fld_hghval_util;

  Fl_Output   *m_fld_lowmin_util;
  Fl_Output   *m_fld_hghmin_util;
};
#endif




