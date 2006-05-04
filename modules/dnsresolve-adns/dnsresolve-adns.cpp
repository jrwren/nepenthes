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

#include <sys/poll.h>
#include <errno.h>


#include "dnsresolve-adns.hpp"

#include "DNSManager.hpp"

#include "DNSQuery.hpp"


#include "DNSResult.hpp"
#include "DNSResult.cpp"


#include "LogManager.hpp"
#include "EventHandler.cpp"


#include "EventManager.hpp"

using namespace std;

using namespace nepenthes;

Nepenthes *g_Nepenthes;

DNSResolverADNS::DNSResolverADNS(Nepenthes *nepenthes)
{
	m_ModuleName        = "dnsresolve-adns";
	m_ModuleDescription = "resolve dns async";
	m_ModuleRevision    = "$rev$";

	m_DNSHandlerName = "DNSResolverADNS";
	m_Queue = 0;

	m_EventHandlerName = "DNSResolverADNS";
	m_EventHandlerDescription = "poll adns sockets, and resolve the queries";
	m_Timeout = 0;

	g_Nepenthes = nepenthes;
}

DNSResolverADNS::~DNSResolverADNS()
{
	
}

bool DNSResolverADNS::Init()
{
	int32_t r;
	r =adns_init(&m_aDNSState, adns_if_noautosys, 0);
	if ( m_aDNSState == NULL )
	{
		logCrit("Error opening /etc/resolv.conf: %s; r = %d", strerror(errno), r);
		return false;
	}

	g_Nepenthes->getDNSMgr()->registerDNSHandler(this);
	REG_EVENT_HANDLER(this);
	logDebug("adns_init() Success\n");

	return true;
}
bool DNSResolverADNS::Exit()
{
	adns_finish(m_aDNSState);
	return true;
}

bool DNSResolverADNS::resolveDNS(DNSQuery *query)
{
	logPF();
	if (m_Queue == 0)
	{
		m_Events.set(EV_TIMEOUT);
	}

	ADNSContext *ctx = new ADNSContext;
	ctx->m_DNSQuery = query;

	adns_submit (m_aDNSState, query->getDNS().c_str(), adns_r_a, adns_qf_owner, ctx, &ctx->m_ADNS);
//	logSpam("addDNS: query %8x for %s\n", query,callback->getDNSCallbackName().c_str());
	m_Queue++;
	return true;
}

bool DNSResolverADNS::resolveTXT(DNSQuery *query)
{
	logPF();
	if (m_Queue == 0)
	{
		m_Events.set(EV_TIMEOUT);
	}

	ADNSContext *ctx = new ADNSContext;
	ctx->m_DNSQuery = query;

	adns_submit(m_aDNSState, query->getDNS().c_str(), adns_r_txt, adns_qf_owner, ctx, &ctx->m_ADNS);
//	logSpam("addTXT: query %8x for %s\n", query,callback->getDNSCallbackName().c_str());
	m_Queue++;
	return true;
}


uint32_t DNSResolverADNS::handleEvent(Event *event)
{
	logPF();
	if (event->getType() != EV_TIMEOUT)
	{
		return 0;
	}

	struct pollfd pfd[100];
	int32_t nfds = 100;
	int32_t timeout = 0;
	memset(pfd,0,100*sizeof(struct pollfd));

	struct timeval currenttime;
	struct timezone tz; 
	memset(&tz,0,sizeof(struct timezone));
	gettimeofday(&currenttime,&tz);

	adns_beforepoll(m_aDNSState, pfd, (int *)&nfds, (int *)&timeout, &currenttime);
	poll(pfd, nfds, timeout);
	adns_afterpoll(m_aDNSState, pfd, nfds, &currenttime);
	adns_processany(m_aDNSState);
	callBack();
	return 0;
}

void DNSResolverADNS::callBack()
{
	adns_query q, r;
	void *vr;
	
	adns_answer *answer;
	DNSQuery *query;
	
	ADNSContext *ctx;
	void *vctx;

	logSpam("%i DNS Resolves in Queue\n", m_Queue);

	adns_forallqueries_begin(m_aDNSState);
	while ( (q = adns_forallqueries_next(m_aDNSState, (void **)&vr)) != NULL )
	{
	        r = (adns_query)vr;
	        
	        int adns_ret = adns_check(m_aDNSState, &q, &answer, (void **)&vctx);
	        ctx = (ADNSContext *)vctx;
	        
		switch ( adns_ret )
		{
		case 0:
			{
				m_Queue--;
				query = ctx->m_DNSQuery;
				logDebug("resolved dns %s (%i left) \n", query->getDNS().c_str(),m_Queue);
				DNSResult result(answer,(char *)query->getDNS().c_str(), query->getQueryType(),query->getObject());

				if (answer->nrrs == 0)
				{
					query->getCallback()->dnsFailure(&result);
				}else
				{
                    query->getCallback()->dnsResolved(&result);
				}
				delete ctx;
				delete query;
				free(answer);
			}
			break;
		case EAGAIN:
			/* Go into the queue again */
			break;
		default:
			m_Queue--;
            logWarn("resolving %s failed (%i left) \n", answer->cname, m_Queue);
			query = ctx->m_DNSQuery;
			delete query;
			delete ctx;
			free(answer);
			break;
		}
	}

	if (m_Queue == 0)
	{
		m_Events.reset(EV_TIMEOUT);
	}
	return;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new DNSResolverADNS(nepenthes);
        return 1;
    } else {
        return 0;
    }
}


