/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_Rater.h                                           */
/*    DATE: Nov 14, 2000 (separated from RandOF.H)               */
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

#ifndef OF_RATER_HEADER
#define OF_RATER_HEADER

#include "PDMap.h"
#include "AOF.h"

class OF_Rater {
public:
  OF_Rater(const PDMap *pdmap=0, const AOF *aof=0);
  virtual ~OF_Rater() {if(err) delete [] err;};

public: 
  void    setPDMap(const PDMap*);
  void    setAOF(const AOF*);
  void    takeSamples(int i=1000, double wi=0.0);
  int     getSampleCount()  {return(sampleCount);};
  double  getSampHigh()     {return(sampHigh);};
  double  getSampLow()      {return(sampLow);};

  void    resetSamples();
  double  getAvgErr() const;
  double  getWorstErr() const;
  double  getSquaredErr() const;

protected:
  const AOF*   aof;
  const PDMap* pdmap;
  
  int     sampleCount;   // Count of samples taken so far
  double  totalErr;      // Total Err of all samples so far
  double  worstErr;      // Worst val of all samples so far
  double  squaredErr;
  double  sampHigh;      // Highest value of samples
  double  sampLow;       // Lowest  value of samples
  double* err;           // Err val of all samples so far
};

#endif










