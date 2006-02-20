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

#include "download-ftp.hpp"
#include "CTRLDialogue.hpp"
#include "FILEDialogue.hpp"
#include "FTPContext.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"

#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"

#include "DownloadManager.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"

#include "DNSManager.hpp"
#include "DNSResult.hpp"

#include "Config.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_hlr

using namespace nepenthes;

/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;
FTPDownloadHandler *g_FTPDownloadHandler;

/**
 * The Constructor
 * creates a new FTPDownloadHandler Module, 
 * FTPDownloadHandler is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
FTPDownloadHandler::FTPDownloadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "download-ftp";
	m_ModuleDescription = "painless simple activex-2l ftp client";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "download-ftp";
	m_DialogueFactoryDescription = "download-ftp dialogue factory";

	m_DownloadHandlerDescription = "simple ftp downloadhandler";
	m_DownloadHandlerName  = "ftp download handler";

	g_Nepenthes = nepenthes;
	g_FTPDownloadHandler = this;
	m_DNSCallbackName = "download-ftp dns callback";

	m_DynDNS = "";

	m_RetrAddress = 0;
}

FTPDownloadHandler::~FTPDownloadHandler()
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
bool FTPDownloadHandler::Init()
{
	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	StringList sList;
	try
	{
		if (m_Config->getValInt("download-ftp.use_nat") == 1)
		{
        		sList = *m_Config->getValStringList("download-ftp.nat_settings.forwarded_ports");
				if ( sList.size() == 2 )
				{
					m_MinPort = atoi(sList[0]);
					m_MaxPort = atoi(sList[1]);
				}

				m_DynDNS = m_Config->getValString("download-ftp.nat_settings.dyndns");

				logInfo("download-ftp nat settings; uses %s for external ip and ports %i->%i for transferr\n",
						m_DynDNS.c_str(),
						m_MinPort, 
						m_MaxPort);
		}
	} catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_DOWNLOAD_HANDLER(this,"ftp");
	return true;
}

bool FTPDownloadHandler::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *x-2)
 * 
 * creates a new FTPDownloadHandlerDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *FTPDownloadHandler::createDialogue(Socket *socket)
{
	logPF();
	logDebug("Incoming connection on port %i\n",socket->getLocalPort());

	list<FTPContext *>::iterator it;
	for (it= m_Contexts.begin();it != m_Contexts.end();it++)
	{
		logSpam("Ports  %i <-> %i \n",(*it)->getActiveFTPBindPort(), socket->getLocalPort());
		if ((*it)->getActiveFTPBindPort() == socket->getLocalPort())
		{

			Dialogue *dia = new FILEDialogue(socket,(*it)->getDownload(), (*it)->getCTRLDialogue());
			FTPContext *delme = *it;
			m_Contexts.erase(it);
			delete delme;
			return dia;

		}
	}
//	return new FTPDownloadHandlerDialogue(socket);
	return NULL;
}




bool FTPDownloadHandler::download(Download *down)
{
	logPF();

	if ( m_DynDNS == "" )
	{
		uint32_t host = inet_addr(down->getDownloadUrl()->getHost().c_str());

		if ( (int32_t)host == -1 )
		{
			logInfo("url %s has a dns as hostname, we have to resolve it \n", down->getUrl().c_str());
			g_Nepenthes->getDNSMgr()->addDNS(this,(char *)down->getDownloadUrl()->getHost().c_str(), down);
			return true;
		} else
		{
			logInfo("url has %s ip, we will download it now\n",down->getUrl().c_str());
			Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(down->getLocalHost(),host,down->getDownloadUrl()->getPort(),30);
			CTRLDialogue *dia = new CTRLDialogue(socket,down);
			socket->addDialogue(dia);
			FTPContext *context = new FTPContext(down,dia);
			dia->setContext(context);
			m_Contexts.push_back(context);
		}
	} else
	{
		logSpam("Resolving DynDNS %s for active ftp\n",m_DynDNS.c_str());
		g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_DynDNS.c_str(),down);
	}
	return true;
}



bool FTPDownloadHandler::dnsResolved(DNSResult *result)
{
	logInfo("url %s resolved \n",result->getDNS().c_str());
	uint32_t host = result->getIP4List().front();
	Download *down = (Download *) result->getObject();
	if ( result->getDNS() != m_DynDNS )
	{ // resolved domain for a download
		Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(down->getLocalHost(),host,down->getDownloadUrl()->getPort(),30);
		CTRLDialogue *dia = new CTRLDialogue(socket,down);
		socket->addDialogue(dia);
		FTPContext *context = new FTPContext(down,dia);
		dia->setContext(context);
		m_Contexts.push_back(context);
	}else
	{ // resolved dyndns
		m_RetrAddress = host;
		// resolve domain for download
		uint32_t host = inet_addr(down->getDownloadUrl()->getHost().c_str());
		if ( (int32_t)host == -1 )
		{
			logInfo("url %s has a dns as hostname, we have to resolve it \n", down->getUrl().c_str());
			g_Nepenthes->getDNSMgr()->addDNS(this,(char *)down->getDownloadUrl()->getHost().c_str(), down);
			return true;
		} else
		{
			logInfo("url has %s ip, we will download it now\n",down->getUrl().c_str());
			Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(down->getLocalHost(),host,down->getDownloadUrl()->getPort(),30);
			CTRLDialogue *dia = new CTRLDialogue(socket,down);
			socket->addDialogue(dia);
			FTPContext *context = new FTPContext(down,dia);
			dia->setContext(context);
			m_Contexts.push_back(context);
		}
	}
	return true;
}

bool FTPDownloadHandler::dnsFailure(DNSResult *result)
{
	Download *down = (Download *) result->getObject();
	logWarn("url %s unresolved, dropping download %s \n",result->getDNS().c_str(),down->getUrl().c_str());

	
	delete down;

	return true;
}

bool FTPDownloadHandler::removeContext(FTPContext *context)
{
	logPF();
	list<FTPContext *>::iterator it;
	for (it= m_Contexts.begin();it != m_Contexts.end();it++)
	{
		if ((*it) == context)
		{
			FTPContext *delme = *it;
			m_Contexts.erase(it);
			delete delme;
			return true;
		}
	}

	return false;
}

uint16_t FTPDownloadHandler::getMinPort()
{
	return m_MinPort;
}

uint16_t FTPDownloadHandler::getMaxPort()
{
	return m_MaxPort;
}

uint32_t FTPDownloadHandler::getRetrAddress()
{
	return m_RetrAddress;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new FTPDownloadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
