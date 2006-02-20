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
		UDPSocket(Nepenthes *nepenthes,uint32_t localhost, uint32_t remotehost, int32_t remoteport, time_t connectiontimeout);
		UDPSocket(Nepenthes *nepenthes, uint32_t localhost, int32_t port, time_t bindtimeout, time_t accepttimeout);
		~UDPSocket();

		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int32_t doSend();
		int32_t doRecv();
		int32_t doWrite(char *msg, uint32_t len);
		int32_t doWriteTo(uint32_t ip, uint16_t port, char *msg, uint32_t len);

        bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, uint32_t len);
	private:
		list <UDPPacket *> m_TxPackets;
	};
}
