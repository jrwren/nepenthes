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

#ifndef HAVE_DNSQUERY_HPP
#define HAVE_DNSQUERY_HPP


#include <stdint.h>
#include <string>

#include "DNSCallback.hpp"

#define DNS_QUERY_A		0x0001
#define DNS_QUERY_TXT	0x0002


namespace nepenthes
{


	/**
	 * if we need to resolve a domains A or TXT record, the DNSManager will create a encapsulating 
	 * class of the provided information
	 * the DNSQuery class
	 */
	class DNSQuery
	{
	public:
		DNSQuery(DNSCallback *manager, DNSCallback *handler, char *dns, uint16_t querytype, void *obj);
		virtual ~DNSQuery();

		virtual DNSCallback *getCallback();
		virtual DNSCallback *getCallbackUser();
		virtual void cancelCallback();
		virtual string getDNS();
		virtual uint16_t getQueryType();
		virtual void *getObject();

	protected:
		DNSCallback  *m_Callback;
		DNSCallback  *m_CallbackUser;
		void 		*m_Object;

		string      m_DNS;
		uint16_t 	m_QueryType;
	};

}

#endif
