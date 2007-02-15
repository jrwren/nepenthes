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

#include <ctype.h>
//#include <openssl/ssl.h>

#include "sqlhandler-postgres.hpp"

#include "Socket.hpp"
#include "SocketManager.hpp"

#include "SQLManager.hpp"
#include "LogManager.hpp"


#include "SQLCallback.hpp"
#include "SQLResult.hpp"

#include "Config.hpp"

#include "DNSManager.hpp"
#include "DNSResult.hpp"
#include "DNSQuery.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;


Nepenthes *g_Nepenthes;

SQLHandlerFactoryPostgres::SQLHandlerFactoryPostgres(Nepenthes *nepenthes)
{
	m_ModuleName        = "sqlhandler-postgres";
	m_ModuleDescription = "use postgres' async socket interface for smooth queries";
	m_ModuleRevision    = "$Rev$";


	m_Nepenthes = nepenthes;
	g_Nepenthes = nepenthes;

	m_DBType = "postgres";
}

SQLHandlerFactoryPostgres::~SQLHandlerFactoryPostgres()
{

}

bool SQLHandlerFactoryPostgres::Init()
{
#ifdef HAVE_POSTGRES
	g_Nepenthes->getSQLMgr()->registerSQLHandlerFactory(this);
	return true;
#else
	logCrit("Could not load module, compiled without support for postgres\n");
	return false;
#endif
}

bool SQLHandlerFactoryPostgres::Exit()
{

	return true;
}



/**
 * creates a postgres sql handler
 * 
 * @param server  the server
 * @param user    the username to login
 * @param passwd  the password to use
 * @param table   the database to use
 * @param options the options to use
 * 
 * @return pointer to the created SQLHandler
 */
SQLHandler *SQLHandlerFactoryPostgres::createSQLHandler(string server, string user, string passwd, 
														string table, string options, SQLCallback *cb)
{
#ifdef HAVE_POSTGRES
	return new SQLHandlerPostgres(m_Nepenthes, server, user, passwd, table, options, cb);
#else
	return NULL;
#endif

}


#ifdef HAVE_POSTGRES
/**
 * the constructor
 * 
 * @param nepenthes
 * @param server
 * @param user
 * @param passwd
 * @param table
 * @param options
 */
SQLHandlerPostgres::SQLHandlerPostgres(Nepenthes *nepenthes, string server, string user, string passwd, 
									   string table, string options, SQLCallback *cb)
{

	m_SQLHandlerName    = "sqlhandler-postgres";
	m_Nepenthes = nepenthes;
	m_LockSend = false;

	m_PGConnection = NULL;

	m_RemoteHost= server;

	m_PGServer  = "";
	m_PGTable   = table;
	m_PGUser    = user;
	m_PGPass    = passwd;
	m_PGOptions	= options;

	m_Callback = cb;
}

SQLHandlerPostgres::~SQLHandlerPostgres()
{
	logPF();
	Exit();
}


/**
 * the postgres default notice processor
 * 
 * @param arg     we use a pointer to the PGCon as arg
 * @param message the postgres message
 */
void SQLHandlerPostgres::defaultNoticeProcessor(void * arg, const char * message)
{
	PGconn *conn = (PGconn *)arg;
    logWarn("%s@%s:%s db %s: %s\n",
			PQuser(conn),
			PQhost(conn),
			PQport(conn),
			PQdb(conn),
			message);
}


/**
 * Module::Init()
 * 
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool SQLHandlerPostgres::Init()
{

	g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_RemoteHost.c_str(),NULL);

	PQsetNoticeProcessor(m_PGConnection, SQLHandlerPostgres::defaultNoticeProcessor, (void *)m_PGConnection);
	m_TimeoutIntervall = 1;
	m_ConnStatusType = CONNECTION_BAD;

	m_Polled = false;

	return true;
}


bool SQLHandlerPostgres::Exit()
{
	logPF();
	if (m_PGConnection != NULL)
	{
		PQfinish(m_PGConnection);
		m_PGConnection = NULL;
		g_Nepenthes->getSocketMgr()->removePOLLSocket(this);
	}
	
	return true;
}


/**
 * if the connection is established, 
 *  not locked, 
 *  and no other query pending, 
 * send the query, 
 * else 
 *  queue it
 * 
 * @param query  the query
 * 
 * @return true on success, false on failure
 */
