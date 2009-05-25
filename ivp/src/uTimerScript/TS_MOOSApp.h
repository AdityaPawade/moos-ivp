/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TS_MOOSApp.h                                    */
/*    DATE: May 21st 2009                                   */
/************************************************************/

#ifndef TIMER_SCRIPT_MOOS_APP_HEADER
#define TIMER_SCRIPT_MOOS_APP_HEADER

#include <vector>
#include <string>
#include "MOOSLib.h"
#include "VarDataPair.h"

class TS_MOOSApp : public CMOOSApp
{
 public:
  TS_MOOSApp();
  virtual ~TS_MOOSApp() {};

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Locally defined and locally used functions
  void RegisterVariables();
  bool addNewEvent(std::string);
  void sortEvents();
  void printScript();
  void checkForReadyPostings();
  void jumpToNextPostingTime();
  void handleReset();
  void postStatus();
  void seedRandom();

 protected: // Configuration parameters
  std::vector<VarDataPair> m_pairs;
  std::vector<double>      m_ptime;
  std::vector<bool>        m_poked;
  
  std::string m_var_next_event;
  std::string m_var_forward;
  std::string m_var_pause;
  std::string m_var_status;
  std::string m_var_reset;

  int      m_reset_max;
  double   m_reset_time;  // numerical, after-last

 protected: // State variables
  double   m_previous_time;
  double   m_elapsed_time;
  double   m_start_time;
  double   m_skip_time;
  double   m_pause_time;
  bool     m_paused;
  int      m_posted_count;
  int      m_reset_count;
};

#endif 
