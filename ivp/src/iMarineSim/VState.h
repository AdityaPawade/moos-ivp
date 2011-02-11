/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VState.h                                             */
/*    DATE: Oct 25th 2004                                        */
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

#ifndef VSTATE_HEADER
#define VSTATE_HEADER

#include <string>

// A self explanatory class encapsulating the vehicle's state. 
// This is essentially a data package that keeps things neat
class VState
{
public:
  VState();
  ~VState() {};
  std::string toString();

public:
  double m_dfX;                  // X position
  double m_dfY;                  // Y position
  double m_dfHeading;            // Heading position (radians)
  double m_dfHeadingOverGround;  // Heading over ground (degrees)
  double m_dfSpeed;              // Speed
  double m_dfSpeedOverGround;    // Speed over ground
  double m_dfDepth;              // Depth
  double m_dfTime;               // Time
};

#endif 
