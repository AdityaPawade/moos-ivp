/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvoidCollision.h                                 */
/*    DATE: Nov 18th 2006                                        */
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
 
#ifndef BHV_AVOID_COLLISION_HEADER
#define BHV_AVOID_COLLISION_HEADER

#include "IvPContactBehavior.h"

class IvPDomain;
class BHV_AvoidCollision : public IvPContactBehavior {
public:
  BHV_AvoidCollision(IvPDomain);
  ~BHV_AvoidCollision() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();
  void         onRunToIdleState();
  void         onCompleteState();

protected:
  double getRelevance();
  double getPriority();
  void   postInfo(double, double);
  void   postRange(bool ok=true);
  
private: // Configuration Parameters

  std::string m_pwt_grade;

  double m_completed_dist;
  double m_pwt_outer_dist;
  double m_pwt_inner_dist;
  double m_min_util_cpa_dist;
  double m_max_util_cpa_dist;
  double m_roc_max_heighten; // Rate of Closure w/ max relevance heightening
  double m_roc_max_dampen;   // Rate of Closure w/ max relevance dampening

  double m_collision_depth;

private:  // State Variables
  double m_curr_closing_spd;
};
#endif

