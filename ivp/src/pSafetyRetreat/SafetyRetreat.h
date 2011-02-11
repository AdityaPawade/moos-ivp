/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: SafetyRetreat.h                                 */
/*    DATE: August 2010                                     */
/************************************************************/

#ifndef SAFETY_RETREAT_HEADER
#define SAFETY_RETREA_HEADER

#include "MOOSLib.h"
#include "XYPolygon.h"

class SafetyRetreat : public CMOOSApp
{
 public:
  SafetyRetreat();
  virtual ~SafetyRetreat() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected: 
  bool handleNewPolygon(std::string);
  bool handleSafetyRetreat();

  unsigned int closestPolygon();

 protected:
  // A set of polygons where the vehicle may retreat to. Which 
  // polygon chosen is based on current vehicle position.
  std::vector<XYPolygon> m_retreat_areas;

  // A MOOS variable that will trigger the retreat alert msg.
  std::string   m_retreat_cue;

  // A MOOS variable that will contain the retreat alert msg.
  std::string   m_retreat_message;

  // A MOOS variable that will contain the retreat alert msg.
  std::string   m_retreat_notify;

  // Number of seconds recommended for the retreat
  double        m_retreat_duration;
  
  // True if more terminal I/O is desired for debugging.
  bool          m_verbose;

  double        m_osx;
  double        m_osy;

};

#endif 
