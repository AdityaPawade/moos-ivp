/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYSegList.h                                          */
/*    DATE: Apr 29, 2005                                         */
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
 
#ifndef XY_SEGLIST_HEADER
#define XY_SEGLIST_HEADER

#include <vector>
#include <string>

class XYSegList {
public:
  XYSegList() {};
  virtual ~XYSegList() {};

  // XYSegList create and edit functions
  void   add_vertex(double, double);       // One way to build
  bool   initialize(std::string);          // Another way to build
  bool   init_zigzag(std::string);
  void   alter_vertex(double, double);
  void   delete_vertex(double, double);
  void   insert_vertex(double, double);
  void   clear();

public:
  void   shift_horz(double val);
  void   shift_vert(double val);
  void   grow_by_pct(double pct);
  void   grow_by_amt(double amt);
  void   rotate(double degrees);
  void   apply_snap(double snapval);
  void   reverse();
  void   new_center(double x, double y);
  void   print() const;

public:
  void   set_label(std::string str)  {label=str;};
  int    size() const                {return(vertex_x.size());};
  double get_vx(int) const;
  double get_vy(int) const;
  double get_center_x() const;
  double get_center_y() const;
  double get_avg_x() const;
  double get_avg_y() const;

  std::string get_spec() const;
  std::string get_label() const {return(label);};

protected:
  int    closest_vertex(double, double) const; 
  int    closest_segment(double, double) const;
  void   grow_pt_by_pct(double, double, double, double&, double&);
  void   grow_pt_by_amt(double, double, double, double&, double&);
  void   rotate_pt(double, double, double, double&, double&);

protected:
  std::vector<double> vertex_x;
  std::vector<double> vertex_y;
  std::string         label;
};

#endif









