/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GUI.h                                                */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SIMPLE_A_GUI_HEADER
#define SIMPLE_A_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "MY_Output.h"

#include "Viewer.h"

class GUI : Fl_Window {
public:
  GUI(int w, int h, const char *l=0);

  static Fl_Menu_Item menu_[];

  void updateXY();
  int  handle(int);
  void setAOF(AOF* aof)           
    {viewer->setAOF(aof); viewer->modColorMap("default");};

public:
  Fl_Menu_Bar *mbar;
  Viewer      *viewer;

  MY_Output   *p_worst_err;
  MY_Output   *p_avg_err;
  MY_Output   *p_square_err;
  MY_Output   *p_samp_count;
  MY_Output   *p_samp_high;
  MY_Output   *p_samp_low;

  MY_Output   *p_piece_count;
  MY_Output   *p_unif_aug_size;
  MY_Output   *p_create_time;

private:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);

  inline void cb_Script_i();
  static void cb_Script(Fl_Widget*);

  inline void cb_Reset_i(int);
  static void cb_Reset(Fl_Widget*, int);

  inline void cb_RotateX_i(int);
  static void cb_RotateX(Fl_Widget*, int);

  inline void cb_RotateZ_i(int);
  static void cb_RotateZ(Fl_Widget*, int);

  inline void cb_ModScale_i(int);
  static void cb_ModScale(Fl_Widget*, int);

  inline void cb_ModBaseAOF_i(int);
  static void cb_ModBaseAOF(Fl_Widget*, int);

  inline void cb_ModBaseIPF_i(int);
  static void cb_ModBaseIPF(Fl_Widget*, int);

  inline void cb_ToggleAOF_i();
  static void cb_ToggleAOF(Fl_Widget*);

  inline void cb_ToggleIPF_i();
  static void cb_ToggleIPF(Fl_Widget*);

  inline void cb_ToggleUniformAug_i();
  static void cb_ToggleUniformAug(Fl_Widget*);

  inline void cb_TogglePriorityAug_i();
  static void cb_TogglePriorityAug(Fl_Widget*);

  inline void cb_PriorityAugAmt_i(int);
  static void cb_PriorityAugAmt(Fl_Widget*, int);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_ToggleStrict_i();
  static void cb_ToggleStrict(Fl_Widget*);

  inline void cb_ModPatchAOF_i(int);
  static void cb_ModPatchAOF(Fl_Widget*, int);

  inline void cb_ModUniformAug_i(int);
  static void cb_ModUniformAug(Fl_Widget*, int);

  inline void cb_Sample_i(int);
  static void cb_Sample(Fl_Widget*, int);

  inline void cb_Rebuild_i(int);
  static void cb_Rebuild(Fl_Widget*, int);

  inline void cb_MakeUniform_i(int);
  static void cb_MakeUniform(Fl_Widget*, int);

  inline void cb_ColorMap_i(int);
  static void cb_ColorMap(Fl_Widget*, int);

  inline void cb_ColorBack_i(int);
  static void cb_ColorBack(Fl_Widget*, int);

  static void cb_Quit();
};
#endif
