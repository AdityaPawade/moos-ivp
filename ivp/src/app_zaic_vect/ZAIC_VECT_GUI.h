/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_VECT_GUI.h                                      */
/*    DATE: May 16th, 2016                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_VECT_GUI_HEADER
#define ZAIC_VECT_GUI_HEADER

#include "ZAIC_Vector.h"
#include "ZAIC_GUI.h"

class ZAIC_VECT_GUI : public ZAIC_GUI {
 public:
  ZAIC_VECT_GUI(int w, int h, const char *label=0);
  ~ZAIC_VECT_GUI() {};

  void setZAIC(ZAIC_Vector *zaic);
  void setVerbose(bool);

 protected:
  void updateOutput(); // overloading virtual function of ZAIC_GUI
  void augmentMenu();

 private:
  Fl_Output  *m_min_util;
  Fl_Output  *m_max_util;
};
#endif