bool SQLHandlerPostgres::runQuery(SQLQuery *query)
{

	logPF();
	m_Queries.push_back(query);
	if ( PQstatus(m_PGConnection) == CONNECTION_OK && PQisBusy(m_PGConnection) == 0 && m_LockSend == false )
	{
		logInfo("sending query %s\n",m_Queries.front()->getQuery().c_str());
		int ret = PQsendQuery(m_PGConnection, m_Queries.front()->getQuery().c_str());
		if ( ret != 1 )
			logCrit("ERROR %i %s\n",ret,PQerrorMessage(m_PGConnection));
	}

	return true;
}

/**
 * 
 * @param str    escape the string
 * 
 * @return the escaped string
 */
string SQLHandlerPostgres::escapeString(string *str)
{
	int size = str->size() * 2 + 1 ;
	char *escaped = (char *)malloc(size);
	size = PQescapeString(escaped,str->c_str(),str->size());
	string result(escaped,size);
	free(escaped);
	return result;
}

/**
 * 
 * @param str    escape a binary (bytea)
 * 
 * @return the escaped binary as (bytea) string
 */
string SQLHandlerPostgres::escapeBinary(string *str)
{

	unsigned char *res;
	size_t size;
	res = PQescapeBytea((const unsigned char *)str->c_str(),str->size(),&size);
	string result((char *)res,(int)size-1);
	PQfreemem(res);
	return result;
}

/**
 * 
 * @param str    (bytea) string
 * 
 * @return the unescaped binary
 */
string SQLHandlerPostgres::unescapeBinary(string *str)
{
	logPF();
	unsigned char *res;
	size_t size;
	res = PQunescapeBytea((unsigned char *)str->c_str(),&size);
	string result((char *)res,(int)size);
	PQfreemem(res);
	return result;
}


/**
 * do we want to send something?
 * 
 * if the postgres connection is established, 
 * check if there is stuff to send in the PGCon using PGflush
 * PGflush will try to flush whats left
 * if PGflush returns 1 there is still stuff to send, 
 * and we will poll for writeability
 * 
 * if the connection is bad, we disconnect
 * 
 * if the connection is pending, we check which type of polling the connection requested
 * PGRES_POLLING_ACTIVE is the initial state, 
 * it means we have to call PGConnectPoll and check if we want to send
 * 
 * @return true if we want to send, else false
 */
bool SQLHandlerPostgres::wantSend()
{
//	logPF();
	switch ( PQstatus(m_PGConnection) )
	{
	case CONNECTION_OK:
		if ( PQflush(m_PGConnection) == 1 )
			return true;
		else
			return false;
		break;

	case CONNECTION_BAD:
		disconnected();
		return false;
		break;

	default:
		if (m_PollingStatusType == PGRES_POLLING_WRITING)
			return true;

		if (m_PollingStatusType == PGRES_POLLING_ACTIVE)
			if ( (m_PollingStatusType = PQconnectPoll(m_PGConnection)) == PGRES_POLLING_WRITING )
				return true;

	}
	return false;
}


/**
 * if the connection is established, call PQflush, and done
 * 
 * if the connection is bad, disconnect
 * 
 * if the connection is pending, check 
 *  if the connection told us in the previous PQconnectPoll it wants to send
 *  if so, call PQconnectPoll
 *    if this call to PQconnectPoll establishes the connection, call connect
 * 
 * @return 1
 */
int32_t SQLHandlerPostgres::doSend()
{
	logPF();
	switch ( PQstatus(m_PGConnection) )
	{
	case CONNECTION_OK:
		PQflush(m_PGConnection);
		break;

	case CONNECTION_BAD:
		disconnected();
		break;

	default:
		if (m_PollingStatusType == PGRES_POLLING_WRITING)
		{
        		m_PollingStatusType = PQconnectPoll(m_PGConnection);
				if (PQstatus(m_PGConnection) == CONNECTION_OK )
				{
					connected();
				}
		}
	}

	m_LastAction = time(NULL);
	return 1;
}

/**
 * if the connection is bad, disconnect
 * 
 * if the connection is established,
 *  call PQconsumeInput
 *  if PGisBusy returns != 0, a query is done, and we can process it
 *  while PQgetResult returns != NULL,
 *   we retrieve data and add it to our result
 *   we recognize bytea data by oid, and unescape it
 *   once this is done, we call the callback for the result
 *   while the callback is running
 *    we have to lock the connection, 
 *    to make sure the callback does not add a query 
 *    which could interfere with the current query.
 *   once the callback is done, check if there are queries to be sent, 
 *   if so, sent them
 *    
 * 
 * 
 * if the connection is pending,
 *  check if the previous call to PQconnectPoll
 *  told us we want to recv data, if so, call PQconnectPoll
 *  else, there is something wrong with the connection (connection refused)
 *  and we call PQconnectPoll and check for a dead connection.
 * 
 * @return 
 */
