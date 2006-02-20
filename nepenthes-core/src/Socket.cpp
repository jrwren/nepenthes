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

#include <string>
#include <sstream>

#ifdef WIN32
#include <time.h>
#else
#include <arpa/inet.h>

#endif

#include "Socket.hpp"
#include "DialogueFactory.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_net


using namespace std;
using namespace nepenthes;


bool Socket::addDialogueFactory(DialogueFactory *diaf)
{
	list <DialogueFactory *>::iterator diaff;
	bool known=false;
	for(diaff = m_DialogueFactories.begin();diaff != m_DialogueFactories.end(); diaff++)
	{
		if (diaf == (*diaff))
		{
			known = true;
		}
	}
	if (known == false)
	{
    	logInfo("%s \n\tAdding DialogueFactory %s \n",getDescription().c_str(),diaf->getFactoryName().c_str());
		m_DialogueFactories.push_back(diaf);
	}else
	{
		logInfo("%s \tAdding DialogueFactory: already known\n",getDescription().c_str(),diaf->getFactoryName().c_str());
	}
	return true;
}


bool Socket::addDialogue(Dialogue *dia)
{
	m_Dialogues.push_back(dia);
	return true;
}

/**
 * get the sockets status
 * 
 * @return returns the sockets status
 */
int   Socket::getStatus()
{
	return m_Status;
}


/**
 * set the sockets status
 * 
 * @param i      the new socket status
 */
void  Socket::setStatus(socket_state i)
{
	m_Status = i;
	return;
}


/**
 * set the socket polled
 */
void  Socket::setPolled()
{
	m_Polled = true;
	return;
}


/**
 * set the socket not polled
 */
void  Socket::unsetPolled()
{
	m_Polled = false;
	return;
}


/**
 * check if the socket is polled
 * 
 * @return returns true if the socket is polled
 *         else false
 */
bool  Socket::isPolled()
{

	return m_Polled;
}


/**
 * run getsockopt on the socket
 * 
 * @param level
 * @param optname
 * @param optval
 * @param optlen
 * 
 * @return the getsockopt returnvalue
 */
int   Socket::getsockOpt(int level, int optname,void *optval,socklen_t *optlen)
{
#ifdef WIN32
	return getsockopt(m_Socket, level, optname, (char *)optval,optlen);
#else
	return getsockopt(m_Socket, level, optname, optval, optlen);
#endif
}


/**
 * get the socket
 * 
 * @return the socket
 */
int   Socket::getSocket()
{
	return m_Socket;
}


/**
 * set the socket
 * 
 * @param i      the new socket
 */
void  Socket::setSocket(int i)
{
	m_Socket = i;
	return;
}


/**
 * get the sockets type
 * 
 * @return returns the sockets type
 */
int   Socket::getType()
{
	return m_Type;
}


/**
 * get the sockets local port
 * 
 * @return returns the sockets local port
 */
int   Socket::getLocalPort()
{
	return m_LocalPort;
}


/**
 * get the sockets remote port
 * 
 * @return returns the sockets remote port
 */
int  Socket::getRemotePort()
{

	return m_RemotePort;
}


/**
 * set the sockets localport
 * 
 * @param i      the new local port as int in host byte order
 */
void  Socket::setLocalPort(int i)
{
	m_LocalPort = i;
	return;
}


/**
 * set the sockets remoteport
 * 
 * @param i      the new remote port as int in host byte order
 */
void  Socket::setRemotePort(int i)
{
	m_RemotePort = i;
	return;
}


/**
 * set the sockets remotehost
 * 
 * @param i      the new remote host as unsigned long in network byte order
 */
void Socket::setRemoteHost(unsigned long i)
{
	m_RemoteHost = i;
	return;
}


/**
 * set the sockets localhost
 * 
 * @param i      the new localhost as unsigned long in network byte order
 */
void Socket::setLocalHost(unsigned long i)
{
	m_LocalHost = i;
	return;
}


/**
 * get the sockets local host
 * 
 * @return returns the sockets localhost as unsigned long
 */
unsigned long Socket::getLocalHost()
{
	return m_LocalHost;
}


/**
 * get the sockets remote host
 * 
 * @return the sockets remote host as unsigned long
 */
unsigned long Socket::getRemoteHost()
{
	return m_RemoteHost;
}


