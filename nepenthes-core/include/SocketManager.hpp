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

#ifndef HAVE_SOCKETMANAGER_HPP
#define HAVE_SOCKETMANAGER_HPP

#include <list>
#include <stdint.h>

#include "Manager.hpp"

using namespace std;

namespace nepenthes
{
	class Socket;
	class POLLSocket;
	class Nepenthes;
	class DialogueFactory;
	class Dialogue;

	/**
	 * the SocketManager keeps his Socket 's working.
	 * he cares about them like a mum, if they are dead, he removes them, if they establish, he polls them
	 * if you want a new connection, the SocketManager will set one up
	 */
	class SocketManager : public Manager
	{
	public:
		SocketManager(Nepenthes *pNepethes);
		virtual ~SocketManager();
		virtual Socket *bindTCPSocket(uint32_t localHost, uint16_t Port,time_t bindtimeout,time_t accepttimeout);
		virtual Socket *bindTCPSocket(uint32_t localHost, uint16_t Port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory);
		virtual Socket *bindTCPSocket(uint32_t localHost, uint16_t Port,time_t bindtimeout,time_t accepttimeout, char *dialoguefactoryname);

		virtual Socket *bindUDPSocket(uint32_t localhost, uint16_t port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory);

		virtual Socket *openFILESocket(char *filepath, int32_t flags);
		virtual Socket *connectUDPHost(uint32_t localHost, uint32_t remotehost, uint16_t remoteport,time_t connecttimeout);
		virtual Socket *connectTCPHost(uint32_t localHost, uint32_t remotehost, uint16_t remoteport,time_t connecttimeout);
		virtual Socket *connectTCPHost(uint32_t localHost, uint32_t remotehost, uint16_t localport, uint16_t remoteport,time_t connecttimeout);

		virtual Socket *createRAWSocketUDP(uint16_t localport, uint16_t remoteport,time_t bindtimeout,time_t accepttimeout, DialogueFactory *diaf);
		virtual Socket *createRAWSocketTCP(uint16_t localport, uint16_t remoteport,time_t bindtimeout,time_t accepttimeout, DialogueFactory *diaf);

		virtual Socket *addPOLLSocket(POLLSocket *sock);

		bool doLoop(uint32_t polltimeout);

		bool Init();
		bool Exit();
		void doList();

	private:
		list<Socket *> 	m_Sockets;
        bool 			m_UseRawSockets;
		uint32_t 		m_BindAddress;
	};

}

#endif

