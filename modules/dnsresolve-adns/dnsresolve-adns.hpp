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

#include <adns.h>

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "Nepenthes.hpp"
#include "Socket.hpp"

#include "DNSHandler.hpp"
#include "EventHandler.hpp"
#include "Event.hpp"

using namespace std;

namespace nepenthes
{
	struct ADNSContext 
	{
		adns_query 	m_ADNS;
		DNSQuery	*m_DNSQuery;
	};

    class DNSResolverADNS : public Module , DNSHandler , EventHandler
	{
	public:
		DNSResolverADNS(Nepenthes *);
		~DNSResolverADNS();

		bool Init();
		bool Exit();

		bool resolveDNS(DNSQuery *query);
		bool resolveTXT(DNSQuery *query);

		uint32_t handleEvent(Event *event);

		void callBack();

	protected:
		adns_state m_aDNSState;
		uint32_t m_Queue;
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;
