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

#ifndef HAVE_SUBMITXMLRPC_HPP
#define HAVE_SUBMITXMLRPC_HPP

#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "SubmitHandler.hpp"

#include "DNSHandler.hpp"

#include "DNSResult.hpp"



#include "EventHandler.hpp"
#include "Utilities.hpp"

using namespace std;

namespace nepenthes
{
	class SubmitXMLRPC : public Module , public SubmitHandler, public DNSHandler , public EventHandler
	{
	public:
		SubmitXMLRPC(Nepenthes *);
		~SubmitXMLRPC();
		bool Init();
		bool Exit();

		void Submit(Download *down);
		void Hit(Download *down);
		
		bool dnsResolved(DNSResult *result);
		bool dnsFailure(DNSResult *result);

		uint32_t handleEvent(Event *event);

		string getXMLRPCHost();
		string getXMLRPCPath();

	protected:
        string m_XMLRPCServerAddress;
		string m_XMLRPCServerPath;
		uint16_t m_XMLRPCServerPort;
		bool m_HTTPPipeline;
		
	};
}
extern nepenthes::Nepenthes *g_Nepenthes;
extern nepenthes::SubmitXMLRPC *g_SubmitXMLRPC;


namespace nepenthes
{
}



#endif
