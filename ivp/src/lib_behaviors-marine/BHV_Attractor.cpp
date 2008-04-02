/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Attractor.cpp                                     */
/*    DATE: May 10th 2005                                        */
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
#include <math.h>
#include "AOF_AttractorCPA.h"
#include "BHV_Attractor.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

BHV_Attractor::BHV_Attractor(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_cutrange");
  this->setParam("build_info", "uniform_box=course:2,speed:3");
  this->setParam("build_info", "uniform_grid=course:8,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");

  // If equal: 100% priority above, 0% priority below
  // If not equal: Linear scale between 0-100 in between

  m_min_priority_range = 0;
  m_max_priority_range = 0;

  m_min_util_cpa_dist = 100;
  m_max_util_cpa_dist = 0; 

  m_extrapolate  = true;
  m_patience     = 0;
  m_giveup_range = 0;   // meters - zero means never give up
  m_time_on_leg  = 15;  // seconds

  strength = 1.0;

  m_decay_start = 5;
  m_decay_end   = 10;

  m_extrapolator.setDecay(m_decay_start, m_decay_end);

  m_cpa_speed = 1.2;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Attractor::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    m_them_name = toupper(g_val);
    addInfoVars(m_them_name+"_NAV_X");
    addInfoVars(m_them_name+"_NAV_Y");
    addInfoVars(m_them_name+"_NAV_SPEED");
    addInfoVars(m_them_name+"_NAV_HEADING");
    addInfoVars(m_them_name+"_NAV_UTC");
    return(true);
  }  
  else if(g_param == "dist_priority_interval") {
    g_val = stripBlankEnds(g_val);
    vector<string> svector = parseString(g_val, ',');
    if(svector.size() != 2)
      return(false);
    if(!isNumber(svector[0]) || !isNumber(svector[1]))
      return(false);
    
    double dval1 = atof(svector[0].c_str());
    double dval2 = atof(svector[1].c_str());
    if((dval1 < 0) || (dval2 < 0) || (dval1 > dval2))
      return(false);
    m_min_priority_range = dval1;
    m_max_priority_range = dval2;
    return(true);
  }  
  else if(g_param == "cpa_utility_interval") {
    g_val = stripBlankEnds(g_val);
    vector<string> svector = parseString(g_val, ',');
    if(svector.size() != 2)
      return(false);
    if(!isNumber(svector[0]) || !isNumber(svector[1]))
      return(false);

    double dval1 = atof(svector[0].c_str());
    double dval2 = atof(svector[1].c_str());
    if((dval1 < 0) || (dval2 < 0) || (dval1 >= dval2))
      return(false);
    m_max_util_cpa_dist = dval1;
    m_min_util_cpa_dist = dval2;
    return(true);
  }  
  else if(g_param == "time_on_leg") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_time_on_leg = dval;
    return(true);
  }  
  else if(g_param == "giveup_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_giveup_range = dval;
    return(true);
  }  

  else if(g_param == "patience") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (dval > 100) || (!isNumber(g_val)))
      return(false);
    m_patience = dval;
    return(true);
  }  

  else if(g_param == "strength") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (dval > 100) || (!isNumber(g_val)))
      return(false);
    strength = dval;
    return(true);
  }  

  else if(g_param == "cpa_speed") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (dval > 100) || (!isNumber(g_val)))
      return(false);
    m_cpa_speed = dval;
    return(true);
  }  

  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Attractor::onIdleState()
{
  postMessage("ATTRACTOR_ACTIVE", 0);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Attractor::onRunState() 
{
  postMessage("ATTRACTOR_ACTIVE", 1);

  if(m_them_name == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  // Set m_osx, m_osy, m_osh, m_osv, m_cnx, m_cny, m_cnh, m_cnv
  if(!updateInfoIn())
    return(0);

  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(m_osx, m_osy, m_cnx, m_cny);

  if(relevance <= 0) {
    postMessage("PURSUIT", 0);
    return(0);
  }

  postMessage("PURSUIT", 1);

  double dist = hypot((m_osx - m_cnx), (m_osy - m_cny));

  IvPFunction *ipf = 0;
  if (dist > m_min_priority_range)
    {  AOF_AttractorCPA aof(m_domain);
      aof.setParam("cnlat", m_cny);
      aof.setParam("cnlon", m_cnx);
      aof.setParam("cncrs", m_cnh);
      aof.setParam("cnspd", 0.5*m_cpa_speed);
      //      aof.setParam("cnspd", m_cnv);
      aof.setParam("oslat", m_osy);
      aof.setParam("oslon", m_osx);
      aof.setParam("tol", m_time_on_leg);
      aof.setParam("patience", m_patience);
      aof.setParam("min_util_cpa_dist", m_min_util_cpa_dist);
      aof.setParam("max_util_cpa_dist", m_max_util_cpa_dist);
      bool ok1 = aof.initialize();
      
      if(!ok1) {
	postWMessage("Error in initializing AOF_AttractorCPA.");
	return(0);
      }
      
      OF_Reflector reflector(&aof, 1);
      reflector.create(m_build_info);
      ipf = reflector.extractOF();
    }
  else
    {
      ZAIC_PEAK hdg_zaic(m_domain, "course");
      hdg_zaic.setSummit(m_cnh);
      hdg_zaic.setValueWrap(true);
      hdg_zaic.setPeakWidth(30);
      hdg_zaic.setBaseWidth(150);
      hdg_zaic.setSummitDelta(50.0);
      hdg_zaic.setMinMaxUtil(0,100);
      IvPFunction *hdg_ipf = hdg_zaic.extractOF();
      
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      
      spd_zaic.setSummit(m_cnv);
      spd_zaic.setPeakWidth(0.1);
      spd_zaic.setBaseWidth(2.0);
      spd_zaic.setSummitDelta(50.0); 
      
      IvPFunction *spd_ipf = spd_zaic.extractOF();
      
      OF_Coupler coupler;
      ipf = coupler.couple(hdg_ipf, spd_ipf);
    }
  

  ipf->getPDMap()->normalize(0.0, 100.0);

  ipf->setPWT(relevance * m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an EMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_Attractor::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4, ok5, ok6, ok7, ok8, ok9;
 
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);

  m_cnx = getBufferDoubleVal(m_them_name+"_NAV_X", ok5);
  m_cny = getBufferDoubleVal(m_them_name+"_NAV_Y", ok6);
  m_cnh = getBufferDoubleVal(m_them_name+"_NAV_HEADING", ok7);
  m_cnv = getBufferDoubleVal(m_them_name+"_NAV_SPEED", ok8);
  m_cnutc = getBufferDoubleVal(m_them_name+"_NAV_UTC", ok9);

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || 
     !ok6 || !ok7 || !ok8 || !ok9)
    return(false);
  
  double curr_time = getBufferCurrTime();
  // double mark_time = getBufferTimeVal(m_them_name+"_NAV_X");

  postMessage("TRAIL_CONTACT_TIME", m_cnutc);
  postMessage("TRAIL_CURRENT_TIME", curr_time);
  postMessage("TRAIL_DELTA_TIME", curr_time-m_cnutc);

  if(!m_extrapolate)
    return(true);

  // if(mark_time == 0)
    m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, m_cnutc);

  // Even if mark_time is zero and thus "fresh", still derive the 
  // the contact position from the extrapolator since the UTC time
  // of the position in this report may still be substantially 
  // behind the current own-platform UTC time.

  double new_cnx, new_cny;
  bool ok = m_extrapolator.getPosition(new_cnx, new_cny, curr_time);

  if(ok) {
    m_cnx = new_cnx;
    m_cny = new_cny;
    return(true);
  }
  else {
    postWMessage("Incomplete Linear Extrapolation");
    return(false);
  }
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_Attractor::getRelevance(double osX, double osY,
				  double cnX, double cnY)
{
  double range_max = m_max_priority_range;
  double range_min = m_min_priority_range;

  // Should be caught when setting the parameters, but check again
  if((range_max < 0) || (range_min < 0) || (range_min > range_max)) {
    postWMessage("Priority Range Error");
    return(0);
  }
  
  double dist = hypot((osX - cnX), (osY - cnY));

  if((m_giveup_range > 0) && (dist > m_giveup_range))
    return(0);

  
  double total_range = range_max - range_min;
  //double exp_wgt= 1.0-exp(-10*(dist-range_min)/range_max);
  double exp_wgt = 1;
  double pct = 1;
  // if total_range==0 one of the two cases will result in a return

  if (dist >= range_max)
    pct = 1+strength;
  else
    if (dist < range_min)
      pct = dist/range_min;
    else 
      if (total_range > 0)
	pct = (1 + strength * (dist-range_min) / total_range);
      else
	pct = 1;
  return(pct);
}









