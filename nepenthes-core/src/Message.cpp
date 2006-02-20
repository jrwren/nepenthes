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


/**
 * Message constructor
 * 
 * @param msg        the data
 * @param len        the datas size
 * @param localport  the local port used
 * @param remoteport the remote port used
 * @param localhost  the local address used
 * @param remotehost the remote address used
 * @param responder  the responder to respond to the message	
 * @param socket     the socket used to receive the message
 */
Message::Message(char *msg, uint32_t len, uint32_t localport, uint32_t remoteport, 
				 uint32_t localhost, uint32_t remotehost, Responder *responder, Socket *socket)
{
	if((int32_t)len > 0 && msg != NULL )
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


/**
 * Message constructor for timeout messages
 * 
 * @param localport  the local port used
 * @param remoteport the remote port used
 * @param localhost  the local address used
 * @param remotehost the remote address used
 * @param responder  the responder to respond to the message	
 * @param socket     the socket used to receive the message
 */
Message::Message(uint32_t localport, uint32_t remoteport, uint32_t localhost, 
				 uint32_t remotehost, Responder *responder, Socket *socket)
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


/**
 * Message destructor
 */
Message::~Message()
{
	if(m_Msg != NULL)
	{
		free(m_Msg);
	}
}

/**
 * get the data
 * 
 * @return returns pointer to the data
 */
char *Message::getMsg()
{
	return m_Msg;
}

/**
 * get the datas size
 * 
 * @return returns datas size
 */
uint32_t Message::getSize()
{
	return m_MsgLen;
}

/**
 * get local address
 * 
 * @return returns local address
 */
uint32_t Message::getLocalHost()
{
	return m_LocalHost;
}

/**
 * get local port
 * 
 * @return returns local port
 */
uint32_t Message::getLocalPort()
{
	return m_LocalPort;
}

/**
 * get remote host
 * 
 * @return returns remote address
 */
uint32_t Message::getRemoteHost()
{
	return m_RemoteHost;
}

/**
 * get remote port
 * 
 * @return returns remote port
 */
uint32_t Message::getRemotePort()
{
	return m_RemotePort;
}

/**
 * get receive time
 * 
 * @return returns receive time
 */
time_t Message::getReceiveTime()
{
	return m_ReceiveTime;
}

/**
 * get the socket 
 * 
 * @return returns the socket
 */
Socket *Message::getSocket()
{
	return m_Socket;
}

/**
 * get the responder
 * 
 * @return returns the responder
 */
Responder *Message::getResponder()
{
	return m_Reply;
}
