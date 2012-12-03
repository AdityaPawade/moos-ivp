/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsMarker.h                                */
/*    DATE: May 12, 2011                                         */
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
 
#ifndef XY_FORMAT_UTILS_MARKER_HEADER
#define XY_FORMAT_UTILS_MARKER_HEADER

#include <string>
#include "XYMarker.h"

//---------------------------------------------------------------
// Create an XYMarker from a string specification. This function will
// call one of the string*2Marker functions below. This is the only
// function that should be called by the user. Other functions
// are subject to change without regard to backward compatibility.
XYMarker string2Marker(std::string);

// *** IMPORTANT NOTE **** 
// The below functions are not intended to be invoked directly by 
// the user. They are subortinate functions called by the above
// function. While the above function will be maintained in the 
// future for backward compatibility, the below functions are not.

//---------------------------------------------------------------
// #1 Create an XYMarker from a string specification. 
// This function is standard because it processes the string format
// used when a string is created from an existing XYMarker instance.
// Example: x=2,y=-8,type=triangle,width=44,primary_color=red, 
//          secondary_color=blue, label=home,active=true
XYMarker stringStandard2Marker(std::string);

#endif


