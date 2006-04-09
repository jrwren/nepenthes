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

#include "UploadManager.hpp"
#include "UploadHandler.hpp"
#include "UploadQuery.hpp"

#include "DownloadUrl.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace std;
using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_mgr



/**
 * UploadManager constructor
 * 
 * @param nepenthes the nepenthes
 */
UploadManager::UploadManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}

/**
 * UploadManager destructor
 */
UploadManager::~UploadManager()
{

}

/**
 * gives the UploadQuery to the fitting UploadHandler
 * 
 * @param up     the Upload
 * 
 * @return returns true
 */
bool UploadManager::uploadUrl(UploadQuery *up)
{
	list <UploadHandlerTuple>::iterator handler;
	for(handler = m_UploadHandlers.begin(); handler != m_UploadHandlers.end(); handler++)
	{
		if(handler->m_Protocol == up->getUploadUrl()->getProtocol())
		{
			logInfo("Handler %s will upload %s \n",handler->m_Handler->getUploadHandlerName().c_str(),up->getUrl().c_str());
			handler->m_Handler->upload(up);
			return true;
		}
	}

	logCrit("No Handler for protocoll %s \n",up->getUploadUrl()->getProtocol().c_str());
	return true;
}  


/**
 * creates a UploadQuery
 * 
 * @param url      the url to upload to
 * @param payload  the payload to upload
 * @param size     the payloads size
 * @param callback the UploadCallback to call when the upload is done
 * @param obj      additional data
 * 
 * @return returns true
 */
bool UploadManager::uploadUrl(char *url, char *payload, uint32_t size, UploadCallback *callback, void *obj)
{
	UploadQuery *query = new UploadQuery(url,payload,size,callback,obj);
	return uploadUrl(query);
}

/**
 * register a UploadHandler
 * 
 * @param handler  the handler to register
 * @param protocol the protocoll the uploadhandler will server
 * 
 * @return returns true
 */
bool UploadManager::registerUploadHandler(UploadHandler *handler, const char * protocol)
{
	UploadHandlerTuple dht;
	dht.m_Handler = handler;
	dht.m_Protocol = protocol;
	m_UploadHandlers.push_back(dht);
	logDebug("Registerd %s as handler for protocol %-9s (%i protocols supported)\n",handler->getUploadHandlerName().c_str(),protocol, m_UploadHandlers.size());
	return true;
}

/**
 * unregister a UploadHandler
 * 
 * @param protocol the UploadHandlers protocoll to unregister
 */
void UploadManager::unregisterUploadHandler(const char *protocol)
{

}

/**
 * Init the UploadHandler
 * 
 * @return returns true if no error showed up, else false
 */
bool UploadManager::Init()
{
	return true;
}

/**
 * Exit the UploadManager
 * 
 * @return returns true
 */
bool UploadManager::Exit()
{
	return true;
}

/**
 * list the registerd UploadHandler 's
 */
void UploadManager::doList()
{
	list <UploadHandlerTuple>::iterator dhandler;
	logSpam("=--- %-69s ---=\n","UploadManager");
	uint32_t i=0;
	for(dhandler = m_UploadHandlers.begin();dhandler != m_UploadHandlers.end();dhandler++,i++)
	{
		logSpam("  %i) %5s %-8s %s\n",i,dhandler->m_Protocol.c_str() ,dhandler->m_Handler->getUploadHandlerName().c_str(), dhandler->m_Handler->getUploadHandlerDescription().c_str());
	}
    logSpam("=--- %2i %-66s ---=\n",i, "UploadHandlers registerd");
}
