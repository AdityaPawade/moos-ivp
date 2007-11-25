/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_StandOn17.cpp                                    */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h>
#include "AngleUtils.h"
#include "BHV_StandOn17.h"
#include "AOF_R16.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_StandOn17::BHV_StandOn17(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_giveway17");
  this->setParam("unifbox", "course=1, speed=1, tol=1");
  this->setParam("gridbox", "course=8, speed=5, tol=5");

  domain = subDomain(domain, "course,speed,tol");

  range = -1;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
  info_vars.push_back("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_StandOn17::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    if(!param_lock) {
      them_name = toupper(g_val);
      info_vars.push_back(them_name+"_NAV_X");
      info_vars.push_back(them_name+"_NAV_Y");
      info_vars.push_back(them_name+"_NAV_SPEED");
      info_vars.push_back(them_name+"_NAV_HEADING");
    }
    return(true);
  }  
  else if(g_param == "range") {
    range = atof(g_val.c_str());
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_StandOn17::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  if(them_name == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok1, ok2;

  double cnCRS = info_buffer->dQuery(them_name+"_NAV_HEADING", ok1);
  double cnSPD = info_buffer->dQuery(them_name+"_NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postWMessage("contact course/speed info not found.");
    return(0);
  }

  double osCRS = info_buffer->dQuery("NAV_HEADING", ok1);
  double osSPD = info_buffer->dQuery("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(0);
  }

  if(cnCRS < 0) cnCRS += 360.0;

  double cnLAT = info_buffer->dQuery(them_name+"_NAV_Y", ok1);
  double cnLON = info_buffer->dQuery(them_name+"_NAV_X", ok2);
  if(!ok1 || !ok2) {
    postWMessage("contact x/y info not found.");
    return(0);
  }

  double osLAT = info_buffer->dQuery("NAV_Y", ok1);
  double osLON = info_buffer->dQuery("NAV_X", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(0);
  }

  double relevance = getRelevance(osLON, osLAT, osCRS, osSPD, 
				  cnLON, cnLAT, cnCRS, cnSPD);

  cout << "BHV_StandOn17::produceOF(): relevance: " << relevance << endl;
  if(relevance == 0)
    return(0);

  AOF_R16 aof(domain);
  aof.setParam("os_lat", osLAT);
  aof.setParam("os_lon", osLON);
  aof.setParam("cn_lat", cnLAT);
  aof.setParam("cn_lon", cnLON);
  aof.setParam("cn_crs", cnCRS);
  aof.setParam("cn_spd", cnSPD);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);

  reflector.createUniform(unif_box, grid_box);

  IvPFunction *of = reflector.extractOF();

  of->setPWT(relevance * priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_StandOn17::getRelevance(double oslon, double oslat,
				   double oscrs, double osspd,
				   double cnlon, double cnlat, 
				   double cncrs, double cnspd)
{
  if((oslon==cnlon)&&(oslat==cnlat))
    return(0);

  if(range > -1) { // -1 means range wasn't specified
    double dist = hypot((oslon-cnlon), (oslat-cnlat));
    if(dist > range)
      return(0);
  }

  double ang_to_cn = relAng(oslon, oslat, cnlon, cnlat);
  double ang_to_os = ang_to_cn + 180.0;
  if(ang_to_os >= 360.0)
    ang_to_os -= 360.0;

  cout << "ang_to_cn: " << ang_to_cn << endl;
  cout << "oscrs:     " << oscrs << endl;
  cout << "ang_to_os: " << ang_to_os << endl;
  cout << "cncrs:     " << cncrs << endl;


  // Determine the "bearing delta" - the difference between the course
  // of a vehicle and the bearing of that vehicle to the other vehicle.
  //
  //                                x     Bearing to "x" is 35 degrees
  //                              /
  //                             /
  //                            /         "Bearing delta" is 55 degrees
  //                           /
  //                          /
  //                         O------->    Current course is 90 degrees

#if 0
  double os_bearing_delta = oscrs - ang_to_cn;
  if(os_bearing_delta < 0)
    os_bearing_delta += 360.0;
  if(os_bearing_delta > 180)
    os_bearing_delta = 360.0 - os_bearing_delta;
  if(os_bearing_delta > 15.0)
    return(0.0);
#endif

#if 1
  double cn_bearing_delta = cncrs - ang_to_os;
  if(cn_bearing_delta < 0)
    cn_bearing_delta += 360.0;
  if(cn_bearing_delta > 180)
    cn_bearing_delta = 360.0 - cn_bearing_delta;
  if(cn_bearing_delta > 15.0)
    return(0.0);
#endif

  return(1.0);
}






