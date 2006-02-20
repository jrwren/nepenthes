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
#include "DNSHandler.hpp"
#include "DNSResult.hpp"
#include "DNSQuery.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

extern int32_t errno;

DNSManager::DNSManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
	m_Queue = 0;
}

DNSManager::~DNSManager()
{

}

bool DNSManager::Init()
{
#ifdef WIN32
	return true;
#else
	int32_t r;
	r =adns_init(&m_aDNSState, adns_if_noautosys, 0);
	if ( m_aDNSState == NULL )
	{
		logCrit("Error opening /etc/resolv.conf: %s; r = %d", strerror(errno), r);
		return false;
	}

	logDebug("%s","adns_init() Success\n");
	return true;
#endif
	
}

bool DNSManager::Exit()
{
	return true;
}

void DNSManager::doList()
{
	return;
}


bool DNSManager::addDNS(DNSHandler *callback,char *dns, void *obj)
{
#ifdef WIN32

	return true;
#else
	logSpam("addDNS: Adding DNS %s for (%s)\n",dns,callback->getDNSHandlerName().c_str());

	if (strncasecmp(dns,"localhost",strlen("localhost")) == 0)
	{
		logSpam("DNS is %s resolving to 127.0.0.1\n",dns);
		uint32_t ip = inet_addr("127.0.0.1");
		DNSResult result(ip,dns, obj);
		callback->dnsResolved(&result);
		return true;
	}else
	if (  inet_addr(dns) != INADDR_NONE )
	{
		uint32_t ip = inet_addr(dns);
		logSpam("DNS is ip %s \n",dns);
		DNSResult result(ip,dns, obj);
		callback->dnsResolved(&result);
		return true;
	}


	DNSQuery *query = new DNSQuery(callback,dns, obj);

//	adns_query *newadns = (adns_query *) malloc(sizeof (adns_query));
    adns_submit (m_aDNSState, dns, adns_r_a, adns_qf_owner, query, query->getADNS());

	logSpam("addDNS: query %8x for %s\n", query,callback->getDNSHandlerName().c_str());

	m_Queue++;

	return true;

#endif
}

void DNSManager::pollDNS()
{
#ifdef WIN32
	return;
#else
	if (m_Queue == 0)
		return;


	struct pollfd pfd[100];
    int32_t nfds = 100;
    int32_t timeout = 0;
	memset(pfd,0,100*sizeof(struct pollfd));

	struct timeval currenttime;
	struct timezone tz; 
	memset(&tz,0,sizeof(struct timezone));
	gettimeofday(&currenttime,&tz);

    adns_beforepoll(m_aDNSState, pfd, &nfds, &timeout, &currenttime);
    poll(pfd, nfds, timeout);
    adns_afterpoll(m_aDNSState, pfd, nfds, &currenttime);
    adns_processany(m_aDNSState);
	callBack();
	return;
#endif
}

void DNSManager::callBack()
{
#ifdef WIN32

#else
	adns_query q, r;
	adns_answer *answer;
	DNSQuery *query;

	logSpam("%i DNS Resolves in Queue\n", m_Queue);

	adns_forallqueries_begin(m_aDNSState);
	while ( (q = adns_forallqueries_next(m_aDNSState, (void **)&r)) != NULL )
	{

		switch ( adns_check(m_aDNSState, &q, &answer, (void **)&query) )
		{
		case 0:
			{
				m_Queue--;
				logDebug("resolved dns %s (%i left) \n", query->getDNS().c_str(),m_Queue);
				DNSResult result(answer,(char *)query->getDNS().c_str(), query->getObject());
				if (answer->nrrs == 0)
				{
					query->getHandler()->dnsFailure(&result);
				}else
				{
                    query->getHandler()->dnsResolved(&result);
				}
			}
			break;
		case EAGAIN:
			/* Go into the queue again */
			break;
		default:
			m_Queue--;
			logWarn("resolving %s failed (%i left) \n", answer->cname, m_Queue);
			break;
		}
	}
	return;
#endif
}

uint32_t DNSManager::getSize()
{
	return m_Queue;
}


