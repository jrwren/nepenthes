/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * 
 *             contact nepenthesdev@users.sourceforge.net  
 *
 *******************************************************************************/

 /* $Id$ */

#include "config.h"

#ifdef HAVE_POSTGRES
#include <libpq-fe.h>
#endif 

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

#include "SQLHandler.hpp"
#include "SQLHandlerFactory.hpp"

#include "POLLSocket.hpp"
#include "POLLSocket.cpp"
#include "Socket.cpp"

#include "SQLResult.hpp"
#include "SQLResult.cpp"

#include "SQLQuery.hpp"
#include "SQLQuery.cpp"

using namespace std;

namespace nepenthes
{

	class Buffer;


	class SQLHandlerFactoryPostgres : public Module , public SQLHandlerFactory
	{

	public:
		SQLHandlerFactoryPostgres(Nepenthes *nepenthes);
		~SQLHandlerFactoryPostgres();

		bool Init();
		bool Exit();

		SQLHandler * createSQLHandler(string server, string user, string passwd, string table, string options);

	};

#ifdef HAVE_POSTGRES
	class SQLHandlerPostgres : public SQLHandler , public POLLSocket 
	{
    public:
		SQLHandlerPostgres(Nepenthes *nepenthes, string server, string user, string passwd, string table, string options);
		~SQLHandlerPostgres();

		bool Init();
		bool Exit();


		bool runQuery(SQLQuery *query);
		string escapeString(string *str);
		string escapeBinary(string *str);
		string unescapeBinary(string *str);


		bool wantSend();

		int32_t doSend();
		int32_t doRecv();
		int32_t getSocket();
		int32_t   getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen);


	private:
		PGconn *m_PGConnection;
		Nepenthes	*m_Nepenthes;

		bool 	m_LockSend;

		list <SQLQuery *> 	m_Queries;

		string m_PGServer;
		string m_PGTable;
		string m_PGUser;
		string m_PGPass;

	};

	class PGSQLResult : public SQLResult
	{
	public:
		PGSQLResult(PGresult *res, string query, void *obj) : SQLResult(query,obj)
		{
			int i,j;
			for ( j = 0;  j < PQntuples(res); j++ )
			{
				map<string,string> foo;
				for ( i=0;i<PQnfields(res);i++ )
				{
					
					foo[PQfname(res,i)] = PQgetvalue(res, j, i);
//					printf(" adding %s %s\n",PQfname(res,i), PQgetvalue(res, j, i));
					
				}
				m_Result.push_back(foo);

			}
		}

		PGSQLResult(vector< map<string,string> > *result, string query, void *obj) : SQLResult(query,obj)
		{
			m_Result = *result;
		}

	};
#endif // HAVE_POSTGRES

}

extern nepenthes::Nepenthes *g_Nepenthes;
