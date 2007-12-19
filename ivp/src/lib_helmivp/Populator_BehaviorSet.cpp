/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_BehaviorSet.cpp                            */
/*    DATE:                                                      */
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
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Populator_BehaviorSet.h"

// CORE Behaviors
#include "BHV_Waypoint.h"
#include "BHV_Loiter.h"
#include "BHV_OpRegion.h"
#include "BHV_ConstantDepth.h"
#include "BHV_ConstantHeading.h"
#include "BHV_ConstantSpeed.h"
#include "BHV_PeriodicSpeed.h"
#include "BHV_PeriodicSurface.h"
#include "BHV_Trail.h"
#include "BHV_Shadow.h"
#include "BHV_Timer.h"
#include "BHV_StationKeep.h"
#include "BHV_RStationKeep.h"
#include "BHV_CutRange.h"
#include "BHV_AvoidCollision.h"
#include "BHV_GoToDepth.h"
#include "BHV_MemoryTurnLimit.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

#if 0
#include "WayPointVW.h"
#endif

#if 0
// Sandbox behaviors
//#include "BHV_WaypointOx.h"
#include "BHV_LoiterMigrate.h"
#include "BHV_AvoidCollision.h"
#include "BHV_AvoidObstacles.h"
#include "BHV_WaypointRC.h"
#include "BHV_WaypointSimple.h"
#include "BHV_CutRangeFCPA.h"
#include "BHV_SearchGrid.h"
#include "BHV_SteadyHeading.h"
#endif

#if 1
// COLREGS Behaviors
#include "BHV_HeadOn14.h"
#include "BHV_GiveWay16.h"
#include "BHV_StandOn17.h"
#endif

#if 0
// Don Eickstedt Behaviors
#include "BHV_CoOrbit.h"
#include "BHV_Adaptive_YoYo.h"
#include "BHV_CloseRange.h"
#include "BHV_ArrayAngle.h"
#include "BHV_ArrayTurn.h"
#include "BHV_2VAngle.h"
#endif

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

Populator_BehaviorSet::Populator_BehaviorSet(IvPDomain g_domain,
					     InfoBuffer *g_buffer)
{
  // cout << "Populator_BehaviorSet::Constructor()" << endl;
  // g_domain.print();
  // cout << endl << endl;

  domain             = g_domain;
  info_buffer        = g_buffer;
  define_mode        = 0;
  open_behavior_mode = false;
  ok                 = false;
}

//-------------------------------------------------------------
// Procedure: populate

BehaviorSet *Populator_BehaviorSet::populate(set<string> bhv_files)
{
  int i;
  set<string>::const_iterator p;
  for(p=bhv_files.begin(); p!=bhv_files.end(); p++) {

    string filename = *p;

    FILE *f = fopen(filename.c_str(), "r");
    
    if(!f) 
      cout << "Could not find File: " << filename << endl;
    else {
      cout << "Successfully found file: " << filename << endl;
      fclose(f);
      
      vector<string> file_vector = fileBufferSlash(filename);
      int lineCount = file_vector.size();
    
      for(i=0; i<lineCount; i++) {
	string line = stripBlankEnds(file_vector[i]);
	
	if((line.length()!=0) && ((line)[0]!='#')) {
	  int res = handleLine(line);
	  if(!res) {
	    cout << " Problem with line " << i+1;
	    cout << " in the BehaviorSet file: " << filename << endl;
	    cout << line << endl;
	    return(0);
	  }
	}
      }
    }
  }

  if(behaviors.size() == 0) 
    return(0);
  else {
    BehaviorSet *bset = new BehaviorSet;
    for(i=0; i<behaviors.size(); i++) {
      behaviors[i]->setInfoBuffer(info_buffer);
      bset->addBehavior(behaviors[i]);
    }
    for(i=0; i<initial_vars.size(); i++)
      bset->addInitialVar(initial_vars[i]);
    for(i=0; i<default_vars.size(); i++)
      bset->addDefaultVar(default_vars[i]);
    return(bset);
  }
}

//-------------------------------------------------------------
// Procedure: populate
//      Note: A convenience function when the behaviors are 
//            given by only one file.

BehaviorSet *Populator_BehaviorSet::populate(string filestr)
{
  set<string> bhv_files;
  bhv_files.insert(filestr);
  return(populate(bhv_files));
}

//----------------------------------------------------------
// Procedure: handleLine
//   Returns: 1 if all OK
//            0 otherwise
//
bool Populator_BehaviorSet::handleLine(string line)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  line = stripComment(line, "#");
  line = stripBlankEnds(line);
  if(line.size() == 0)  // Either blank or comment line
    return(true);  
  
  if(!strncasecmp("initialize", line.c_str(), 10))
    line = findReplace(line, "initialize", "initial_var = ");

  vector<string> svector = chompString(line, '=');

  string left  = stripBlankEnds(svector[0]);
  string right = stripBlankEnds(svector[1]);

  if(right == "") {
    if((left != "{") && (left != "}")) {
      cout << "PROBLEM #1" << endl;
      return(false);
    }

    if(left == "{") {
      if(define_mode == 0) {
	if(open_behavior_mode)
	  define_mode = 1;
	return(true);
      }
      else { 
	cout << "PROBLEM #2" << endl;
	return(false);
      }
    }
      
    if(left == "}") {
      open_behavior_mode = false;
      define_mode = 0;
      return(true);
    }
  }
  
  // Handle initialization lines
  string str = tolower(left);
  if((str == "initial_var") && (define_mode == 0)) {
    right = findReplace(right, ',', '=');
    vector<string> dvector = parseString(right, '=');
    if(dvector.size() != 2) {
      cout << "PROBLEM #2" << endl;
      return(false);
    }
    VarDataPair msg(dvector[0], dvector[1], "auto");
    initial_vars.push_back(msg);
    return(true);
  }

  // Handle default lines
  if((str == "default") && (define_mode == 0)) {
    vector<string> dvector = parseString(right, '=');
    if(dvector.size() != 2)
      return(false);
    VarDataPair msg(dvector[0], dvector[1], "auto");
    default_vars.push_back(msg);
    return(true);
  }
  
  if(define_mode == 0) {
    if(left != "Behavior") {
      if(open_behavior_mode)
	return(false);
      else
	return(true);
    }
    else
      open_behavior_mode = true;

    IvPBehavior *bhv = initializeBehavior(right); 

    if(bhv)
      behaviors.push_back(bhv);
    return(bhv!=0);
  }
  
  if(define_mode == 1) {
    IvPBehavior *bhv = behaviors[behaviors.size()-1];
    bool result = bhv->setParam(left.c_str(), right.c_str());
    return(result);
  }

  return(false);
}


