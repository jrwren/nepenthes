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
#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Socket.hpp"
#include "DialogueFactory.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_net

#include <cstring>


using namespace std;

using namespace nepenthes;


/**
 * add a DialogueFactory to the Socket
 * 
 * @param diaf   the DialogueFactory to add
 * 
 * @return returns true
 */
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
    	logDebug("%s \n\tAdding DialogueFactory %s \n",getDescription().c_str(),diaf->getFactoryName().c_str());
		m_DialogueFactories.push_back(diaf);
	}else
	{
		logDebug("%s \tAdding DialogueFactory: already known\n",getDescription().c_str(),diaf->getFactoryName().c_str());
	}
	return true;
}


/**
 * add a Dialogue to the Socket
 * 
 * @param dia    the Dialogue to add
 * 
 * @return returns true
 */
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
int32_t   Socket::getStatus()
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
int32_t   Socket::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
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
int32_t   Socket::getSocket()
{
	return m_Socket;
}


/**
 * set the socket
 * 
 * @param i      the new socket
 */
void  Socket::setSocket(int32_t i)
{
	m_Socket = i;
	return;
}


/**
 * get the sockets type
 * 
 * @return returns the sockets type
 */
int32_t   Socket::getType()
{
	return m_Type;
}


/**
 * get the sockets local port
 * 
 * @return returns the sockets local port
 */
uint16_t   Socket::getLocalPort()
{
	return m_LocalPort;
}


/**
 * get the sockets remote port
 * 
 * @return returns the sockets remote port
 */
uint16_t  Socket::getRemotePort()
{

	return m_RemotePort;
}


/**
 * set the sockets localport
 * 
 * @param i      the new local port as int32_t in host byte order
 */
void  Socket::setLocalPort(uint16_t i)
{
	m_LocalPort = i;
	return;
}


/**
 * set the sockets remoteport
 * 
 * @param i      the new remote port as int32_t in host byte order
 */
void  Socket::setRemotePort(uint16_t i)
{
	m_RemotePort = i;
	return;
}


/**
 * set the sockets remotehost
 * 
 * @param i      the new remote host as uint32_t in network byte order
 */
void Socket::setRemoteHost(uint32_t i)
{
	m_RemoteHost = i;
	return;
}


/**
 * set the sockets localhost
 * 
 * @param i      the new localhost as uint32_t in network byte order
 */
void Socket::setLocalHost(uint32_t i)
{
	m_LocalHost = i;
	return;
}


/**
 * get the sockets local host
 * 
 * @return returns the sockets localhost as uint32_t
 */
uint32_t Socket::getLocalHost()
{
	return m_LocalHost;
}


/**
 * get the sockets remote host
 * 
 * @return the sockets remote host as uint32_t
 */
uint32_t Socket::getRemoteHost()
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




/**
 * get a description of the Socket
 * 
 * @return returns a description as string
 */
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

bool Socket::getRemoteHWA(string *address)
{
	if ( !(m_Type & ST_TCP) && !(m_Type & ST_UDP) )
	{
		return false;
	}

/*
 *
 * borrowed from arp.c in net-tools 
 *
 */

#define _PATH_PROCNET_ARP               "/proc/net/arp"
	char ip[101];
	char hwa[101];
	char mask[101];
	char line[200];
	char dev[101];
	int type, flags;
	FILE *fp;

	/* Open the PROCps kernel table. */
	if ( (fp = fopen(_PATH_PROCNET_ARP, "r")) == NULL )
	{
		logCrit("Could not open %s\n",_PATH_PROCNET_ARP);
		return false;
	}

	/* Bypass header -- read until newline */
	if ( fgets(line, sizeof(line), fp) != (char *) NULL )
	{
		strcpy(mask, "-");
		strcpy(dev, "-");
		/* Read the ARP cache entries. */
		for ( ; fgets(line, sizeof(line), fp); )
		{
			int num = sscanf(line, "%s 0x%x 0x%x %100s %100s %100s\n",
							 ip, &type, &flags, hwa, mask, dev);
			if ( num < 4 )
				break;

			if ( inet_addr(ip) == m_RemoteHost )
			{
//				logSpam("ip:%s type:0x%x flags:0x%x hwa:%s mask:%s dev:%s\n",ip, type, flags, hwa, mask, dev);
				*address = hwa;
				fclose(fp);
				return true;
			}
		}
	}

	fclose(fp);
	return false;
}
