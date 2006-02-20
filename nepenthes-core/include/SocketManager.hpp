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

#include "Manager.hpp"

using namespace std;

namespace nepenthes
{
	class Socket;
	class POLLSocket;
	class Nepenthes;
	class DialogueFactory;
	class Dialogue;

	class SocketManager : public Manager
	{
	public:
		SocketManager(Nepenthes *pNepethes);
		virtual ~SocketManager();
		virtual Socket *bindTCPSocket(unsigned long localHost, unsigned int Port,time_t bindtimeout,time_t accepttimeout);
		virtual Socket *bindTCPSocket(unsigned long localHost, unsigned int Port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory);
		virtual Socket *bindTCPSocket(unsigned long localHost, unsigned int Port,time_t bindtimeout,time_t accepttimeout, char *dialoguefactoryname);

		virtual Socket *bindUDPSocket(unsigned long localhost, unsigned int port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory);

		virtual Socket *openFILESocket(char *filepath, int flags);
		virtual Socket *connectUDPHost(unsigned long localHost, unsigned long remotehost, unsigned int Port,time_t connecttimeout);
		virtual Socket *connectTCPHost(unsigned long localHost, unsigned long remotehost, unsigned int Port,time_t connecttimeout);

		virtual Socket *createRAWSocketUDP(unsigned int localport, unsigned int remoteport,time_t bindtimeout,time_t accepttimeout, DialogueFactory *diaf);
		virtual Socket *createRAWSocketTCP(unsigned int localport, unsigned int remoteport,time_t bindtimeout,time_t accepttimeout, DialogueFactory *diaf);

		virtual Socket *addPOLLSocket(POLLSocket *sock);

		bool doLoop(unsigned int polltimeout);

		bool Init();
		bool Exit();
		void doList();

	private:
		list<Socket *> m_Sockets;
        bool m_UseRawSockets;
	};

}

#endif

