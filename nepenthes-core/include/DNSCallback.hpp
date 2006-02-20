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

#ifndef HAVE_DNSCALLBACK_HPP
#define HAVE_DNSCALLBACK_HPP

#include <string>

using namespace std;

namespace nepenthes
{
	class DNSResult;

	/**
	 * as resolving domains takes some time, and we got no time to wait,
	 * we setup the DNSQuery and assign a DNSCallback
	 * When the query is done, the DNSHandler will call the DNSCallback and provide the result.
	 */
	class DNSCallback
	{
		public:
		virtual ~DNSCallback(){};
		/**
		 * resolving the domain was a success
		 * 
		 * @param result the DNSResult
		 * 
		 * @return 
		 */
        virtual bool dnsResolved(DNSResult *result)=0;
		/**
		 * resolving the domain failed
		 * 
		 * @param result
		 * 
		 * @return 
		 */
		virtual bool dnsFailure(DNSResult *result)=0;

		/**
		 * maybe we will need the dnscallbacks name for debugging.
		 * 
		 * @return returns the dnscallbacks name
		 */
		virtual string getDNSCallbackName()
		{
			return m_DNSCallbackName;
		}

	protected:
		string m_DNSCallbackName;
	};
}

#endif
