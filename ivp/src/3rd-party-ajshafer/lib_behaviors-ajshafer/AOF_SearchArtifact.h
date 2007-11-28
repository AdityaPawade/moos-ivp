/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: AOF_SearchArtifact.h                                 */
/*    DATE: NOV 28, 2007                                         */
/*****************************************************************/
 
#ifndef AOF_SEARCH_ARTIFACT_HEADER
#define AOF_SEARCH_ARTIFACT_HEADER

#include "AOF.h"
#include "IvPDomain.h"
#include "XYGrid.h"
#include "Timutif.h"

class AOF_SearchArtifact: public AOF {
 public:
  AOF_SearchArtifact(IvPDomain, const XYGrid*);
  ~AOF_SearchArtifact() {};

public: // virtual functions
  double evalBox(const IvPBox*) const; // virtual defined
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
  bool   initialize();
  void   fillCache();

protected:
  const XYGrid *search_grid;
  
  Timutif timutif;

  double  os_lat;
  double  os_lon;
  double  time_horizon;

  bool    os_lat_set;
  bool    os_lon_set;

  int     crs_ix;  // Index of "course" variable in IvPDomain
  int     spd_ix;  // Index of "speed" variable in IvPDomain

  std::vector<std::vector<int> > ray_cache;
};

#endif






