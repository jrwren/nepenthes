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

#ifndef HAVE_DNSRESULT_HPP
#define HAVE_DNSRESULT_HPP

#ifdef WIN32

#else
#include <adns.h>
#include <arpa/inet.h>
#endif


#include <list>
#include <string>

using namespace std;

namespace nepenthes
{

	class DNSResult
	{
	public:

#ifdef WIN32

#else
		DNSResult(adns_answer *answer, char *dns, uint16_t querytype, void *obj);
#endif
		DNSResult(uint32_t ip , char *dns, uint16_t querytype, void *obj);

		virtual ~DNSResult();
		virtual list <uint32_t> getIP4List();
		virtual string getDNS();
		virtual void *getObject();
		virtual uint16_t getQueryType();
		virtual string getTXT();

	protected:
        list <uint32_t> m_ResolvedIPv4;
		string m_DNS;
		void 	*m_Object;
		uint16_t 	m_QueryType;
		string m_TXT;
	};
}

#endif
