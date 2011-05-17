/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BehaviorReport.h                                     */
/*    DATE: May 10th 2011                                        */
/*****************************************************************/

#ifndef BEHAVIOR_REPORT_HEADER
#define BEHAVIOR_REPORT_HEADER

#include <string>
#include <vector>
#include "IvPFunction.h"

class BehaviorReport {
public:
  BehaviorReport();
  virtual ~BehaviorReport() {};
  
  bool addIPF(IvPFunction*, std::string="");
  
  unsigned int size() const    {return(m_keys.size());};
  bool         isEmpty() const {return(m_keys.empty());};

  IvPFunction *getIPF(unsigned int index) const;
  IvPFunction *getIPF(std::string key) const;

 protected:
  std::vector<std::string>  m_keys;
  std::vector<IvPFunction*> m_ipfs;

  bool  m_keys_unique;
};

#endif 
