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

#ifndef HAVE_MODULEHONEYTRAP_HPP
#define HAVE_MODULEHONEYTRAP_HPP


#include "config.h"

#include <map>

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"
#include "POLLSocket.hpp"
#include "EventHandler.hpp"



using namespace std;

namespace nepenthes
{

	class Buffer;

	typedef struct 
	{
		uint32_t	m_RemoteHost;
		uint16_t 	m_RemotePort;
		uint32_t	m_LocalHost;
		uint16_t	m_LocalPort;
	}connection_t;

	struct cmp_connection_t
	{
		bool operator()(connection_t s1, connection_t s2) const
		{
			if (s1.m_RemoteHost < s2.m_RemoteHost)
				return true;
			if (s1.m_RemotePort < s2.m_RemotePort)
				return true;
			if (s1.m_LocalHost < s2.m_LocalHost)
				return true;
			if (s1.m_LocalPort < s2.m_LocalPort)
				return true;

			return false;
		}
	};


	class ModuleHoneyTrap : public Module, public EventHandler
	{
	public:
		ModuleHoneyTrap(Nepenthes *);
		~ModuleHoneyTrap();
		bool Init();
		bool Exit();

		/* EventHandler */
		uint32_t handleEvent(Event *event);


		/* own */
		bool socketAdd(uint32_t remotehost, uint16_t remoteport, uint32_t localhost, uint16_t localport, Socket *s);
		bool socketDel(Socket *s);
        bool socketExists(uint32_t remotehost, uint16_t remoteport, uint32_t localhost, uint16_t localport);

	protected:
		map<connection_t ,Socket *,cmp_connection_t> m_Sockets;
		Nepenthes *m_Nepenthes;
//		honeytrap_type m_HTType;

		string m_DialogueFactory;
	};

}
extern nepenthes::Nepenthes *g_Nepenthes;
extern nepenthes::ModuleHoneyTrap *g_ModuleHoneytrap;

#endif
