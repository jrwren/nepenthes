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


#include "Socket.hpp"
#include "Responder.hpp"

namespace nepenthes
{
	class UDPPacket;

	class UDPSocket : public Socket
	{
public:
		UDPSocket(Nepenthes *nepenthes,unsigned long localhost, unsigned long remotehost, int remoteport, time_t connectiontimeout);
		UDPSocket(Nepenthes *nepenthes, unsigned long localhost, int port, time_t bindtimeout, time_t accepttimeout);
		~UDPSocket();

		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int doSend();
		int doRecv();
		int doWrite(char *msg, unsigned int len);
		int doWriteTo(unsigned long ip, unsigned short port, char *msg, unsigned int len);

        bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, unsigned int len);
	private:
		list <UDPPacket *> m_TxPackets;
	};
}
