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

#endif

#include "DNSCallback.hpp"
#include "Manager.hpp"
#include "EventHandler.hpp"

namespace nepenthes
{
	class DNSHandler;

	/**
	 * if you want to resolve a domains A record or TXT record, ask the DNSManager
	 */
	class DNSManager : public DNSCallback, public Manager
	{
	public:
		DNSManager(Nepenthes *nepenthes);
		virtual ~DNSManager();

		/**
		 * resolve a domains A record
		 * 
		 * @param callback the DNSCallback who needs the result
		 * @param dns      the dns to resolve
		 * @param obj      a context object you might need
		 * 
		 * @return 
		 */
		virtual bool addDNS(DNSCallback *callback,char *dns, void *obj);
		/**
		 * resolve a domains TXT record
		 * 
		 * @param callback the DNSCallback who needs the result
		 * @param dns      the dns to resolve
		 * @param obj      a context object you might need
		 * 
		 * @return 
		 */
		virtual bool addTXT(DNSCallback *callback,char *dns, void *obj);

		void doList();
		bool Init();
		bool Exit();

		virtual bool registerDNSHandler(DNSHandler *handler);
		virtual bool unregisterDNSHandler(DNSHandler *handler);

		bool dnsResolved	( DNSResult * );
		bool dnsFailure		( DNSResult * );

	protected:
		DNSHandler *m_DNSHandler;
	};

};
