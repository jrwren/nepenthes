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

#include "dnsresolve-udns.hpp"

#include "DNSManager.hpp"

#include "DNSQuery.hpp"


#include "DNSResult.hpp"
#include "DNSResult.cpp"


#include "LogManager.hpp"
#include "EventHandler.cpp"


#include "EventManager.hpp"
#include "SocketManager.hpp"

#include "Socket.cpp"
#include "POLLSocket.cpp"

using namespace std;

using namespace nepenthes;

Nepenthes *g_Nepenthes;



DNSResolverUDNS::DNSResolverUDNS(Nepenthes *nepenthes)
{
	g_Nepenthes = nepenthes;
}  

DNSResolverUDNS::~DNSResolverUDNS()
{

}  

bool DNSResolverUDNS::Init()
{
	/* 
	 * create default resolver context
	 * dont open it
	 */
	dns_init(0);

	/* copy default dns context */
	m_DNSctx = dns_new(NULL);

	dns_open(m_DNSctx);

	logInfo("Socket is %i \n",dns_sock(m_DNSctx));

	g_Nepenthes->getSocketMgr()->addPOLLSocket(this);
	g_Nepenthes->getDNSMgr()->registerDNSHandler(this);
	setSocket(dns_sock(m_DNSctx));
	return true;
}  

bool DNSResolverUDNS::Exit()
{
	return true;
}  


/* POLLSocket */
bool DNSResolverUDNS::wantSend()
{
	return false;
}  

int32_t DNSResolverUDNS::doSend()
{
	return 0;
}  

int32_t DNSResolverUDNS::doRecv()
{
	logPF();
	dns_ioevent(m_DNSctx,0);
	return 0;
}  

bool DNSResolverUDNS::checkTimeout()
{
	/* process any pending timeouts and return number of secounds from current time (now if it is not 0) 
	* to the time when the library wants the application to pass it control to process more queued requests. 
	* In case when there are no requests pending, this time is -1. The routine will not request a time larger than maxwait secounds 
	* if it is greather or equal to zero. If now is 0, the routine will obtain current time by it\u2019s own; when it is not 0, 
	* it should contain current time as returned by time().
	*/
	dns_timeouts(m_DNSctx,0,0);
	return false;
}  

bool DNSResolverUDNS::handleTimeout()
{
	return false;
}  

int32_t DNSResolverUDNS::getSocket()
{
   return dns_sock(m_DNSctx);
}  


/* DNSHandler */
bool DNSResolverUDNS::resolveDNS(DNSQuery *query)
{
	logPF();
	// struct dns_query * dns_submit_dn(ctx,const unsigned char *dn, qcls, qtyp, flags,parse, cbck, data)
	struct dns_query *q;
/*	unsigned char dn[DNS_MAXDN];
	int abs;

	dns_ptodn(query->getDNS().c_str(), query->getDNS().size(), dn, sizeof(dn), &abs);

	logInfo("dn is %s %i\n",dn,abs);

	q = dns_submit_dn(m_DNSctx,
				  dn,
				  DNS_C_IN,
				  DNS_T_A,
				  abs,
				  0, 
				  DNSResolverUDNS::dnscb, 
				  query);
*/
	q = dns_submit_a4(m_DNSctx,query->getDNS().c_str(),0,dnscbA,query);

	if (q == NULL)
	{
		logCrit("dnsresolve-udns %s\n",dns_strerror(dns_status(m_DNSctx)));
	}
	logInfo("dnsresolve-udns %i queries in queue\n",dns_active(m_DNSctx));
	return true;
}  

bool DNSResolverUDNS::resolveTXT(DNSQuery *query)
{
	return true;
}  

void DNSResolverUDNS::dnscbA(struct dns_ctx *ctx, struct dns_rr_a4 *result, void *data)
{
	logPF();
	DNSQuery *q = (DNSQuery *)data;
	for (int i=0;i< result->dnsa4_nrr;i++)
	{
		logInfo("domain %s ip %s\n",inet_ntoa(result->dnsa4_addr[i]),q->getDNS().c_str());
	}
}

