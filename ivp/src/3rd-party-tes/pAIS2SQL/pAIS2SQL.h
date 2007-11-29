// t. schneider tes@mit.edu 11.29.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is pAIS2SQL.h 
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PAIS2SQLH
#define PAIS2SQLH

#include "MOOSLib.h"

// for mysql C API
#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>



class CpAIS2SQL : public CMOOSApp  
{
public:
	//standard construction and destruction
	CpAIS2SQL();
	virtual ~CpAIS2SQL();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	
	void DoRegistrations();

        void print_error (MYSQL *conn, char *message);
	void process_result_set (MYSQL *conn, MYSQL_RES *res_set);

	
	std::string m_host_name;      /* server host (default=localhost) */
	std::string m_user_name;      /* username (default=login name) */
	std::string m_password;     /* password (default=none) */
	unsigned int m_port_num;   /* port number (use built-in value) */
	std::string m_db_name;        /* database name (default=none) */
	MYSQL *conn;
};

#endif 
