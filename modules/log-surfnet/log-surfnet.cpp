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

#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log-surfnet.hpp"


#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SocketEvent.hpp"
#include "SubmitEvent.hpp"
#include "Socket.hpp"

#include "EventHandler.cpp"

#include "ShellcodeHandler.hpp"

#include "Config.hpp"

#include "SQLHandler.hpp"
#include "SQLResult.hpp"
#include "SQLManager.hpp"

#include <cstdlib>

using namespace nepenthes;

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_ev | l_hlr




LSDetail::LSDetail(uint32_t host, int type, string data)
{
	
	m_host = inet_ntoa(*(in_addr *)&host);
	m_type = type;
	m_data = data;
}


LSContext::LSContext()
{
	m_attackID = 0;
	m_closed = false;

	m_severity = -1;
}





/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;
uint16_t myevent;

/**
 * Constructor
 * creates a new LogSurfNET Module, where x% is public Module, public EventHandler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the EventHandlerName
 * - sets the EventHandlerDescription
 * - sets the EventHandlers Timeout
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
LogSurfNET::LogSurfNET(Nepenthes *nepenthes)
{
	m_ModuleName        = "log-surfnet";
	m_ModuleDescription = "log various malicious events to postgresql";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = "LogSurfNETEventHandler";
	m_EventHandlerDescription = "hook malicious events and log them to mysql";

	g_Nepenthes = nepenthes;

	m_RunningMode = LS_MODE_LIST;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
LogSurfNET::~LogSurfNET()
{

}



/**
 * bool Module::Init()
 * setup Module specific values 
 * here:
 * - register als EventHandler
 * - set wanted events
 * 
 * @return returns true if everything was fine, else false
 *         returning false will showup errors in warning a module
 */
bool LogSurfNET::Init()
{

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	StringList sList;

	string server;
	string user;
	string pass;
	string db;
	string options;
	string mode;
	try
	{
		sList = *m_Config->getValStringList("log-surfnet.ports");
		server  = m_Config->getValString("log-surfnet.server");
		user    = m_Config->getValString("log-surfnet.user");
		pass    = m_Config->getValString("log-surfnet.pass");
		db      = m_Config->getValString("log-surfnet.db");
		options = m_Config->getValString("log-surfnet.options");
		mode 	= m_Config->getValString("log-surfnet.mode");
	}
	catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	m_Ports = (uint16_t *)malloc(sizeof(uint16_t)*sList.size());
	m_MaxPorts = sList.size();

	try
	{
		mode 	= m_Config->getValString("log-surfnet.mode");
		if (mode == "list")
		{
			m_RunningMode = LS_MODE_LIST;
		}
		else
		if ( mode == "any" )
		{
			m_RunningMode = LS_MODE_ANY;
		}

	}
	catch ( ... )
	{
		logWarn("No 'mode' value found in config, using 'any'\n");
	}


	if ( m_RunningMode == LS_MODE_LIST )
	{
		uint32_t i = 0;
		while ( i < sList.size() )
		{
			m_Ports[i] = (uint16_t)atoi(sList[i]);
			i++;
		}
	}

	switch (m_RunningMode)
	{
	case nepenthes::LS_MODE_ANY:
		logInfo("Running mode is any port\n");
		break;

	case nepenthes::LS_MODE_LIST:
		logInfo("Running mode is port list\n");
		break;

	}

	m_SQLHandler = g_Nepenthes->getSQLMgr()->createSQLHandler("postgres",server,user,pass,db,options,this);


	if ( m_SQLHandler == NULL )
	{
		logCrit("Could not create sqlhandler for the postgres database connection\n");
		return false;
	}


	m_ModuleManager = m_Nepenthes->getModuleMgr();
	m_Events.set(EV_SOCK_TCP_ACCEPT);
	m_Events.set(EV_SOCK_TCP_CLOSE);
	m_Events.set(EV_DIALOGUE_ASSIGN_AND_DONE);
	m_Events.set(EV_SHELLCODE_DONE);

	m_Events.set(EV_DOWNLOAD);
	m_Events.set(EV_SUBMISSION);

	REG_EVENT_HANDLER(this);

	return true;
}


/**
 * unregister as EventHandler
 * 
 * @return returns true if everything was fine
 */
bool LogSurfNET::Exit()
{
	return true;
}


