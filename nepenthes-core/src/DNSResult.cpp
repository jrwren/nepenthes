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

#include "DNSResult.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "DNSQuery.hpp"
#include "Utilities.hpp"

#include <cstring>

using namespace nepenthes;

/**
 * constructor for DNSResult
 * 
 * @param ip        the resolved ip
 * @param dns       the domain we resolved
 * @param querytype the querytype we resolved
 * @param obj       the additional data
 */
DNSResult::DNSResult(DNSQuery *query, uint32_t ip , char *dns, uint16_t querytype, void *obj)
{
	m_Query = query;
	m_ResolvedIPv4.push_back(ip);
	m_DNS = dns;
	m_Object = obj;
	m_QueryType = querytype;
}


#ifdef WIN32

#else
/**
 * constructor for DNSResult 
 * takes its information from an adns_result
 * 
 * @param answer    adns_answer struct
 * @param dns       the domain we resolved
 * @param querytype the querytype we used
 * @param obj       the additional data
 */
DNSResult::DNSResult(DNSQuery *query, adns_answer *answer, char *dns, uint16_t querytype, void *obj)
{
	int32_t i;
	if ( querytype & DNS_QUERY_A )
	{

		struct in_addr *mysi_addr = answer->rrs.inaddr;
		logSpam(" %i resolves \n", answer->nrrs);
		for ( i=0;  answer->nrrs > i; i++ )
		{
			logSpam("result '%i %s \n",i, inet_ntoa((struct in_addr) mysi_addr[i]));
			uint32_t ip;
			memcpy(&ip,mysi_addr+i,4);
			m_ResolvedIPv4.push_back(ip);
		}
	}else
    if (querytype & DNS_QUERY_TXT)
	{
		
		if ( answer->rrs.manyistr != 0 )
		{
			adns_rr_intstr *test = answer->rrs.manyistr[0];
			while ( test->i != -1 )
			{
				m_TXT.append(test->str,test->i);
//				g_Nepenthes->getUtilities()->hexdump((byte *)test->str,test->i);
				test++;
			}
		}
	}

	m_DNS = dns;
	m_Object = obj;
	m_Query = query;
	m_QueryType = querytype;
}
#endif

/**
 * destructor
 * deletes all internal data.
 */
DNSResult::~DNSResult()
{
	m_ResolvedIPv4.clear();
}

/**
 * returns the iplist of the resolved domain
 * 
 * @return list of ips in networkbyteorder
 */
list <uint32_t> DNSResult::getIP4List()
{
	return m_ResolvedIPv4;
}

/**
 * 
 * @return returns the domain we resolved
 */
string DNSResult::getDNS()
{
	return m_DNS;
}

/**
 * 
 * @return returns pointer to the additional data
 */
void *DNSResult::getObject()
{
	return m_Object;
}

DNSQuery *
DNSResult::getQuery ( void )
{
	return m_Query;
}

/**
 * 
 * @return returns the QueryType used
 */
uint16_t DNSResult::getQueryType()
{
	return m_QueryType;
}

/**
 * 
 * @return returns content of the domains TXT record
 */
string DNSResult::getTXT()
{
	return m_TXT;
}
