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

#include "x-9.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"

#include "SQLManager.hpp"
#include "SQLResult.hpp"
#include "SQLQuery.hpp"
#include "SQLHandler.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

#include <cstring>

using namespace nepenthes;

/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * The Constructor
 * creates a new X9 Module, 
 * X9 is an example for binding a socket & setting up the Dialogue & DialogueFactory
 * 
 * 
 * it can be used as a shell emu to allow trigger commands 
 * 
 * 
 * sets the following values:
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
X9::X9(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-9";
	m_ModuleDescription = "eXample Module 9 -using the sql interface-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "x-9 Factory";
	m_DialogueFactoryDescription = "eXample Dialogue Factory";

	g_Nepenthes = nepenthes;

}

X9::~X9()
{

}


bool X9::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,10007,0,45,this);
	return true;
}

bool X9::Exit()
{
	return true;
}

Dialogue *X9::createDialogue(Socket *socket)
{
	return new X9Dialogue(socket);
}


X9Dialogue::X9Dialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "X9Dialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;
	m_SQLCallbackName = "x-9 eXample module";

	m_Socket->doRespond("Welcome to sql Shell\n",strlen("Welcome to sql Shell\n"));

	m_SQLHandler = g_Nepenthes->getSQLMgr()->createSQLHandler("postgres",
															  string("192.168.53.21"),
															  string("postgres"),
															  string(""),
															  string("abm"),
															  string(""),
															  this);
}

X9Dialogue::~X9Dialogue()
{
	logPF();
	while (m_OutstandingQueries.size() > 0)
	{
		m_OutstandingQueries.front()->cancelCallback();
		m_OutstandingQueries.pop_front();
	}
	delete m_SQLHandler;
}

ConsumeLevel X9Dialogue::incomingData(Message *msg)
{

	string query(msg->getMsg(),msg->getSize());
	SQLQuery *sqlq = m_SQLHandler->addQuery(&query,this,NULL);

	m_OutstandingQueries.push_back(sqlq);

	return CL_ASSIGN;
}

ConsumeLevel X9Dialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

ConsumeLevel X9Dialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel X9Dialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel X9Dialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}


bool X9Dialogue::sqlSuccess(SQLResult *result)
{
	logPF();


	logSpam("Query %s had success (%i results)\n",result->getQuery().c_str(),(*(result->getResult())).size());

	vector< map<string,string> > resvec = *result->getResult();

	vector< map<string,string> >::iterator it = resvec.begin();
	map<string,string>::iterator jt;


	string msg;

	for (jt = it->begin(); jt != it->end(); jt++ )
	{
		msg = msg + "| " + jt->first + " ";
	}

	msg += "|\n";

	for (it = resvec.begin(); it != resvec.end(); it++)
	{

		for (jt = it->begin(); jt != it->end(); jt++ )
		{
			msg = msg + "| " + string((*it)[jt->first].data(),(*it)[jt->first].size()) + " ";
		}
		msg += "|\n";
	}

	logInfo("%s\n",msg.c_str());
	m_Socket->doWrite((char *)msg.c_str(),msg.size());
	
	m_OutstandingQueries.pop_front();

//	g_Nepenthes->getUtilities()->hexdump((byte *)msg.data(),msg.size());
	return true;
}


bool X9Dialogue::sqlFailure(SQLResult *result)
{
	logPF();


	string msg("\nQuery " + result->getQuery() + "failed!\n");
	logCrit("ERROR %s\n",msg.c_str());
	m_Socket->doWrite((char *)msg.c_str(),msg.size());

	m_OutstandingQueries.pop_front();

	return true;
}

void X9Dialogue::sqlConnected()
{
	logPF();
	const char *est = "connection to server established\n";
	m_Socket->doWrite((char *)est,strlen(est));
}

void X9Dialogue::sqlDisconnected()
{
	logPF();
	const char *lost = "connection to server lost\n";
	m_Socket->doWrite((char *)lost,strlen(lost));

}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new X9(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