void DNSResolverUDNS::dnscb(struct dns_ctx *ctx, void *result, void *data)
{
	logPF();
	int r = dns_status(ctx);
	DNSQuery *q = (DNSQuery *)data;
	struct dns_parse p;
	struct dns_rr rr;
	unsigned nrr;
	unsigned char dn[DNS_MAXDN];
	const unsigned char *pkt, *cur, *end, *qdn;

	if ( !result )
	{
		logCrit("Could not resolve %s\n",q->getDNS().c_str());
		return;
	}

	pkt = (const unsigned char *)result; 
	end = pkt + r; 
	cur = dns_payload(pkt);
	dns_getdn(pkt, &cur, end, dn, sizeof(dn));
	dns_initparse(&p, NULL, pkt, cur, end);
//	p.dnsp_qcls = 0;
//	p.dnsp_qtyp = 0;
	qdn = dn;
	nrr = 0;

	p.dnsp_qcls = DNS_C_INVALID;
	p.dnsp_qtyp = DNS_T_INVALID;


	while ( (r = dns_nextrr(&p, &rr)) > 0 )
	{
//		logSpam("%s:%i\n",__FILE__,__LINE__);

		const unsigned char *dptr = rr.dnsrr_dptr;
		switch (rr.dnsrr_typ)
		{
		case DNS_T_CNAME:
			{
				string sa,sb;

				dns_getdn(pkt, &rr.dnsrr_dptr, end,p.dnsp_dnbuf, sizeof(p.dnsp_dnbuf));

				sa = dns_dntosp(rr.dnsrr_dn);
				sb = dns_dntosp(p.dnsp_dnbuf);
				logInfo("%s %x CNAME %s %x \n",sa.c_str(),qdn,sb.c_str(),p.dnsp_dnbuf);
			}
			break;

		case DNS_T_A:
			logInfo(" domain %s %s %s %d.%d.%d.%d\n",dns_dntosp(rr.dnsrr_dn),dns_typename(rr.dnsrr_typ),dns_classname(rr.dnsrr_cls),
					dptr[0], dptr[1], dptr[2], dptr[3]);
			break;

		default:
			logInfo("No case yet %s %s\n",dns_typename(rr.dnsrr_typ),dns_classname(rr.dnsrr_cls));
		}

		if ( !dns_dnequal(qdn, rr.dnsrr_dn) ) 
			continue;
		

		if ( rr.dnsrr_cls == DNS_C_IN && ( 
										  ( q->getQueryType() == DNS_QUERY_A && rr.dnsrr_typ == DNS_T_A ) ||
										  ( q->getQueryType() == DNS_QUERY_TXT && rr.dnsrr_typ == DNS_T_TXT ) ) )
		{
        	++nrr;
		}
		else if ( rr.dnsrr_typ == DNS_T_CNAME && !nrr )
		{
			qdn = p.dnsp_dnbuf;
		}

	}

//	logSpam("%s:%i\n",__FILE__,__LINE__);
	if ( !r && !nrr )
		r = DNS_E_NODATA;


	logSpam("%s:%i\n",__FILE__,__LINE__);

	if ( r < 0 )
	{
		logCrit("Could not resolve %s\n",q->getDNS().c_str());
		free(result);
		logSpam("%s:%i\n",__FILE__,__LINE__);
		return;
	}

	dns_rewind(&p, NULL);
	p.dnsp_qtyp = DNS_T_A;
	p.dnsp_qcls = DNS_C_IN;
/*	while ( dns_nextrr(&p, &rr) )
	{
		logSpam("%s:%i\n",__FILE__,__LINE__);
		const unsigned char *dptr = rr.dnsrr_dptr;
		switch ( rr.dnsrr_typ )
		{
		case DNS_T_A:
			if ( rr.dnsrr_dsz == 4 )
				logInfo(" dns %s has ip %d.%d.%d.%d \n", q->getDNS().c_str(), dptr[0], dptr[1], dptr[2], dptr[3]);
			break;

		case DNS_T_CNAME:
		case DNS_T_PTR:
		case DNS_T_NS:
		case DNS_T_MB:
		case DNS_T_MD:
		case DNS_T_MF:
		case DNS_T_MG:
		case DNS_T_MR:
//		if ( dns_getdn(pkt, &dptr, end, dn, DNS_MAXDN) <= 0 ) 
			logInfo("%s.", dns_dntosp(dn));
			break;


		default:
			logSpam("%s:%i\n",__FILE__,__LINE__);
			logWarn("Unrequested dns result %i %s\n",rr.dnsrr_typ, dns_typename(rr.dnsrr_typ));
		}
	}
	logSpam("%s:%i\n",__FILE__,__LINE__);
*/	
	free(result);
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new DNSResolverUDNS(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