//----------------------------------------------------------
// Procedure: initializeBehavior

IvPBehavior* Populator_BehaviorSet::initializeBehavior(string bhv_name)
{
  IvPBehavior *bhv = 0;      

#if 1 // Core Behaviors
  if(bhv_name == "BHV_OpRegion")
    bhv = new BHV_OpRegion(domain);
  else if(bhv_name == "BHV_Waypoint")   
    bhv = new BHV_Waypoint(domain);
  else if(bhv_name == "BHV_ConstantSpeed")     
    bhv = new BHV_ConstantSpeed(domain);
  else if(bhv_name == "BHV_Trail")      
    bhv = new BHV_Trail(domain);
  else if(bhv_name == "BHV_ConstantDepth")      
    bhv = new BHV_ConstantDepth(domain);
  else if(bhv_name == "BHV_ConstantHeading")      
    bhv = new BHV_ConstantHeading(domain);
  else if(bhv_name == "BHV_Loiter")     
    bhv = new BHV_Loiter(domain);
  else if(bhv_name == "BHV_StationKeep")     
    bhv = new BHV_StationKeep(domain);
  else if(bhv_name == "BHV_RStationKeep")     
    bhv = new BHV_RStationKeep(domain);
  else if(bhv_name == "BHV_Timer")     
    bhv = new BHV_Timer(domain);
  else if(bhv_name == "BHV_Shadow")     
    bhv = new BHV_Shadow(domain);
  else if(bhv_name == "BHV_CutRange")   
    bhv = new BHV_CutRange(domain);
  else if(bhv_name == "BHV_AvoidCollision") 
    bhv = new BHV_AvoidCollision(domain);
  else if(bhv_name == "BHV_PeriodicSpeed") 
    bhv = new BHV_PeriodicSpeed(domain);
  else if(bhv_name == "BHV_PeriodicSurface") 
    bhv = new BHV_PeriodicSurface(domain);
  else if(bhv_name == "BHV_GoToDepth")      
    bhv = new BHV_GoToDepth(domain);
  else if(bhv_name == "BHV_MemoryTurnLimit")      
    bhv = new BHV_MemoryTurnLimit(domain);
#endif

#if 0 
  // Oxford Behaviors
  else if(bhv_name == "WayPointVW")      
    bhv = new CWayPointVW(domain);
#endif
  
#if 0
  // Sandbox behaviors
  else if(bhv_name == "BHV_AvoidObstacles")
    bhv = new BHV_AvoidObstacles(domain);
  //else if(bhv_name == "BHV_WaypointOx") 
  //  bhv = new BHV_WaypointOx(domain);
  else if(bhv_name == "BHV_AvoidCollision") 
    bhv = new BHV_AvoidCollision(domain);
  else if(bhv_name == "BHV_LoiterMigrate") 
    bhv = new BHV_LoiterMigrate(domain);
  else if(bhv_name == "BHV_WaypointRC")      
    bhv = new BHV_WaypointRC(domain);
  else if(bhv_name == "BHV_WaypointSimple")      
    bhv = new BHV_WaypointSimple(domain);
  else if(bhv_name == "BHV_CutRangeFCPA")   
    bhv = new BHV_CutRangeFCPA(domain);
  else if(bhv_name == "BHV_SearchGrid")   
    bhv = new BHV_SearchGrid(domain);
  else if(bhv_name == "BHV_SteadyHeading")   
    bhv = new BHV_SteadyHeading(domain);
#endif
  
#if 1
  // COLREGS Behaviors --------------------------------
  else if(bhv_name == "BHV_HeadOn14")        
    bhv = new BHV_HeadOn14(domain);
  else if(bhv_name == "BHV_GiveWay16")        
    bhv = new BHV_GiveWay16(domain);
  else if(bhv_name == "BHV_StandOn17")        
    bhv = new BHV_StandOn17(domain);
#endif
  
#if 0
  // Don Eickstedt Behaviors --------------------------
  else if(bhv_name == "BHV_CoOrbit")    
    bhv = new BHV_CoOrbit(domain);
  else if(bhv_name == "BHV_Adaptive_YoYo")    
    bhv = new BHV_Adaptive_YoYo(domain);
  else if (bhv_name == "BHV_CloseRange")
    bhv = new BHV_CloseRange(domain);
  else if (bhv_name == "BHV_ArrayTurn")
    bhv = new BHV_ArrayTurn(domain);
  else if (bhv_name == "BHV_ArrayAngle")
    bhv = new BHV_ArrayAngle(domain);
  else if (bhv_name == "BHV_2VAngle")
    bhv = new BHV_2VAngle(domain);
#endif

  return(bhv);
}







