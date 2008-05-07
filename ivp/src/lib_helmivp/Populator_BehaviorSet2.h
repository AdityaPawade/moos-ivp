/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_BehaviorSet.h                              */
/*    DATE: Jan 27 2005 NICOP funding approved by D.Wagner       */
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

#ifndef POPULATOR_BEHAVIORSET2_HEADER
#define POPULATOR_BEHAVIORSET2_HEADER

#include <string>
#include <set>
#include <map>
#include "BehaviorSet.h"
#include "VarDataPair.h"
#include "IvPDomain.h"
#include "IvPBehavior.h"
#include "InfoBuffer.h"
#include "BehaviorFactory.h"

class Populator_BehaviorSet2 {
public:
  Populator_BehaviorSet2(IvPDomain, InfoBuffer*);
  ~Populator_BehaviorSet2() {};

  void load_behavior_libs(std::string dirname);

  BehaviorSet* populate(std::set<std::string>);
  BehaviorSet* populate(std::string filename);

protected:
  bool handleLine(std::string);
  bool handleEntry(std::string, std::string, std::string);

  IvPBehavior *initializeBehavior(std::string);

  void loadBehaviors(const std::string & lib_directory);

protected:
  BehaviorFactory bhv_factory;

  std::vector<IvPBehavior*>  behaviors;
  std::vector<VarDataPair>   initial_vars;
  std::vector<VarDataPair>   default_vars;

  IvPDomain    domain;
  InfoBuffer*  info_buffer;
  int          define_mode;
  bool         open_behavior_mode;

  bool ok;
  
};
#endif


















