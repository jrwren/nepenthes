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
#include <adns.h>
#endif


#include <string>


#include "DNSHandler.hpp"


namespace nepenthes
{

	class DNSQuery
	{
	public:
		DNSQuery(DNSHandler *handler, char *dns, void *obj);
		~DNSQuery();

		DNSHandler *getHandler();
		string getDNS();
#ifdef WIN32

#else
		adns_query *getADNS();
#endif 
		void *getObject();
	protected:
		DNSHandler  *m_Handler;
		void 		*m_Object;

		string      m_DNS;

#ifdef WIN32

#else
		adns_query  m_ADNS_QUERY;
#endif

	};

}