int32_t SQLHandlerPostgres::doRecv()
{
	logPF();

	switch ( PQstatus(m_PGConnection) )
	{
	case CONNECTION_BAD:
		disconnected();
		break;

	case CONNECTION_OK:
		{

			if ( PQconsumeInput(m_PGConnection) != 1 )
			{
				logInfo("PQcomsumeInput() failed %s\n",PQerrorMessage(m_PGConnection));
				disconnected();
				return 1;
			}

			if ( PQisBusy(m_PGConnection) != 0 )
				return 1;

			if ( PQstatus(m_PGConnection) == CONNECTION_BAD )
			{
				logInfo("PQstatus() says BAD %s\n",PQerrorMessage(m_PGConnection));
				disconnected();
				return 1;
			}

			if ( m_Queries.size() == 0 )
			{
//				logCrit("Why did I end up here %s:%i?\n status %i \n message %s? \n",__FILE__,__LINE__, PQstatus(m_PGConnection),PQerrorMessage(m_PGConnection));
				return 1;
			}

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
							string bar;
							string baz;

							for ( i=0;i<PQnfields(res);i++ )
							{
								switch ( PQftype(res,i) )
								{
								case 17: // BYTEAOID
									bar = PQgetvalue(res, j, i);
									baz = unescapeBinary(&bar);
									foo[PQfname(res,i)] = string(baz.data(),baz.size());
									break;

								default:
									foo[PQfname(res,i)] = PQgetvalue(res, j, i);
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
				if ( broken_query == true )
				{
					sqlquery->getCallback()->sqlFailure(sqlresult);
				}
				else
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
		}
		break;

	default:
		if (m_PollingStatusType == PGRES_POLLING_READING)
		{
        		m_PollingStatusType = PQconnectPoll(m_PGConnection);
				if (PQstatus(m_PGConnection) == CONNECTION_OK)
					connected();
				else 
				if (PQstatus(m_PGConnection) == CONNECTION_BAD)
					logCrit("ERROR %s\n",PQerrorMessage(m_PGConnection));

		}else
		{
			m_PollingStatusType = PQconnectPoll(m_PGConnection);
			if (PQstatus(m_PGConnection) == CONNECTION_BAD)
			{
				logCrit("ERROR %s\n",PQerrorMessage(m_PGConnection));
			}
		}
	}

	m_LastAction = time(NULL);
	return 1;
}

/**
 * if the connection is not bad, established or pending, 
 * return the socket of the connection, else -1, 
 * so the socket is not polled by the mainloop, 
 * but we can use the timeout checks to reconnect
 * 
 * @return 
 */
int32_t SQLHandlerPostgres::getSocket()
{
	if ( PQstatus(m_PGConnection) != CONNECTION_BAD )
	{
		return PQsocket(m_PGConnection);
	}
	else
	{
		return -1;
	}
}

/**
 * call plain getsockopt on out socket, nothing special
 * 
 * @param level
 * @param optname
 * @param optval
 * @param optlen
 * 
 * @return 
 */
int32_t SQLHandlerPostgres::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
{
   	return getsockopt(getSocket(), level, optname, optval, optlen);
}


/**
 * if the connection was bad, we call this, 
 *   so we can print out information about the loss
 */
void SQLHandlerPostgres::disconnected()
{
	logPF();
	if ( PQstatus(m_PGConnection) == CONNECTION_BAD )
	{
		logWarn("PostgreSQL Server disconnected - %i queries in queue - reconnecting in %i seconds\nMessage: %s",
				m_Queries.size(),
				m_TimeoutIntervall, PQerrorMessage(m_PGConnection));
		m_ConnStatusType = CONNECTION_BAD;
		m_LastAction = time(NULL);
		m_Callback->sqlDisconnected();
	}
}

/**
 * to reconnect, 
 * we ask the dnsmanager to resolve the host which was provided
 * the actual reconnect attempt is done in the dns callback
 */
void SQLHandlerPostgres::reconnect()
{
	logPF();
//	PQresetStart(m_PGConnection);
//	m_PollingStatusType = PGRES_POLLING_ACTIVE;
//	m_LastAction = time(NULL);
	g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_RemoteHost.c_str(),NULL);
}


/**
 * if the connection moves from pending to established,
 * we call this
 *  so we can print out the information
 *  and start running the queue'd queries.
 */
void SQLHandlerPostgres::connected()
{
	logPF();
	if (PQstatus(m_PGConnection) == CONNECTION_OK)
	{
		m_ConnStatusType = CONNECTION_OK;

		logInfo("Connected %s@%s:%s DB %s BackendPID %i ServerVersion %i ProtocolVersion %i\n",
				PQuser(m_PGConnection),
				PQhost(m_PGConnection) ? m_RemoteHost.c_str() : m_RemoteHost.c_str(),
				PQport(m_PGConnection),
				PQdb(m_PGConnection),
				PQbackendPID(m_PGConnection),
#ifdef HAVE_PQSERVERVERSION
				PQserverVersion(m_PGConnection),
#else
				-1,
#endif 
				PQprotocolVersion(m_PGConnection));

		m_LastAction = time(NULL);

#ifdef HAVE_SSL
		SSL *sslctx;
		X509 *peer;
		char issuer[256];
		char subject[256];
		char peer_CN[256];
		

		if ((sslctx = PQgetssl(m_PGConnection)) != NULL)
		{	
			peer=SSL_get_peer_certificate(sslctx);
			X509_NAME_oneline(X509_get_issuer_name(peer), issuer, 256);
			X509_NAME_oneline(X509_get_subject_name(peer), subject, 256); 
			X509_NAME_get_text_by_NID(X509_get_subject_name (peer),  NID_commonName,  peer_CN, 256);
			logInfo("Using SSL connection %s %s\n"
					"\tIssuer: %s\n"
					"\tSubject: %s\n"
					"\tPeer CN: %s\n",
					SSL_get_cipher_name(sslctx),SSL_get_cipher_version(sslctx),
					issuer,
					subject);
			
		}
#endif

		m_Callback->sqlConnected();

		if ( m_Queries.size() > 0 )
		{
			logInfo("sending query %s\n",m_Queries.front()->getQuery().c_str());
			int ret = PQsendQuery(m_PGConnection, m_Queries.front()->getQuery().c_str());
			if ( ret != 1 )
				logCrit("ERROR %i %s\n",ret,PQerrorMessage(m_PGConnection));
		}

	}

}

/**
 * this is the reconnect with timeout handler
 * 
 * the socket manager will call checkTimeout for every socket, 
 * even the dead, we use this here
 * 
 * if this connection is dead, we wait a given timeout (1 second now)
 * and then we call reconnect()
 * 
 * @return false
 */
bool SQLHandlerPostgres::checkTimeout()
{
	if (PQstatus(m_PGConnection) == CONNECTION_BAD &&
		m_LastAction + m_TimeoutIntervall < time(NULL) )
	{
//		logSpam("TIMEOUTDEBUG %i %i %i %i \n",m_LastAction,m_TimeoutIntervall,m_LastAction + m_TimeoutIntervall, time(NULL) );
		handleTimeout();
	}
	return false;
}

/**
 * just calls reconnect
 * 
 * @return false
 */
bool SQLHandlerPostgres::handleTimeout()
{
	logPF();
	reconnect();
	return false;
}

/**
 * if the hostname got resolved, we can create a new new PGCon of it, 
 * maybe finish the old one first if there was one
 * if there was no PGCon, this is the first connection attempt, and we ask the socketmanager to poll us.
 * 
 * @param result
 * 
 * @return 
 */
bool SQLHandlerPostgres::dnsResolved(DNSResult *result)
{
	logPF();


	if ( result->getQueryType() & DNS_QUERY_A )
	{

		list <uint32_t> resolved = result->getIP4List();

		list <uint32_t>::iterator it;
		for ( it=resolved.begin();it!=resolved.end();it++ )
		{
			logSpam( "domain %s has ip %s \n",result->getDNS().c_str(),inet_ntoa(*(in_addr *)&*it));
		}

		m_PGServer = inet_ntoa(*(in_addr *)&resolved.front());
	}

	string ConnectString;
	ConnectString = 
	"hostaddr = '" + m_PGServer + 
	"' dbname = '" + m_PGTable + 
	"' user = '" + m_PGUser + 
	"' password = '" + m_PGPass +"'";

	if (m_PGOptions.size() > 0)
    	ConnectString += m_PGOptions;
	

	if (m_PGConnection != NULL)
		PQfinish(m_PGConnection);
	else
		g_Nepenthes->getSocketMgr()->addPOLLSocket(this);

	m_PGConnection = PQconnectStart(ConnectString.c_str());
	m_PollingStatusType = PGRES_POLLING_ACTIVE;
	m_ConnStatusType = CONNECTION_BAD;

    return true;
}

bool SQLHandlerPostgres::dnsFailure(DNSResult *result)
{
	logPF();
	logCrit("SQLHandlerPostgres could not resolve domain %s to connect database\n",m_RemoteHost.c_str());

	g_Nepenthes->stop();
	return true;
}

#endif // HAVE_POSTGRES

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new SQLHandlerFactoryPostgres(nepenthes);
		return 1;
	}
	else
	{
		return 0;
	}
}