/**
 * the handleEvent method is called whenever an event occurs 
 * the EventHandler wanted to have.
 * 
 * @param event  the Event
 * 
 * @return return 0
 */
uint32_t LogSurfNET::handleEvent(Event *event)
{
	logPF();
	logInfo("Event %i\n",event->getType());

	Socket *socket=NULL;
	Dialogue *dia=NULL;
	ShellcodeHandler *handler=NULL;
	uint32_t localhost=0;
	uint32_t remotehost=0;
	string url="";
	string md5sum = "";


	switch ( event->getType() )
	{
	case EV_SOCK_TCP_ACCEPT:
		socket = ((SocketEvent *)event)->getSocket();
		break;

	case EV_SOCK_TCP_CLOSE:
		socket = ((SocketEvent *)event)->getSocket();
		break;

	case EV_DIALOGUE_ASSIGN_AND_DONE:
		socket = ((DialogueEvent *)event)->getSocket();
		dia = ((DialogueEvent *)event)->getDialogue();
		break;


	case EV_SHELLCODE_DONE:
		socket = ((ShellcodeEvent *)event)->getSocket();
		handler = ((ShellcodeEvent *)event)->getShellcodeHandler();
		break;

	case EV_DOWNLOAD:
		localhost = ((SubmitEvent *)event)->getDownload()->getLocalHost();
		remotehost = ((SubmitEvent *)event)->getDownload()->getRemoteHost();
		url = ((SubmitEvent *)event)->getDownload()->getUrl();
		break;

	case EV_SUBMISSION:
		localhost = ((SubmitEvent *)event)->getDownload()->getLocalHost();
		remotehost = ((SubmitEvent *)event)->getDownload()->getRemoteHost();
		url = ((SubmitEvent *)event)->getDownload()->getUrl();
		md5sum = ((SubmitEvent *)event)->getDownload()->getMD5Sum();
		break;


	default:
		logWarn("this should not happen\n");
	}


	bool process=false;
	map <uint32_t, LSContext, ltint>::iterator attackit;
	uint32_t attackid=0;

	switch ( event->getType() )
	{
	case EV_SOCK_TCP_ACCEPT:
		{
			if (m_RunningMode == LS_MODE_ANY)
			{
				process = true;
			}
			else
			{
				uint16_t localport = socket->getLocalPort();
				uint16_t i=0;
				while ( i < m_MaxPorts )
				{
					if ( m_Ports[i] == localport )
					{
						process=true;
					}
					i++;
				}
			}
		}
		break;

	case EV_SOCK_TCP_CLOSE:
	case EV_DIALOGUE_ASSIGN_AND_DONE:
	case EV_SHELLCODE_DONE:
		{
			if ( m_SocketTracker.count((uintptr_t) socket) == 0 )
			{
				logCrit("Could not find attackid for %x\n",(uintptr_t) socket);
				 process=false;
			}
			else
			{
				process=true;
				attackit = m_SocketTracker.find((uintptr_t) socket);
				attackid = attackit->second.m_attackID;
			}
		}
		break;

	case EV_DOWNLOAD:
	case EV_SUBMISSION:
		process = true;
		break;

	default:
		logWarn("this should not happen\n");
	}


	if ( process == true )
	{
		switch ( event->getType() )
		{
		case EV_SOCK_TCP_ACCEPT:
			handleTCPAccept(socket);
			break;

		case EV_SOCK_TCP_CLOSE:
			handleTCPclose(socket,attackid);
			break;

		case EV_DIALOGUE_ASSIGN_AND_DONE:
			handleDialogueAssignAndDone(socket,dia,attackid);
			break;

		case EV_SHELLCODE_DONE:
			handleShellcodeDone(socket,handler,attackid);
			break;

		case EV_DOWNLOAD:
			handleDownloadOffer(localhost,remotehost,url.c_str());
			break;

		case EV_SUBMISSION:
			handleDownloadSuccess(localhost,remotehost,url.c_str(), md5sum.c_str());
			break;


		default:
			logWarn("this should not happen\n");
		}
	}else
		logInfo("not processed\n");

	return 0;
}

string  itos( long i )
{
	std::ostringstream s;
	s << i;
	return s.str();
}


