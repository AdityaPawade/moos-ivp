/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ScalarPID.h                                          */
/*    DATE: Oct 1 2005                                           */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

/******************************************************************/
/* This file is a slightly augmented version of P.Newman's        */
/* ScalarPID class found in MOOSGenLib.                           */
/*                                                                */
/* It is augmented w/ a copy constructor and assignment operator. */
/*                                                                */
/* All variable names remain the same, some comments are added.   */
/*                                                                */
/* Name of the class is changed from "CScalarPID" to "ScalarPID"  */
/******************************************************************/


#ifndef MOD_SCALARPID_HEADER
#define MOD_SCALARPID_HEADER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
#include <fstream>

class ScalarPID  
{
public:
  ScalarPID();
  ScalarPID(double dfKp, double dfKd,
	     double dfKi, double dfIntegralLimit,
	     double dfOutputLimit);
  ScalarPID(const ScalarPID&);    // **new **
  virtual ~ScalarPID();

  const ScalarPID &operator=(const ScalarPID&);  // **new**

  void SetGains(double dfKp,double dfKd,double dfKi);
  void SetLimits(double dfIntegralLimit, double dfOutputLimit);

  void SetGoal(double dfGoal);
  void SetLogPath(std::string & sPath);
  void SetLog(bool bLog);
  void SetName(std::string sName);
  bool Run(double dfeIn, double dfErrorTime, double& dfOut);

protected:  // Core parameters
  double m_dfKi;
  double m_dfKd;
  double m_dfKp;
  double m_dfIntegralLimit;
  double m_dfOutputLimit;

protected: // Data persistent between runs
  double m_dfeOld;
  double m_dfOldTime;
  double m_dfOut;
  int    m_nHistorySize;
  std::list<double> m_DiffHistory;
  
  double m_dfe;
  double m_dfeSum;
  double m_dfeDiff;
  double m_dfDT;

  
protected:
  bool Log();
  
  //note this is just for logging purposes...
  double  m_dfGoal;
  int     m_nIterations;

  bool          m_bLog;
  std::string   m_sName;
  std::string   m_sLogPath;
  std::ofstream m_LogFile;
};

#endif





