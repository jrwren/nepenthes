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

#include "log-surfnet.hpp"
#include "DatabaseConnection.hpp"

#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SocketEvent.hpp"
#include "SubmitEvent.hpp"
#include "Socket.hpp"

#include "EventHandler.cpp"

#include "ShellcodeHandler.hpp"

#include "Config.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_ev | l_hlr


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
	m_ModuleDescription = "log various malicious events to mysql";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = "LogSurfNETEventHandler";
	m_EventHandlerDescription = "hook malicious events and log them to mysql";

	g_Nepenthes = nepenthes;
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
		logCrit("%s","I need a config\n");
		return false;
	}

	StringList sList;

	string server;
	string user;
	string pass;
	string db;
	try
	{
		sList = *m_Config->getValStringList("log-surfnet.ports");
		server = m_Config->getValString("log-surfnet.server");
		user = m_Config->getValString("log-surfnet.user");
		pass = m_Config->getValString("log-surfnet.pass");
		db = m_Config->getValString("log-surfnet.db");
	} catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	m_Ports = (uint16_t *)malloc(sizeof(uint16_t)*sList.size());
	m_MaxPorts = sList.size();

	uint32_t i = 0;
	while (i < sList.size())
	{
		m_Ports[i] = (uint16_t)atoi(sList[i]);
		i++;
	}


	m_DB = new DatabaseConnection(server.c_str(),user.c_str(),pass.c_str(),db.c_str());

	if ( m_DB->Init() == false )
	{
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

	
	switch(event->getType())
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
		logWarn("%s","this should not happen\n");
	}


	bool process=false;
	map <uint32_t, uint32_t, ltint>::iterator attackit;
	uint32_t attackid=0;

	switch(event->getType())
	{
	case EV_SOCK_TCP_ACCEPT:
		{
			uint16_t localport = socket->getLocalPort();
			uint16_t i=0;
			while (i < m_MaxPorts)
			{
				if (m_Ports[i] == localport)
				{
					process=true;
				}
				i++;
			}
		}
		break;

	case EV_SOCK_TCP_CLOSE:
	case EV_DIALOGUE_ASSIGN_AND_DONE:
	case EV_SHELLCODE_DONE:
		{
			if (m_SocketTracker.count((uint32_t) socket) == 0)
			{
				process=false;
			}else
			{
				process=true;
				attackit = m_SocketTracker.find((uint32_t) socket);
				attackid = attackit->second;
			}
		}
		break;

	case EV_DOWNLOAD:
	case EV_SUBMISSION:
		process = true;
		break;

	default:
		logWarn("%s","this should not happen\n");
	}


	if (process == true)
	{
		switch(event->getType())
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
			logWarn("%s","this should not happen\n");
		}
	}

	return 0;
}


void LogSurfNET::handleTCPAccept(Socket *socket)
{
	logCrit("handleTCPAccept()\n"
			"\tSocket 0x%x\n",
			(uint32_t) socket);

    int32_t sensorid = m_DB->getSensorID(socket->getLocalHost());
	int32_t attackid = m_DB->addAttack(AS_POSSIBLE_MALICIOUS_CONNECTION, socket->getRemoteHost(), socket->getRemotePort(), socket->getLocalHost(), socket->getLocalPort(),sensorid);

	m_SocketTracker[(uint32_t)socket] = attackid;
}

void LogSurfNET::handleTCPclose(Socket *socket, uint32_t attackid)
{
	logCrit("handleTCPclose()\n"
			"\tSocket 0x%x\n"
			"\tattackID %i\n",
			(uint32_t) socket, 
			attackid);

	m_SocketTracker.erase((uint32_t) socket);
}

void LogSurfNET::handleDialogueAssignAndDone(Socket *socket, Dialogue *dia, uint32_t attackid)
{
	logCrit("handleDialogueAssignAndDone()\n"
			"\tSocket 0x%x\n"
			"\tDialogue %s\n"
			"\tattackID %i\n",
			(uint32_t) socket, 
			dia->getDialogueName().c_str(), 
			attackid);

	int32_t sensorid = m_DB->getSensorID(socket->getLocalHost());

	m_DB->addDetail(attackid, sensorid, DT_DIALOGUE_NAME, dia->getDialogueName().c_str());
	m_DB->updateAttackSeverity(attackid,AS_DEFINITLY_MALICIOUS_CONNECTION);


}


void LogSurfNET::handleShellcodeDone(Socket *socket, ShellcodeHandler *handler, uint32_t attackid)
{
	logCrit("handleShellcodeDone()\n"
			"\tSocket 0x%x\n"
			"\tShellcodeHandler %s\n"
			"\tattackID %i\n",
			(uint32_t) socket, 
			handler->getShellcodeHandlerName().c_str(), 
			attackid);

//	m_DB->addDetail(int32_t attackid, char *text);
	int32_t sensorid = m_DB->getSensorID(socket->getLocalHost());
	m_DB->addDetail(attackid, sensorid, DT_SHELLCODEHANDLER_NAME ,handler->getShellcodeHandlerName().c_str());
}



void LogSurfNET::handleDownloadOffer(uint32_t localhost, uint32_t remotehost,const char *url)
{
	int32_t sensorid = m_DB->getSensorID(localhost);
	int32_t attackid = m_DB->addAttack(AS_DOWNLOAD_OFFER, remotehost, 0, localhost, 0,sensorid);
    m_DB->addDetail(attackid, sensorid, DT_DOWNLOAD_URL, url);
}

void LogSurfNET::handleDownloadSuccess(uint32_t localhost, uint32_t remotehost, const char *url, const char *md5hash)
{
	int32_t sensorid = m_DB->getSensorID(localhost);
	int32_t attackid = m_DB->addAttack(AS_DOWNLOAD_SUCCESS, remotehost, 0, localhost, 0,sensorid);

    m_DB->addDetail(attackid, sensorid, DT_DOWNLOAD_URL, url);
	m_DB->addDetail(attackid, sensorid, DT_DOWNLOAD_HASH, md5hash);
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new LogSurfNET(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
