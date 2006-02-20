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

#ifdef WIN32

#else
#include <poll.h>
#endif

#include <errno.h>
#include "DNSManager.hpp"
#include "DNSCallback.hpp"
#include "DNSResult.hpp"
#include "DNSQuery.hpp"
#include "DNSHandler.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

//extern int32_t errno;

/**
 * DNSManager constructor
 * 
 * @param nepenthes our nepenthes instance
 */
DNSManager::DNSManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
	m_DNSHandler = NULL;

}

/**
 * DNSManager destructor
 */
DNSManager::~DNSManager()
{
	logPF();
}

/**
 * checks if a DNSHandler is registerd
 * 
 * @return returns true if a DNSHandler is registerd,
 *         else false
 */
bool DNSManager::Init()
{
	if (m_DNSHandler == NULL)
	{
		logCrit("%s","NO DNSHandler loaded, hit the docs\n");
		g_Nepenthes->stop();
		return false;
	}else
	{
		return true;
	}
	
}

bool DNSManager::Exit()
{
	return true;
}

/**
 * lists the registerd DNSHandler
 */
void DNSManager::doList()
{

	logInfo("=--- %-69s ---=\n","DNSManager");
	if (m_DNSHandler != NULL)
	{
    	logInfo("  # %s\n",m_DNSHandler->getDNSHandlerName().c_str());
	}else
	{
		logCrit("%s","availible DNSHandler dnsresolve-adns\n");
	}
	logInfo("=--- %2s %-66s ---=\n","", "DNSHandler registerd");

	return;
}


/**
 * ask the DNSManager to resolve the provided domains A Record
 * 
 * @param callback the issuers DNSCallback
 * @param dns      the dns to resolve
 * @param obj      additional context data
 * 
 * @return true
 */
bool DNSManager::addDNS(DNSCallback *callback,char *dns,void *obj)
{
	logSpam("addDNS: Adding DNS %s for (%s)\n",dns,callback->getDNSCallbackName().c_str());


	// the resolver libs lack support for /etc/hosts
	// so we have to look the /etc/hosts up on our own
	// for now we just resolve localhost
	// FIXME parse /etc/hosts
	
	if ( strncasecmp(dns,"localhost",strlen("localhost")) == 0 )
	{
		logSpam("DNS is %s resolving to 127.0.0.1\n",dns);
		unsigned long ip = inet_addr("127.0.0.1");
		DNSResult result(ip,dns, (uint16_t)DNS_QUERY_A ,obj);
		callback->dnsResolved(&result);
		return true;
	} else
		if ( inet_addr(dns) != INADDR_NONE )
	{
		unsigned long ip = inet_addr(dns);
		logSpam("DNS is ip %s \n",dns);
		DNSResult result(ip,dns, (uint16_t)DNS_QUERY_A ,obj);
		callback->dnsResolved(&result);
		return true;
	}


	DNSQuery *query = new DNSQuery(callback,dns, DNS_QUERY_A, obj);
	return m_DNSHandler->resolveDNS(query);
}




/**
 * ask the DNSManager to resolve the provided domains TXT Record
 * 
 * @param callback the issuers DNSCallback
 * @param dns      the dns to resolve
 * @param obj      additional context data
 * 
 * @return true
 */

bool DNSManager::addTXT(DNSCallback *callback,char *dns, void *obj)
{
	logSpam("addTXT: Adding DNS %s for (%s)\n", dns,callback->getDNSCallbackName().c_str());
	DNSQuery *query = new DNSQuery(callback,dns, DNS_QUERY_TXT, obj);
	
	return m_DNSHandler->resolveTXT(query);
}

/**
 * register a DNSHandler
 * 
 * @param handler the handler to register
 * 
 * @return true if there was no DNSHandler registerd before
 *         else false
 */
bool DNSManager::registerDNSHandler(DNSHandler *handler)
{
	logPF();
	if (m_DNSHandler != NULL)
	{
		logCrit("Already DNSHandler %s registerd\n",m_DNSHandler->getDNSHandlerName().c_str());
		return false;
	}else
	{
		m_DNSHandler = handler;
	}
	return true;
}

/**
 * unregisters DNSHandler
 * 
 * @param handler
 * 
 * @return 
 */
bool DNSManager::unregisterDNSHandler(DNSHandler *handler)
{
	m_DNSHandler = NULL;
	return true;
}
