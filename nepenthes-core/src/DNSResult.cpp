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

using namespace nepenthes;

DNSResult::DNSResult(uint32_t ip , char *dns, void *obj)
{
	m_ResolvedIPv4.push_back(ip);
	m_DNS = dns;
	m_Object = obj;
}


#ifdef WIN32

#else
DNSResult::DNSResult(adns_answer *answer, char *dns, void *obj)
{
	int32_t i;
	struct in_addr *mysi_addr = answer->rrs.inaddr;
	logSpam(" %i resolves \n", answer->nrrs);
	for ( i=0;  answer->nrrs > i; i++ )
	{
		logSpam("result '%i %s \n",i, inet_ntoa((struct in_addr) mysi_addr[i]));
		uint32_t ip;
		memcpy(&ip,mysi_addr+i,4);
		m_ResolvedIPv4.push_back(ip);
	}

	m_DNS = dns;
	m_Object = obj;
}
#endif

DNSResult::~DNSResult()
{
	m_ResolvedIPv4.clear();
}

list <uint32_t> DNSResult::getIP4List()
{
	return m_ResolvedIPv4;
}

string DNSResult::getDNS()
{
	return m_DNS;
}

void *DNSResult::getObject()
{
	return m_Object;
}