void LogSurfNET::handleTCPAccept(Socket *socket)
{
	logPF();
	logSpam("handleTCPAccept()\n"
			"\tSocket 0x%x\n",
			(uint32_t) ((intptr_t)socket));

	string hwa = "";
	socket->getRemoteHWA(&hwa);

	uint32_t attackerip = socket->getRemoteHost();
	uint32_t decoyip = socket->getLocalHost();

	string attackerhost = inet_ntoa(*(in_addr *)&attackerip);
	string decoyhost = inet_ntoa(*(in_addr *)&decoyip);


	string query;
	query = "SELECT surfnet_attack_add('";
	query += itos(AS_POSSIBLE_MALICIOUS_CONNECTION);
	query += "','";
	query += attackerhost;
	query += "','";
	query += itos(socket->getRemotePort());
	query += "','";
	query += decoyhost;
	query += "','";
	query += itos(socket->getLocalPort());
	if (hwa != "")
	{
    	query += "','";
		query += hwa;
		query += "','";
	}else
	{
		query += "',NULL,'";
	}
	query += decoyhost;
	query += "');";

	m_SQLHandler->addQuery(&query,this,socket);
	m_SocketTracker[(uintptr_t) socket].m_attackID = 0;
}

void LogSurfNET::handleTCPclose(Socket *socket, uint32_t attackid)
{
	logPF();
	logSpam("handleTCPclose()\n"
			"\tSocket 0x%x\n"
			"\tattackID %i\n",
			(uint32_t) ((intptr_t)socket), 
			attackid);

	if (m_SocketTracker[(uintptr_t) socket].m_Details.size() > 0)
	{
    	m_SocketTracker[(uintptr_t) socket].m_closed = true;
	}else
	{
		m_SocketTracker.erase((uintptr_t)socket);
	}
}

void LogSurfNET::handleDialogueAssignAndDone(Socket *socket, Dialogue *dia, uint32_t attackid)
{
	logPF();
	logSpam("handleDialogueAssignAndDone()\n"
			"\tSocket 0x%x\n"
			"\tDialogue %s\n"
			"\tattackID %i\n",
			(uint32_t) ((uintptr_t)socket), 
			dia->getDialogueName().c_str(), 
			attackid);

	if ( attackid > 0 )
	{

		uint32_t decoyip = socket->getLocalHost();
		string decoyhost = inet_ntoa(*(in_addr *)&decoyip);

		string query;
		query = "SELECT surfnet_detail_add('";
		query += itos(attackid);
		query += "','";
		query += decoyhost;
		query += "','";
		query += itos(DT_DIALOGUE_NAME);
		query += "','";
		query += dia->getDialogueName();
		query += "');";

		m_SQLHandler->addQuery(&query,NULL,NULL);


		query = "SELECT surfnet_attack_update_severity('";
		query += itos(attackid);
		query += "','";
		query += itos(AS_DEFINITLY_MALICIOUS_CONNECTION);
		query += "');";

		m_SQLHandler->addQuery(&query,NULL,NULL);
	}
	else
	{
		LSDetail *d = new LSDetail(socket->getLocalHost(),DT_DIALOGUE_NAME,dia->getDialogueName());
		m_SocketTracker[(uintptr_t) socket].m_Details.push_back(d);
		m_SocketTracker[(uintptr_t) socket].m_severity = AS_DEFINITLY_MALICIOUS_CONNECTION;
	}

}


void LogSurfNET::handleShellcodeDone(Socket *socket, ShellcodeHandler *handler, uint32_t attackid)
{
	logSpam("handleShellcodeDone()\n"
			"\tSocket 0x%x\n"
			"\tShellcodeHandler %s\n"
			"\tattackID %i\n",
			(uint32_t) ((uintptr_t)socket), 
			handler->getShellcodeHandlerName().c_str(), 
			attackid);

	if ( attackid > 0 )
	{
		uint32_t decoyip = socket->getLocalHost();
		string decoyhost = inet_ntoa(*(in_addr *)&decoyip);

		string query;
		query = "SELECT surfnet_detail_add('";
		query += itos(attackid);
		query += "','";
		query += decoyhost;
		query += "','";
		query += itos(DT_SHELLCODEHANDLER_NAME);
		query += "','";
		query += handler->getShellcodeHandlerName();
		query += "');";

		m_SQLHandler->addQuery(&query,NULL,NULL);
	}else
	{
		LSDetail *d = new LSDetail(socket->getLocalHost(),DT_SHELLCODEHANDLER_NAME,handler->getShellcodeHandlerName());
		m_SocketTracker[(uintptr_t) socket].m_Details.push_back(d);
	}
}



