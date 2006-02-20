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

#include <stdlib.h>
#include <string.h>

#include "Message.hpp"
using namespace nepenthes;


Message::Message(char *msg, unsigned int len, unsigned int localport, unsigned int remoteport, 
				 unsigned long localhost, unsigned long remotehost, Responder *responder, Socket *socket)
{
	if((int)len > 0 && msg != NULL )
	{
		// malloc 1 byte more, 
		// and se it 0
		// so str* fns wont complain
		m_Msg = (char *) malloc(len*sizeof(char)+1);
		memset(m_Msg,0,len+1);
		memcpy(m_Msg,msg,len);
		m_MsgLen = len;
	}else
	{
		m_Msg = NULL;
		m_MsgLen = 0;
	}
	m_LocalPort = localport;
	m_RemotePort = remoteport;
	m_LocalHost = localhost;
	m_RemoteHost = remotehost;
	m_Reply = responder;
	m_Socket = socket;


}

Message::Message(unsigned int localport, unsigned int remoteport, unsigned long localhost, 
				 unsigned long remotehost, Responder *responder, Socket *socket)
{
	m_Msg = NULL;
	m_MsgLen = 0;
	
	m_LocalPort = localport;
	m_RemotePort = remoteport;
	m_LocalHost = localhost;
	m_RemoteHost = remotehost;
	m_Reply = responder;
	m_Socket = socket;
}

Message::~Message()
{
	if(m_Msg != NULL)
	{
		free(m_Msg);
	}
}

char *Message::getMsg()
{
	return m_Msg;
}

unsigned int Message::getMsgLen()
{
	return m_MsgLen;
}

unsigned long Message::getLocalHost()
{
	return m_LocalHost;
}

unsigned int Message::getLocalPort()
{
	return m_LocalPort;
}

unsigned long Message::getRemoteHost()
{
	return m_RemoteHost;
}

unsigned int Message::getRemotePort()
{
	return m_RemotePort;
}

time_t Message::getReceiveTime()
{
	return m_ReceiveTime;
}

Socket *Message::getSocket()
{
	return m_Socket;
}

Responder *Message::getResponder()
{
	return m_Reply;
}
