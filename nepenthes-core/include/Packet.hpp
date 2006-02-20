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

#ifndef HAVE_PACKET_HPP
#define HAVE_PACKET_HPP

#include <stdint.h>

namespace nepenthes
{

	/**
	 * Packet
	 * as we are using nonblocking io, and cant rely on sending all data in a single chunk, we have 'packets'
	 * if you send something, it will get put in a Packet.
	 * if the socket is ready to send, he will send your packet,
	 * if he cant send the whole packet, he will shrink it by the size he already sended.
	 */
	class Packet
	{
	public:
		Packet(char *pszData,uint32_t iLen);
		~Packet();
		char *getData();
		uint32_t getSize();
		bool cut(uint32_t offset);

	protected:
		char    *m_Data;
		uint32_t m_Length;
	};

	/**
	 * UDPPackets even store the destinations ip address and port, so we can run them nonblocking too.
	 */
	class UDPPacket : public Packet
	{
	public:
		UDPPacket(uint32_t ip, uint16_t port, char *pszData,uint32_t iLen);
		~UDPPacket();
		uint32_t getHost();
		uint16_t getPort();
	protected:
		uint32_t m_Host;
		uint16_t m_Port;
	};


}

#endif
