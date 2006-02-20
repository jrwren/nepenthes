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

#include "POLLSocket.hpp"

using namespace nepenthes;


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


POLLSocket::POLLSocket()
{
	m_Type = ST_POLL|ST_CONNECT|ST_NODEL;
	m_Status = SS_CONNECTED;
	m_CanSend = false;

	setLocalPort(0);
	setLocalHost(0);
	setRemoteHost(0);
	setRemotePort(0);

}

bool POLLSocket::bindPort()
{
	return false;
}

bool POLLSocket::Init()
{

	return true;
}

bool POLLSocket::Exit()
{
	return true;
}




bool POLLSocket::wantSend()
{
	return false;
}

int32_t POLLSocket::doSend()
{
	return 0;
}

int32_t POLLSocket::doRecv()
{
	return 0;
}

bool POLLSocket::checkTimeout()
{
	return false;
}

bool POLLSocket:: handleTimeout()
{
	return true;
}
		






bool POLLSocket::connectHost()
{
	return false;
}

Socket *POLLSocket::acceptConnection()
{
	return NULL;
}


int32_t POLLSocket::doWrite(char *msg, uint32_t len)
{
	return 0;
}

bool POLLSocket::doRespond(char *msg, uint32_t len)
{
	return false;
}

