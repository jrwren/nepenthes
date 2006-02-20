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
	class Packet;

	class TCPSocket : public Socket
	{
public:
	TCPSocket(Nepenthes *nepenthes, uint32_t localaddress, uint16_t port, time_t bindtimeout, time_t accepttimeout);	// bind socket 
	TCPSocket(Nepenthes *nepenthes, int32_t socket, uint32_t localhost, uint16_t localport, uint32_t  remotehost,uint16_t remoteport, time_t accepttimeout); // accept socket
	TCPSocket(Nepenthes *nepenthes,uint32_t localhost, uint32_t remotehost, uint16_t remoteport, time_t connectiontimeout);	// connect with timeout
	~TCPSocket();
		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int32_t doSend();
		int32_t doRecv();
		int32_t doWrite(char *msg, uint32_t len);
		bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, uint32_t len);

		void setStatus(socket_state i);
	protected:
		list <Packet *> m_TxPackets; 
	};
}