/**
 * get the sockets DialogueFactory list
 * 
 * @return a pointer to the sockets dialogue factory list
 */
list <DialogueFactory *>   * Socket::getFactories()
{
	return &m_DialogueFactories;
}



/**
 * get the Sockets Dialogue list
 * 
 * @return a pointer to the Sockets Dialogue list
 */
list <Dialogue *>          * Socket::getDialogst()
{
	return &m_Dialogues;
}


/**
 * returns the time ramining till timeout for bind sockets
 * 
 * @return the timeout
 */
time_t Socket::getBindTimeout()
{
	return time(NULL) - (m_LastAction + m_BindTimeoutIntervall);
}


/**
 * returns the remaining time till timeout for accept and connect sockets
 * 
 * @return the timeout
 */
time_t Socket::getTimeout()
{
    return time(NULL) - (m_LastAction + m_TimeoutIntervall);
}

/**
 * get the Nepenthes 
 * 
 * @return returns the Nepenthes 
 */
Nepenthes *Socket::getNepenthes()
{
	return m_Nepenthes;
}


/**
 * check if the socket is a accept socket
 * 
 * @return returns true if the socket is a accept socket
 *         else false
 */
bool Socket::isAccept()
{
	if(m_Type & ST_ACCEPT)
		return true;
	return false;
	
}

/**
 * check if the socket is a connect socket
 * 
 * @return returns true if the socket is a connect socket
 *         else false
 */
bool Socket::isConnect()
{
	if(m_Type & ST_CONNECT)
		return true;
	return false;
}

/**
 * check if the socket is a bind socket
 * 
 * @return returns true if the socket is a bind socket
 *         else false
 */
bool Socket::isBind()
{
	if(m_Type & ST_BIND)
		return true;
    return false;
}




string Socket::getDescription()
{
	string sDesc ="Socket " ;
	if( m_Type & ST_TCP )
	{
		sDesc += "TCP ";
	} else
	if( m_Type & ST_UDP )
	{
		sDesc += "UDP ";
	} else
	if( m_Type & ST_RAW )
	{
		sDesc += "RAW ";
	} else
	if( m_Type & ST_UDS )
	{
		sDesc += "UDS ";
	}else
	if ( m_Type & ST_POLL )
	{
		sDesc += "POLL ";
	}
	else
		sDesc += "UKN ";




	if(isAccept())
	{
		sDesc += " (accept) ";
		sDesc += inet_ntoa( *(in_addr *)&m_RemoteHost);
		sDesc += ":";
		stringstream ssString;
		ssString << getRemotePort();
		sDesc += ssString.str();
		sDesc += " -> ";
		sDesc += inet_ntoa( *(in_addr *)&m_LocalHost);
		sDesc += ":";
		stringstream ssString2;
		ssString2 << getLocalPort();
		sDesc += ssString2.str();
	} else
		if(isBind())
	{
		sDesc += " (bind) ";
		sDesc += inet_ntoa( *(in_addr *)&m_RemoteHost);
		sDesc += ":";
		stringstream ssString;
		ssString << getRemotePort();
		sDesc += ssString.str();
		sDesc += " -> ";
		sDesc += inet_ntoa( *(in_addr *)&m_LocalHost);
		sDesc += ":";
		stringstream ssString2;
		ssString2 << getLocalPort();
		sDesc += ssString2.str();

	} else
		if(isConnect())
	{
		sDesc += " (connect) ";
		sDesc += inet_ntoa( *(in_addr *)&m_RemoteHost);
		sDesc += ":";
		stringstream ssString;
		ssString << getRemotePort();
		sDesc += ssString.str();
		sDesc += " -> ";
		sDesc += inet_ntoa( *(in_addr *)&m_LocalHost);
		sDesc += ":";
		stringstream ssString2;
		ssString2 << getLocalPort();
		sDesc += ssString2.str();
	}

	list <DialogueFactory *>::iterator diaf;
	for(diaf = m_DialogueFactories.begin();diaf != m_DialogueFactories.end(); diaf++)
	{
		sDesc += "\n\tDialogueFactory ";
		sDesc += (*diaf)->getFactoryName();
		sDesc += " ";
		sDesc += (*diaf)->getFactoryDescription().c_str();
	}

	return sDesc;
}
