/********************************************************************************
 *                              Library
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
 *             contact librarydev@users.sourceforge.net  
 *
 *******************************************************************************/

 /* $Id$ */

#include <ctype.h>

#include "sqlhandler-postgres.hpp"

#include "Socket.hpp"
#include "SocketManager.hpp"

#include "SQLManager.hpp"
#include "LogManager.hpp"

#include "SQLQuery.hpp"
#include "SQLCallback.hpp"
#include "SQLResult.hpp"

#include "Config.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace library;


/**
 * as we may need a global pointer to our Library in our modules,
 * and cant access the cores global pointer to library
 * we have to use a own global pointer to library per module
 * we need this pointer for logInfo() etc
 */
Library *g_Library;

/**
 * The Constructor
 * creates a new SQLHandlerPostgres Module, 
 * SQLHandlerPostgres is an example for binding a socket & setting up the Dialogue & DialogueFactory
 * 
 * 
 * it can be used as a shell emu to allow trigger commands 
 * 
 * 
 * sets the following values:
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param library the pointer to our Library
 */
SQLHandlerPostgres::SQLHandlerPostgres(Library *library)
{
	m_ModuleName        = "sqlhandler-postgres";
	m_ModuleDescription = "use postgres async interface";
	m_ModuleRevision    = "$Rev$";
	m_SQLHandlerName	= "sqlhandler-postgres";

	m_Library = library;

	g_Library = library;
	m_LockSend = false;
}

SQLHandlerPostgres::~SQLHandlerPostgres()
{

}


/**
 * Module::Init()
 * 
 * binds the port, adds the DialogueFactory to the Socket
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool SQLHandlerPostgres::Init()
{
	try
	{
		m_PGServer= g_Library->getConfig()->getValString("library.database.server");   
		m_PGTable= g_Library->getConfig()->getValString("library.database.table");    
		m_PGUser= g_Library->getConfig()->getValString("library.database.user");     
		m_PGPass= g_Library->getConfig()->getValString("library.database.pass");
	}catch (...)
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	string ConnectString;
	ConnectString = "hostaddr = '" + m_PGServer + "' dbname = '" + m_PGTable + "' user = '" + m_PGUser + "' password = '" + m_PGPass +"'";

	m_PGConnection = PQconnectdb(ConnectString.c_str());

	if ( PQstatus(m_PGConnection) != CONNECTION_OK )
	{
		logCrit("Could not connect to PostgreSQL Database: %s!\n", PQerrorMessage(m_PGConnection));
		return false;
	} else
		logDebug("%s Connected PostgreSQL Database \n", __PRETTY_FUNCTION__);

	g_Library->getSQLMgr()->setSQLHandler(this);
	PQsetnonblocking(m_PGConnection,1);

	g_Library->getSocketMgr()->addPOLLSocket(this);


	return true;
}

bool SQLHandlerPostgres::Exit()
{
	return true;
}

bool SQLHandlerPostgres::runQuery(SQLQuery *query)
{
	logPF();
	m_Queries.push_back(query);
	if (PQisBusy(m_PGConnection) == 0 && m_LockSend == false)
	{
		logInfo("sending query %s\n",m_Queries.front()->getQuery().c_str());
		int ret = PQsendQuery(m_PGConnection, m_Queries.front()->getQuery().c_str());
		if (ret != 1)
			logCrit("ERROR %i %s\n",ret,PQerrorMessage(m_PGConnection));
	}
	return true;
}

string SQLHandlerPostgres::escapeString(string *str)
{
	int size = str->size() * 2 + 1 ;
	char *escaped = (char *)malloc(size);
	size = PQescapeString(escaped,str->c_str(),str->size());
	string result(escaped,size);
	free(escaped);
	return result;
}

string SQLHandlerPostgres::escapeBinary(string *str)
{
	
	unsigned char *res;
	size_t size;
	res = PQescapeBytea((const unsigned char *)str->c_str(),str->size(),&size);
	string result((char *)res,(int)size);
	PQfreemem(res);
	return result;
}

string SQLHandlerPostgres::unescapeBinary(string *str)
{
	
	unsigned char *res;
	size_t size;
	res = PQunescapeBytea((unsigned char *)str->c_str(),&size);
	string result((char *)res,(int)size);
	PQfreemem(res);
	return result;
}


bool SQLHandlerPostgres::wantSend()
{
//	logPF();
	if (PQflush(m_PGConnection) == 1)
		return true;
	else
		return false;
}


int32_t SQLHandlerPostgres::doSend()
{
	logPF();
	PQflush(m_PGConnection);
	return 1;
}

int32_t SQLHandlerPostgres::doRecv()
{
	logPF();
	if ( PQconsumeInput(m_PGConnection) != 1 )
		return 1;

	if ( PQisBusy(m_PGConnection) != 0 )
		return 1;

	PGresult   *res=NULL;
	PGSQLResult *sqlresult = NULL;
	SQLQuery *sqlquery = m_Queries.front();
	m_Queries.pop_front();

//	int foo = rand()%1024;

	vector< map<string,string> >        result;
	bool broken_query=false;

	while ( (res = PQgetResult(m_PGConnection)) != NULL )
	{
//		logCrit("README %i %x %x\n",foo,res,sqlquery);
		switch ( PQresultStatus(res) )
		{
		case PGRES_COMMAND_OK:
			break;
		case PGRES_TUPLES_OK:
			if ( sqlquery->getCallback() != NULL )
			{
				int i,j;
				for ( j = 0;  j < PQntuples(res); j++ )
				{
					map<string,string> foo;
					for ( i=0;i<PQnfields(res);i++ )
					{
						if ( PQfformat(res,i) == 0 )
						{
							foo[PQfname(res,i)] = PQgetvalue(res, j, i);
						} else
						{
							string bar = PQgetvalue(res, j, i);
							foo[PQfname(res,i)] = unescapeBinary(&bar);
						}
					}
					result.push_back(foo);
				}
			}
			break;

		default:
			logCrit("Query failure. Query'%s' Error '%s' ('%s')\n",
					sqlquery->getQuery().c_str(),
					PQresStatus(PQresultStatus(res)),
					PQresultErrorMessage(res));
			broken_query = true;
		}

		PQclear(res);

	}
	if ( sqlquery->getCallback() != NULL )
	{
		m_LockSend = true;

		sqlresult = new PGSQLResult(&result,sqlquery->getQuery(),sqlquery->getObject());
		if (broken_query == true)
		{
			sqlquery->getCallback()->sqlFailure(sqlresult);
		}else
		{
			sqlquery->getCallback()->sqlSuccess(sqlresult);
		}
		
		delete sqlresult;
		m_LockSend = false;

	}

	delete sqlquery;



	if ( m_Queries.size() > 0 )
	{
		logInfo("sending query %s\n",m_Queries.front()->getQuery().c_str());
		int ret = PQsendQuery(m_PGConnection, m_Queries.front()->getQuery().c_str());
		if ( ret != 1 )
			logCrit("ERROR %i %s\n",ret,PQerrorMessage(m_PGConnection));
	}
	return 1;
}

int32_t SQLHandlerPostgres::getSocket()
{
	return PQsocket(m_PGConnection);
}

int32_t SQLHandlerPostgres::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
{
        return getsockopt(getSocket(), level, optname, optval, optlen);
}

extern "C" int32_t module_init(int32_t version, Module **module, Library *library)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new SQLHandlerPostgres(library);
        return 1;
    } else {
        return 0;
    }
}

