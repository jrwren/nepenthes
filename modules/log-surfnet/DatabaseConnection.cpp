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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "DatabaseConnection.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

DatabaseConnection::DatabaseConnection(const char *server, const char *user, const char *passwd, const char *db)
{
	m_Server = server;
	m_User = user;
	m_Pass = passwd;
	m_DB = db;
}

DatabaseConnection::~DatabaseConnection()
{

}

bool DatabaseConnection::Init()
{
	logPF();
#ifdef HAVE_POSTGRES
	m_PGConnection = PQsetdbLogin(m_Server.c_str(),NULL,NULL,NULL,m_DB.c_str(),m_User.c_str(),m_Pass.c_str());
	if ( PQstatus(m_PGConnection) != CONNECTION_OK )
	{
		logCrit("Connection to database '%s' failed.\n", PQdb(m_PGConnection));
		logCrit("%s", PQerrorMessage(m_PGConnection));
		return false;
    }
	return true;
#else
	return false;
#endif
}

bool DatabaseConnection::Exit()
{
#ifdef HAVE_POSTGRES
	PQfinish(m_PGConnection);
	return true;
#else
	return false;
#endif
}

int32_t DatabaseConnection::getSensorID(uint32_t ip)
{
#ifdef HAVE_POSTGRES
	logPF();

	logSpam("Looking for ip %s\n",inet_ntoa(*(in_addr *)&ip));

	char *query;
	int32_t querysize;

	querysize = asprintf(&query,"SELECT * FROM sensors WHERE tapip = '%s' ;",inet_ntoa(*(in_addr *)&ip));

	PGresult   *result=NULL;

	if ( (result = PQexec(m_PGConnection, query)) == NULL )
	{
		logCrit("PGexec() failed #1 %s\n",PQerrorMessage(m_PGConnection));
		return -1;
	}


	
	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		logCrit("PGexec() failed #2 %s\n",PQerrorMessage(m_PGConnection));
		PQclear(result);
		return -1;
	}

	

	if( PQntuples(result) != 1)
	{
		logCrit("PQnTuples returned %i, expected 1, make sure the ip is listed on the server and uniq\n",PQntuples(result));
		
		return -1;
	}

	char       *id_ptr;
	int 		id_fnum;
	id_fnum = PQfnumber(result, "id");
	if (id_fnum == -1 )
	{
		logCrit("PQfnumber(.. id) is %i\n",id_fnum);
		PQclear(result);
		return -1;
	}

    id_ptr 	= PQgetvalue(result, 0, id_fnum);



	int32_t sensorid = atoi(id_ptr);

	

	logSpam("Resolved sensorid %i for ip %s\n",sensorid, inet_ntoa(*(in_addr *)&ip));

	return sensorid;
#else
	return 0;
#endif

}

int32_t DatabaseConnection::addAttack(int32_t severity, uint32_t attackerip, uint16_t attackerport, uint32_t decoyip, uint16_t decoyport, int32_t sensorid)
{
#ifdef HAVE_POSTGRES
	logPF();

	int32_t attackid=0;

	char *query;
	int32_t querysize;

	string attackerhost = inet_ntoa(*(in_addr *)&attackerip);
	string decoyhost = inet_ntoa(*(in_addr *)&decoyip);

	querysize = asprintf(&query,"INSERT INTO attacks (severity,timestamp,dest,dport,source,sport,sensorid) VALUES ('%i','%i','%s','%i','%s','%i','%i')",
						 severity,(int)time(NULL),decoyhost.c_str(), decoyport, attackerhost.c_str(), attackerport, sensorid);



	printf("%s",query);


	PGresult *result;

	if ( (result = PQexec(m_PGConnection, query)) == NULL )
	{
		 logCrit("PGexec() failed #1 %s\n",PQerrorMessage(m_PGConnection));
		 return -1;
	}



	 if (PQresultStatus(result) != PGRES_COMMAND_OK)
	 {
		 logCrit("PGexec() failed #2 %s\n",PQerrorMessage(m_PGConnection));
		 PQclear(result);
		 return -1;
	 }
	 

	 PQclear(result);

 	 if ( (result = PQexec(m_PGConnection, "SELECT currval('\"attacks_id_seq\"')")) == NULL )
	 {
		   logCrit("PGexec() failed #3 %s\n",PQerrorMessage(m_PGConnection));
	 }

	 char       *id_ptr;
	 int 		id_fnum;
	 id_fnum = PQfnumber(result, "currval");
	 if (id_fnum == -1 )
	 {
		 logCrit("PQfnumber(.. id) is %i\n",id_fnum);
		 PQclear(result);
		 return -1;
	 }

	 id_ptr 	= PQgetvalue(result, 0, id_fnum);
	 attackid = atoi(id_ptr);


	 logSpam("PQoidStatus(..) = %s \n",PQoidStatus(result));

	free(query);
	PQclear(result);
	

	return attackid;
#else
	return 0;
#endif


}

void DatabaseConnection::updateAttackSeverity(int32_t attackid, int32_t newseverity)
{
#ifdef HAVE_POSTGRES
	char *query;
	int32_t querysize;

	querysize = asprintf(&query,"UPDATE attacks SET severity = '%i' WHERE id = '%i' ",
						 newseverity, attackid);


	PGresult *result;

	if ( (result = PQexec(m_PGConnection, query)) == NULL )
	{
		 logCrit("PGexec() failed #1 %s\n",PQerrorMessage(m_PGConnection));
	}



	 if (PQresultStatus(result) != PGRES_COMMAND_OK)
	 {
		 logCrit("PGexec() failed #2 %s\n",PQerrorMessage(m_PGConnection));
	 }

	 PQclear(result);

	free(query);
#endif

}

void DatabaseConnection::addDetail(int32_t attackid, int32_t sensorid, int32_t detailtype, const char *text)
{
#ifdef HAVE_POSTGRES
	logPF();

	char *query, *esctext;
	int32_t querysize;

	esctext = (char *)malloc(strlen(text)*2+1);

	PQescapeString(esctext,text,strlen(text));
	querysize = asprintf(&query,"INSERT INTO details (attackid,sensorid,type,text) VALUES ('%i','%i','%i','%s')",
						 attackid,sensorid,detailtype,text);


	PGresult *result;

	if ( (result = PQexec(m_PGConnection, query)) == NULL )
	{
		 logCrit("PGexec() failed #1 %s\n",PQerrorMessage(m_PGConnection));
		 
	}



	 if (PQresultStatus(result) != PGRES_COMMAND_OK)
	 {
		 logCrit("PGexec() failed #2 %s\n",PQerrorMessage(m_PGConnection));
         		 
	 }

	 PQclear(result);



	free(query);
	free(esctext);
	return;
#endif

}
