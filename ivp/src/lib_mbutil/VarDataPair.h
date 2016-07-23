/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarDataPair.h                                        */
/*    DATE: Jul 4th 2005 Monday morning at Brueggers             */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef VAR_DATA_PAIR_HEADER
#define VAR_DATA_PAIR_HEADER

#include <string>

class VarDataPair
{
public:
  VarDataPair();

  VarDataPair(std::string varname, double varvalue);

  VarDataPair(std::string varname, std::string varvalue);

  VarDataPair(std::string varname, std::string varvalue, std::string typehint);

  virtual ~VarDataPair() {}
  
  bool        set_var(std::string);
  bool        set_key(std::string);
  bool        set_ptype(std::string);
  bool        set_sdata(std::string);
  bool        set_ddata(double);
  bool        set_smart_data(std::string);

  std::string get_var()   const {return(m_var);}
  std::string get_sdata() const {return(m_sdata);}
  double      get_ddata() const {return(m_ddata);}
  bool        is_string() const {return(m_is_string);}
  bool        is_quoted() const {return(m_is_quoted);}
  std::string get_key()   const {return(m_key);}
  std::string get_ptype() const {return(m_ptype);}

  bool        valid() const;

  std::string getPrintable();

protected:
  std::string m_var;
  std::string m_sdata;
  double      m_ddata;
  bool        m_is_string;
  bool        m_is_quoted;
  std::string m_key;
  std::string m_ptype;

  bool        m_var_set;
  bool        m_key_set;
  bool        m_ptype_set;
  bool        m_ddata_set;
  bool        m_sdata_set;
};

VarDataPair stringToVarDataPair(std::string);

#endif 





