/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: pHelmIvP.cpp                                         */
/*    DATE: Oct 12th 2004                                        */
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

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "HelmIvP.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  HelmIvP helmIvP;
  
  string sMissionFile = "pHelmIvP.moos";
  string sMOOSName = "pHelmIvP";

  switch(argc) {
  case 2:
    sMissionFile = argv[1];
  }

  for(int i=2; i<argc; i++) {
    if(strContains(argv[i], ".bhv"))
      helmIvP.addBehaviorFile(argv[i]);
  }

  helmIvP.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}










