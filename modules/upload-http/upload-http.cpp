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

#include "upload-http.hpp"
#include "HTTPUPDialogue.hpp"

#include "LogManager.hpp"

#include "UploadManager.hpp"
#include "UploadQuery.hpp"
#include "UploadResult.hpp"
#include "UploadCallback.hpp"

#include "DownloadUrl.hpp"
#include "DNSManager.hpp"
#include "DNSResult.hpp"
#include "SocketManager.hpp"
#include "Socket.hpp"


using namespace nepenthes;

/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;



/**
 * the HTTPUploadHandler constructor creates a new HTTPUploadHandler Module
 * 
 * sets ModuleName 
 * sets ModuleDescription
 * sets ModuleRevision
 * 
 * this modules does nothing but load
 * 
 * @param nepenthes the Nepenthes
 */
HTTPUploadHandler::HTTPUploadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "upload-http";
	m_ModuleDescription = "upload files via http POST";
	m_ModuleRevision    = "$Rev$";

	m_UploadHandlerDescription = "upload files via http post";

	m_Nepenthes = nepenthes;
	g_Nepenthes = nepenthes;
}

HTTPUploadHandler::~HTTPUploadHandler()
{

}

/**
 * Module::Init()
 * 
 * as this Modules does nothing, nothing is done here
 * 
 * @return true
 */
bool HTTPUploadHandler::Init()
{
	logPF();
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_UPLOAD_HANDLER(this,"http");
	return true;
}

/**
 * does nothing but return true
 * 
 * @return true
 */
bool HTTPUploadHandler::Exit()
{
	return true;
}



bool HTTPUploadHandler::upload(UploadQuery *up)
{
	g_Nepenthes->getDNSMgr()->addDNS(this,(char *)up->getUploadUrl()->getHost().c_str(),up);
	return true;
}

bool HTTPUploadHandler::dnsResolved(DNSResult *result)
{
	logDebug("url %s resolved %i for %x\n",result->getDNS().c_str(), result->getIP4List().size(), (uint32_t) result->getObject());

	list <uint32_t> resolved = result->getIP4List();
	uint32_t host = resolved.front();
	UploadQuery *query = (UploadQuery *)result->getObject();

	Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,host,query->getUploadUrl()->getPort(),30);
	socket->addDialogue(new HTTPUPDialogue(socket,query));
	return true;
}

bool HTTPUploadHandler::dnsFailure(DNSResult *result)
{
	UploadQuery *query;
	if ( (query = (UploadQuery *)result->getObject()) != NULL)
	{
		if (query->getCallback() != NULL)
		{
			UploadResult *up = new UploadResult(NULL, 0, query->getObject());
			query->getCallback()->uploadFailure(up);
			delete up;
		}
		delete query;
	}
	return true;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new HTTPUploadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
