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

#include "download-http.hpp"
#include "HTTPDialogue.hpp"

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

/**
 * The Constructor
 * creates a new HTTPDownloadHandler Module, 
 * HTTPDownloadHandler is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
HTTPDownloadHandler::HTTPDownloadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "download-http";
	m_ModuleDescription = "painless simple http client";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DownloadHandlerDescription = "simple http downloadhandler";
	m_DownloadHandlerName  = "http download handler";

	g_Nepenthes = nepenthes;
}

HTTPDownloadHandler::~HTTPDownloadHandler()
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
bool HTTPDownloadHandler::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_DOWNLOAD_HANDLER(this,"http");
	return true;
}

bool HTTPDownloadHandler::Exit()
{
	return true;
}



bool HTTPDownloadHandler::download(Download *down)
{
	logPF();

	logInfo("Resolving host %s ... \n", down->getUrl().c_str());
    g_Nepenthes->getDNSMgr()->addDNS(this,(char *)down->getDownloadUrl()->getHost().c_str(), down);
	return true;
}



bool HTTPDownloadHandler::dnsResolved(DNSResult *result)
{
	logInfo("url %s resolved \n",result->getDNS().c_str());
	uint32_t host = result->getIP4List().front();
	Download *down = (Download *) result->getObject();
	Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(down->getLocalHost(),host,down->getDownloadUrl()->getPort(),30);
	HTTPDialogue *dia = new HTTPDialogue(socket,down);
	socket->addDialogue(dia);
	return true;
}

bool HTTPDownloadHandler::dnsFailure(DNSResult *result)
{
	logWarn("url %s unresolved \n",result->getDNS().c_str());
	return true;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new HTTPDownloadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

