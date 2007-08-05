/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IPFViewer.cpp                                        */
/*    DATE: Feb 13, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "IPFViewer.h"

using namespace std;

IPFViewer::IPFViewer(CPA_Model* g_model, int g_x, int g_y, 
		     int g_width, int g_height, const char *g_l)
 : Common_IPFViewer(g_x, g_y, g_width, g_height, g_l)
{
  m_cpa_model = g_model;
  m_view_type = 0;

  m_clear_red   =  73.0 / 256.0;
  m_clear_green =  62.0 / 256.0;
  m_clear_blue  = 120.0 / 256.0;

  m_clear_red   =  1.0;
  m_clear_green =  1.0;
  m_clear_blue  =  1.0;
}

// ----------------------------------------------------------
// Procedure: setTypeIPF
//   Purpose: 

void IPFViewer::setTypeIPF(const string& str)
{
  if(str == "cpa")
    m_view_type = 0;
  else if(str == "wpt")
    m_view_type = 1;
  else if(str == "avd")
    m_view_type = 2;
}


// ----------------------------------------------------------
// Procedure: resetIPF
//   Purpose: 

void IPFViewer::resetIPF(int cn_index, int unif_units)
{
  if(unif_units < 1)
    return;

  IvPFunction *ipf = 0;
  if(m_view_type == 0)
    ipf = m_cpa_model->calc_cpa_ipf(cn_index, unif_units);
  else if(m_view_type == 1)
    ipf = m_cpa_model->calc_wpt_ipf(cn_index, unif_units);
  else if(m_view_type == 2)
    ipf = m_cpa_model->calc_avd_ipf(cn_index, unif_units);

  applyIPF(ipf, true);
}