void LogSurfNET::handleDownloadOffer(uint32_t localhost, uint32_t remotehost,const char *url)
{
	logPF();
	string hwa = "";

	string attackerhost = inet_ntoa(*(in_addr *)&remotehost);
	string decoyhost = inet_ntoa(*(in_addr *)&localhost);

	string surl = url;

	string query;
	query = "SELECT surfnet_detail_add_offer('";
	query += attackerhost;
	query += "','";
	query += decoyhost;
	query += "','";
	query += m_SQLHandler->escapeString(&surl);
	query += "');";

	m_SQLHandler->addQuery(&query,NULL,NULL);

}

void LogSurfNET::handleDownloadSuccess(uint32_t localhost, uint32_t remotehost, const char *url, const char *md5hash)
{
	logPF();

	string attackerhost = inet_ntoa(*(in_addr *)&remotehost);
	string decoyhost = inet_ntoa(*(in_addr *)&localhost);

	string surl = url;
	string smd5hash = md5hash;

	string query;
	query = "SELECT surfnet_detail_add_download('";
	query += attackerhost;
	query += "','";
	query += decoyhost;
	query += "','";
	query += m_SQLHandler->escapeString(&surl);
	query += "','";
	query += m_SQLHandler->escapeString(&smd5hash);
	query += "');";

	m_SQLHandler->addQuery(&query,NULL,NULL);
}


bool LogSurfNET::sqlSuccess(SQLResult *result)
{
	logPF();
	Socket *s;
	vector< map<string,string> > resvec = *result->getResult();
	s = (Socket *)result->getObject();

	logCrit("Socket %x  has cookie %s \n",(uintptr_t)s,
			resvec[0]["surfnet_attack_add"].c_str());
	m_SocketTracker[(uintptr_t)s].m_attackID = atoi(resvec[0]["surfnet_attack_add"].c_str());


	if (m_SocketTracker[(uintptr_t)s].m_Details.size() > 0)
	{
		logDebug("Processing Event Backlog for this connection\n");
	}

	while (m_SocketTracker[(uintptr_t)s].m_Details.size() > 0)
	{
/*		logSpam("WOOOOHOOOOO %s %s %i \n",
				m_SocketTracker[(uintptr_t)s].m_Details.front()->m_host.c_str(),
				m_SocketTracker[(uintptr_t)s].m_Details.front()->m_data.c_str(),
				m_SocketTracker[(uintptr_t)s].m_Details.front()->m_type);
*/

		string query;
		query = "SELECT surfnet_detail_add('";
		query += itos(m_SocketTracker[(uintptr_t)s].m_attackID);
		query += "','";
		query += m_SocketTracker[(uintptr_t)s].m_Details.front()->m_host;
		query += "','";
		query += itos(m_SocketTracker[(uintptr_t)s].m_Details.front()->m_type);
		query += "','";
		query += m_SocketTracker[(uintptr_t)s].m_Details.front()->m_data.c_str();
		query += "');";

		m_SQLHandler->addQuery(&query,NULL,NULL);

		delete m_SocketTracker[(uintptr_t)s].m_Details.front();
		m_SocketTracker[(uintptr_t)s].m_Details.pop_front();
	}

	if (m_SocketTracker[(uintptr_t)s].m_severity != -1)
	{
		string query;

		query = "SELECT surfnet_attack_update_severity('";
		query += itos(m_SocketTracker[(uintptr_t)s].m_attackID);
		query += "','";
		query += itos(m_SocketTracker[(uintptr_t)s].m_severity);
		query += "');";

		m_SQLHandler->addQuery(&query,NULL,NULL);
	}

	if (m_SocketTracker[(uintptr_t)s].m_closed == true)
	{
		m_SocketTracker.erase((uintptr_t)s);
	}

	return true;
}

bool LogSurfNET::sqlFailure(SQLResult *result)
{
	logPF();

	Socket *s;
	s = (Socket *)result->getObject();
	logCrit("Getting attackid for socket %x failed, dropping the whole attack, forgetting all details\n",(uintptr_t)s);
	m_SocketTracker.erase((uintptr_t)s);
	return true;
}

void LogSurfNET::sqlConnected()
{
	logPF();
}

void LogSurfNET::sqlDisconnected()
{
	logPF();
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new LogSurfNET(nepenthes);
		return 1;
	}
	else
	{
		return 0;
	}
}

