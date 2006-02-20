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
#include <string.h>
#include <stdlib.h>

#include "Packet.hpp"

using namespace nepenthes;

Packet::Packet(char *data, uint32_t len)
{
	m_Data = (char *)malloc(len*sizeof(char));
	memcpy(m_Data,data,len);
	m_Length = len;
};

Packet::~Packet()
{
	free(m_Data);
}

char *Packet::getData()
{
	return m_Data;
}
uint32_t Packet::getLength()
{
	return m_Length;
}

bool Packet::cut(uint32_t offset)
{
	if(offset >= m_Length )
		return false;

	memmove(m_Data, m_Data + offset, m_Length - offset);
	m_Length = m_Length - offset;
	return true;
}



// some extensions we will need for bound udp sockets

UDPPacket::UDPPacket(uint32_t ip, uint16_t port, char *data,uint32_t len) : Packet(data,len)
{
	m_Host = ip;
	m_Port = port;
}

UDPPacket::~UDPPacket()
{
//	free(m_Data);
}


uint32_t UDPPacket::getHost()
{
	return m_Host;
}

uint16_t UDPPacket::getPort()
{
	return m_Port;
}
