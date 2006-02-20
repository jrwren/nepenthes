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

#include "Manager.hpp"
#include "EventHandler.hpp"

namespace nepenthes
{
	class DNSHandler;

	class DNSManager : public Manager
	{
	public:
		DNSManager(Nepenthes *nepenthes);
		virtual ~DNSManager();

		virtual bool addDNS(DNSHandler *callback,char *dns, void *obj);
		void pollDNS();
		void callBack();
		uint32_t getSize();

		void doList();
		bool Init();
		bool Exit();

	protected:
		uint32_t m_Queue;
#ifdef WIN32

#else
		adns_state m_aDNSState;
#endif
	};

};
