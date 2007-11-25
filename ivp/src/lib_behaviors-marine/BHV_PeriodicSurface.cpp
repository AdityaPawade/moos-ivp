/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_PeriodicSurface.cpp                              */
/*    DATE: Feb 21st 2007                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include "math.h"
#include "BHV_PeriodicSurface.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_PeriodicSurface::BHV_PeriodicSurface(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_periodic_surface");

  domain = subDomain(domain, "speed,depth");

  // Behavior Parameter Default Values:
  m_period              = 300; // Seconds
  m_mark_variable       = "GPS_UPDATE_RECEIVED";
  m_pending_status_var  = "PENDING_SURFACE";
  m_atsurface_status_var = "TIME_AT_SURFACE";
  m_max_time_at_surface = 300;
  m_ascent_speed        = -1; 
  m_zero_speed_depth    = 0;
  m_ascent_grade        = "linear";

  // Behavior State Variable Initial Values:
  m_first_iteration       = true;
  m_first_marking_string  = true;
  m_first_marking_double  = true;

  m_depth_at_ascent_begin = 0;
  m_speed_at_ascent_begin = 0;
  m_mark_time             = 0;

  m_curr_mark_string_val  = "";
  m_curr_mark_double_val  = 0;

  m_surface_mark_time     = 0;
  m_at_surface            = false;

  // Declare information needed by this behavior
  info_vars.push_back(m_mark_variable);
  info_vars.push_back("NAV_DEPTH");
  info_vars.push_back("NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_PeriodicSurface::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  g_val = stripBlankEnds(g_val);

  if(g_param == "period") {
    double dval = atof(g_val.c_str());
    if((dval <= 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      m_period = dval;
    return(true);
  } 
  else if(g_param == "mark_variable") {
    if(g_val == "")
      return(false);
    if(!param_lock) {
      m_mark_variable = g_val;
      info_vars.push_back(m_mark_variable);
    }
    return(true);
  }
  else if(g_param == "pending_status_var") {
    if(g_val == "")
      return(false);
    if(!param_lock)
      m_pending_status_var = g_val;
    return(true);
  }
  else if(g_param == "atsurface_status_var") {
    if(g_val == "")
      return(false);
    if(!param_lock)
      m_atsurface_status_var = g_val;
    return(true);
  }
  else if((g_param == "speed_to_surface") || 
	  (g_param == "ascent_speed")) {
    double dval;
    if(isNumber(g_val)) {
      dval = atof(g_val.c_str());
      if(dval < 0)
	return(false);
    }
    else {
      if(g_val == "current")
	dval = -1;
      else
	return(false);
    }
    if(!param_lock)
      m_ascent_speed = dval;
    return(true);
  }
  else if(g_param == "zero_speed_depth") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      m_zero_speed_depth = dval;
    return(true);
  }
  else if(g_param == "max_time_at_surface") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      m_max_time_at_surface = dval;
    return(true);
  }
  else if(g_param == "ascent_grade") {
    g_val = tolower(g_val);
    if((g_val!="linear") && (g_val!="quadratic") && 
       (g_val!="quasi") && (g_val != "fullspeed"))
      return(false);
    if(!param_lock)
      m_ascent_grade = g_val;
    return(true);
  }
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_PeriodicSurface::produceOF() 
{
  // Set m_curr_depth, m_curr_speed, m_curr_time
  if(!updateInfoIn()) {
    postMessage(m_pending_status_var, "ERROR");
    return(0);
  }

  checkForMarking();
  
  double time_since_mark       = m_curr_time - m_mark_time;
  double period_wait_remaining = m_period - time_since_mark;
  if(period_wait_remaining < 0)
    period_wait_remaining = 0;
  
  if(m_pending_status_var != "")
    postMessage(m_pending_status_var, period_wait_remaining);

  if(period_wait_remaining > 0) {
    m_state = "waiting";
    postMessage(m_atsurface_status_var, 0);
    return(0);
  }

  if(m_state == "waiting") {
    m_state = "ascending";
    m_depth_at_ascent_begin = m_curr_depth;
    if(m_ascent_speed != -1)
      m_speed_at_ascent_begin = m_ascent_speed;
    else
      m_speed_at_ascent_begin = m_curr_speed;
  }
  
  if(m_state == "ascending") {
    if(m_curr_depth <= m_zero_speed_depth) {
      m_state = "at_surface";
      m_surface_mark_time = m_curr_time;
    }
  }


  if(m_state == "at_surface") {
    double time_at_surface = m_curr_time - m_surface_mark_time;
    postMessage(m_atsurface_status_var, time_at_surface);
    if(time_at_surface >= m_max_time_at_surface) {
      m_mark_time = m_curr_time;
      m_state = "waiting";
      return(0);
    }
  }
  else
    postMessage(m_atsurface_status_var, 0);
  
  double desired_speed;
  if(m_state == "ascending")
    desired_speed = setDesiredSpeed();
  else
    desired_speed = 0;



  ZAIC_PEAK zaic_speed(domain, "speed");
  zaic_speed.setSummit(desired_speed);
  if(desired_speed == 0)
    zaic_speed.setBaseWidth(0);
  else
    zaic_speed.setBaseWidth(0.5);
  zaic_speed.setPeakWidth(0);
  zaic_speed.setMinMaxUtil(0, 100);

  IvPFunction *ipf_speed = zaic_speed.extractOF();


  ZAIC_PEAK zaic_depth(domain, "depth");
  zaic_depth.setSummit(0);
  zaic_depth.setBaseWidth(0);
  zaic_depth.setPeakWidth(0);
  zaic_depth.setMinMaxUtil(0, 100);

  IvPFunction *ipf_depth = zaic_depth.extractOF();

  OF_Coupler coupler;
  IvPFunction *new_ipf = coupler.couple(ipf_speed, ipf_depth);

  new_ipf->setPWT(priority_wt);

  return(new_ipf);
}

//-----------------------------------------------------------
// Procedure: updatInfoIn
//

bool BHV_PeriodicSurface::updateInfoIn()
{  
  bool ok1, ok2;
  m_curr_depth = info_buffer->dQuery("NAV_DEPTH", ok1);
  m_curr_speed = info_buffer->dQuery("NAV_SPEED", ok2);
  m_curr_time  = info_buffer->getCurrTime();
  
  if(!ok1 || !ok2) {
    postEMessage("No ownship NAV_DEPTH or NAV_SPEED in info_buffer");
    return(false);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: checkForMarking
//
//   Check if a "marking" has occurred. The status_variable is 
//   examined and if different than the previous value, a "marking"
//   is declared to have occurred. 

void BHV_PeriodicSurface::checkForMarking()
{
  bool   mark_noted = false;

  bool   ok1, ok2;
  double double_mark_val = info_buffer->dQuery(m_mark_variable, ok1);
  string string_mark_val = info_buffer->sQuery(m_mark_variable, ok2);

  if(m_first_iteration) {
    m_first_iteration = false;
    mark_noted = true;
  }  

  if(ok1) {
    if(m_first_marking_double || (double_mark_val != m_curr_mark_double_val)) {
      mark_noted = true;
      m_curr_mark_double_val = double_mark_val;
      m_first_marking_double = false;
    }
  }
  
  if(ok2) {
    if(m_first_marking_string || (string_mark_val != m_curr_mark_string_val)) {
      mark_noted = true;
      m_curr_mark_string_val = string_mark_val;
      m_first_marking_string = false;
    }
  }

  if(mark_noted)
    m_mark_time = m_curr_time;
}


//-----------------------------------------------------------
// Procedure: setDesiredSpeed
//
//  Info used in this calculation:
//    (1) m_curr_speed - (state variable)
//    (2) m_curr_depth -  (state variable)
//    (3) m_ascent_grade - (configuration variable)
//    (4) m_ascent_speed - (configuration variable)
//    (5) m_zero_speed_depth - (configuration variable)
//    (6) m_speed_at_ascent_begin - (state variable)
//    (7) m_depth_at_ascent_begin - (state variable)


double BHV_PeriodicSurface::setDesiredSpeed()
{
  double desired_speed;

  // Handle the possibility that the ascent begins at depth
  // more shallow than the m_zero_speed_depth.
  if(m_depth_at_ascent_begin <= m_zero_speed_depth)
    return(0);


  // Given that the m_depth_at_ascent_begin is deeper than the
  // m_zero_speed_depth, then THREE cases are possible:


  // Case1: current depth shallower than the m_zero_speed_depth
  if((m_curr_depth <= m_zero_speed_depth) ||
     (m_zero_speed_depth >= m_depth_at_ascent_begin))
    desired_speed = 0;
  // Case2: current depth deeper than m_depth_at_ascent_begin
  else if(m_curr_depth > m_depth_at_ascent_begin)
    desired_speed = m_speed_at_ascent_begin;
  // Case3: current depth somewhere between the zero_speed depth
  //        and the m_depth_at_ascent_begin
  else {
    double range = m_depth_at_ascent_begin - m_zero_speed_depth;
    double sofar = m_curr_depth - m_zero_speed_depth;
    double pct   = sofar / range;
    if(m_ascent_grade == "quadratic")
      desired_speed = m_speed_at_ascent_begin * pct * pct;
    else if(m_ascent_grade == "quasi")
      desired_speed = m_speed_at_ascent_begin * pow(pct, 1.5);
    else if(m_ascent_grade == "fullspeed")
      desired_speed = m_speed_at_ascent_begin;
    else // linear
      desired_speed = m_speed_at_ascent_begin * pct;
  }

  return(desired_speed);
}




