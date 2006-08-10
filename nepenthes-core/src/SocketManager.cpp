/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter & Georg Wicherski
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

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream.h>
#else

#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif


#include <string>

#include "SocketManager.hpp"
#include "Socket.hpp"
#include "TCPSocket.hpp"
#include "FILESocket.hpp"
#include "UDPSocket.hpp"
#include "POLLSocket.hpp"

#include "Nepenthes.hpp"

#include "LogManager.hpp"

#include "Config.hpp"

#ifdef __linux__ // bind to interface on linux
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // end bind to if

using namespace nepenthes;
using namespace std;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_net | l_mgr

/**
 * SocketManager constructor
 * 
 * @param nepenthes the nepenthes
 */
SocketManager::SocketManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
    m_UseRawSockets = false;
	m_BindAddress = INADDR_ANY;
}

/**
 * SocketManager destructor
 */
SocketManager::~SocketManager()
{
	Exit();
}

#define PROC_NET_DEV "/proc/net/dev"

/**
 * check config values
 * 
 * @return true on success, 
 *         else false
 */
bool  SocketManager::Init()
{
	try {
		string bindAddressString = m_Nepenthes->getConfig()->getValString("nepenthes.socketmanager.bind_address");
		
		#ifdef __linux__
		if(bindAddressString.substr(0, 3) == string("if:"))
		{
			const char * interfaceName = bindAddressString.substr(3).c_str();
			int ifaceSocket = socket(AF_INET, SOCK_STREAM, 0);
			struct ifreq interfaceRequest;
			struct sockaddr_in addrInterface;
				                               
			strncpy(interfaceRequest.ifr_name, interfaceName, IFNAMSIZ - 1);
			
			if(ifaceSocket < 0 || ioctl(ifaceSocket, SIOCGIFADDR, &interfaceRequest) < 0)
			{
				logCrit("Failed to obtain address for interface %s: %s!\n", interfaceName, strerror(errno));
			} else
			{              
					memcpy(&addrInterface, &(interfaceRequest.ifr_addr), sizeof(addrInterface));
					logInfo("Obtained address of interface %s: %s\n", interfaceName, inet_ntoa(addrInterface.sin_addr));
					m_BindAddress = addrInterface.sin_addr.s_addr;
					
					close(ifaceSocket);
			}
			
		} else
		#endif
		{
			m_BindAddress = inet_addr(bindAddressString.c_str());
		}
		
		if (m_BindAddress != INADDR_ANY)
		{
			logInfo("Using %s as bind_address for all connections\n", inet_ntoa(*(struct in_addr *)&m_BindAddress));
		}
	} catch ( ... ) {
		logCrit("Could not find nepenthes.socketmanager.bind_address in config file, assuming no\n");
	}

	return true;
}

bool  SocketManager::Exit()
{
	while(m_Sockets.size() > 0)
	{
		if ( !(m_Sockets.front()->getType() & ST_NODEL) )
			delete m_Sockets.front();
		m_Sockets.pop_front();
	}

	return true;
}

/**
 * list all used Socket 's
 */
void SocketManager::doList()
{
	list <Socket *>::iterator socket;
	logSpam("=--- %-69s ---=\n","SocketManager");
	int32_t i=0;
	for(socket = m_Sockets.begin();socket != m_Sockets.end();socket++,i++)
	{
		logSpam("  %i) %-8s \n",i,(*socket)->getDescription().c_str());
	}
	logSpam("=--- %2i %-66s ---=\n\n",i, "Sockets open");
}

/**
 * poll the sockets
 * 
 * @param polltimeout
 *               the polltimeout
 * 
 * @return returns true
 */
bool SocketManager::doLoop(uint32_t polltimeout)
{
	list <Socket *>::iterator itSocket;

// 	check socket timeouts and remove dead sockets
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{
		(*itSocket)->checkTimeout();
		if ((*itSocket)->getStatus() == SS_TIMEOUT )
		{
			logDebug("Deleting Socket %s due to timeout \n",(*itSocket)->getDescription().c_str());
			Socket *delsocket = *itSocket;
			m_Sockets.erase(itSocket);
			delete delsocket;

			itSocket = m_Sockets.begin(); // FIXME ?
			if(m_Sockets.size() == 0)
            	return false;
			

		}

		if ( (*itSocket)->getStatus() == SS_CLOSED )
		{
			logDebug("Deleting %s due to closed connection \n",(*itSocket)->getDescription().c_str());
			Socket *delsocket = *itSocket;
			m_Sockets.erase(itSocket);
			delete delsocket;
			itSocket = m_Sockets.begin(); // FIXME ?
		}
	}

	pollfd *polls = (pollfd *) malloc( (m_Sockets.size())* sizeof(pollfd));
	memset(polls,0,(m_Sockets.size())* sizeof(pollfd));
	int32_t i=0;
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{
		int32_t iError = 0;
		int32_t iSize = sizeof(iError);
		if((*itSocket)->getType() & ST_FILE)
		{
			(*itSocket)->setPolled();
		}else
		if ((*itSocket)->getsockOpt(SOL_SOCKET, SO_ERROR, &iError,(socklen_t *) &iSize) != 0 )
		{
			// socket is dead
			logSpam("Socket %i %s is Dead\n",(*itSocket)->getSocket(), (*itSocket)->getDescription().c_str());
			(*itSocket)->unsetPolled();

		} else
		{
			switch (iError)
			{
			case 0:	// der socket is soweit okay
			case EISCONN:
				if ((*itSocket)->getStatus() == SS_CONNECTING)
				{
					(*itSocket)->setStatus(SS_CONNECTED);
				}
				(*itSocket)->setPolled();	// der socket ist am start
				break;

			case EINPROGRESS: // der socket versuchts
				(*itSocket)->unsetPolled();
				break;


			default:
				(*itSocket)->unsetPolled();		// der is defekt
			}
		}
	}

	i=0;
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{
		polls[i].events = 0;
		if ((*itSocket)->isPolled() == true )
		{
			polls[i].fd = (*itSocket)->getSocket();
			polls[i].events = POLLIN;

			if ((*itSocket)->wantSend() == true)
			{
				polls[i].events |= POLLOUT;
			}
			i++;
		}
	}

	int32_t iPollRet = poll(polls,i,50);

	if (iPollRet != 0)
	{
		// read sockets
		i=0;
		for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
		{
			if ( (*itSocket)->isPolled() == true )
			{
				if ( 
				   ( (*itSocket)->isAccept()  || (*itSocket)->isConnect() ) ||
				   (  (*itSocket)->isBind() && (*itSocket)->getType() & ST_UDP)	  // bound udp sockets dont accept, they recvfrom
				   )
				{
					if ( iPollRet == 0 )
						continue;

					if ( polls[i].revents & POLLIN && polls[i].events & POLLIN )
					{
						(*itSocket)->doRecv();
						iPollRet--;
					}
				}
				i++;
			}
		}

		// write sockets
		i=0;
		for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
		{
			if ( (*itSocket)->isPolled() == true )
			{
				if (
                    ( (*itSocket)->getStatus() == SS_CONNECTED || (*itSocket)->getStatus() == SS_CLEANQUIT ) &&  
				    (
				     (*itSocket)->isAccept() ||
				     (*itSocket)->isConnect() || 
				     (
				      (*itSocket)->isBind() && (*itSocket)->getType() & ST_UDP
				     )
				    )  
				   )
				{
					if ( polls[i].revents & POLLOUT && polls[i].events & POLLOUT )
					{
						(*itSocket)->doSend();
						iPollRet--;
					}
				}
				i++;
			}
		}


		// accept new, non udp clients as udp does not accept()
		i=0;
		for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
		{
				

			if ( (*itSocket)->isPolled() == true )
			{
				if ( (*itSocket)->isBind() )
				{
					if ( iPollRet == 0 )
						continue;

					if ( !((*itSocket)->getType() & ST_UDP) ) // bound udp sockets dont accept, they recvfrom
					{
						if ( polls[i].revents & POLLIN && polls[i].events & POLLIN )
						{
							logDebug("%s could Accept a Connection\n",(*itSocket)->getDescription().c_str());
							Socket * socket = (*itSocket)->acceptConnection();
							if ( socket == NULL )
							{
								logCrit("Accept returned NULL ptr \n");
							} else
							{
								m_Sockets.push_back(socket);
								logDebug("Accepted Connection %s \n%i Sockets in list\n",socket->getDescription().c_str(), m_Sockets.size());
							}
							iPollRet--;
						}
					}
				}
				i++;
			}
		}
	}
	free(polls);
	return true;
}


/**
 * bind a given port to a given local ip address
 * 
 * @param localhost the local ip address to bind to
 * @param port      the port to bind
 * @param bindtimeout
 *                  the timeout for the bind socket in seconds
 * @param accepttimeout
 *                  the timeout for the accepted connections sockets in seconds
 * 
 * @return returns the bound Socket if binding was successfull, else NULL
 */
Socket *SocketManager::bindTCPSocket(uint32_t localhost, uint16_t port,time_t bindtimeout,time_t accepttimeout)
{
	if ( localhost == INADDR_ANY && m_BindAddress != INADDR_ANY )
	{
		logDebug("Changed local Bind address from 0.0.0.0 to %s \n",inet_ntoa(*(in_addr *)&m_BindAddress));
		localhost = m_BindAddress;
	}

	logSpam("bindTCPSocket %li %i %li %li\n",localhost,port,bindtimeout,accepttimeout);
	TCPSocket *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_TCP && (*socket)->isBind() && (*socket)->getLocalPort() == (int32_t)port )
		{
			return (*socket);
		}
	}

	if(sock == NULL)
	{
		// This can bee seen as ambiguous - at least on FreeBSD. We want this:
		// TCPSocket(Nepenthes *nepenthes, uint32_t localaddress, int32_t port, time_t bindtimeout, time_t accepttimeout)
		if ((sock = new TCPSocket(getNepenthes(), (uint32_t)localhost, (uint16_t)port, (time_t) bindtimeout, (time_t) accepttimeout)) == NULL )
		{
			logCrit("ERROR Binding %s:%i failed\n","",port);
			return NULL;
		}else
		{
			if(sock->Init() == false)
			{
				logCrit("ERROR Binding %s:%i failed\n","",port);
				delete sock;
				return NULL;
			}else
			{
				m_Sockets.push_back(sock);
            	return sock;
			}
		}
	}
	return sock;
}


Socket *SocketManager::bindTCPSocket(uint32_t localhost, uint16_t port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory)
{
	if ( localhost == INADDR_ANY && m_BindAddress != INADDR_ANY )
	{
		logDebug("Changed local Bind address from 0.0.0.0 to %s \n",inet_ntoa(*(in_addr *)&m_BindAddress));
		localhost = m_BindAddress;
	}


	logSpam("bindTCPSocket %li %i %li %li %lx\n",localhost,port,bindtimeout,accepttimeout, dialoguefactory);
	TCPSocket *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_TCP && (*socket)->isBind() && (*socket)->getLocalPort() == (int32_t)port )
		{
			(*socket)->addDialogueFactory(dialoguefactory);
			return (*socket);
		}
	}

	if(sock == NULL)
	{
		if ((sock = new TCPSocket(getNepenthes(), localhost, (int32_t) port, bindtimeout, accepttimeout)) == NULL )
		{
			logCrit("ERROR Binding %s:%i failed\n","",port);

			return NULL;
		}else
		{
			if(sock->Init() == false)
			{
				logCrit("ERROR Binding %s:%i failed\n","",port);
				delete sock;
				return NULL;
			}else
			{
				m_Sockets.push_back(sock);
				sock->addDialogueFactory(dialoguefactory);
            	return sock;
			}
		}
	}
    return NULL;
}


Socket *SocketManager::bindUDPSocket(uint32_t localhost, uint16_t port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory)
{
	if ( localhost == INADDR_ANY && m_BindAddress != INADDR_ANY )
	{
		logDebug("Changed local Bind address from 0.0.0.0 to %s \n",inet_ntoa(*(in_addr *)&m_BindAddress));
		localhost = m_BindAddress;
	}


	logSpam("bindUDPSocket %li %i %li %li\n",localhost,port,bindtimeout,accepttimeout);
	UDPSocket *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_UDP && (*socket)->isBind() && (*socket)->getLocalPort() == (int32_t)port )
		{
			(*socket)->addDialogueFactory(dialoguefactory);
			return (*socket);
		}
	}

	if(sock == NULL)
	{
		if ((sock = new UDPSocket(getNepenthes(), localhost, (int32_t) port, bindtimeout, accepttimeout)) == NULL )
		{
			logCrit("ERROR Binding %s:%i failed\n","",port);
			return NULL;
		}else
		{
			if(sock->Init() == false)
			{
				logCrit("ERROR Binding %s:%i failed\n","",port);
				delete sock;
				return NULL;
			}else
			{
				m_Sockets.push_back(sock);
				sock->addDialogueFactory(dialoguefactory);
            	return sock;
			}
		}
	}
    return NULL;
}



Socket *SocketManager::bindTCPSocket(uint32_t localHost, uint16_t Port,time_t bindtimeout,time_t accepttimeout, char *dialoguefactoryname)
{
	return NULL;
}


Socket *SocketManager::openFILESocket(char *filepath, int32_t flags)
{
#ifdef WIN32
	return NULL;
#else
	FILESocket *sock = new FILESocket(getNepenthes(),filepath, flags);
	sock->Init();
	m_Sockets.push_back(sock);
	return sock;
#endif
}

Socket *SocketManager::connectUDPHost(uint32_t localhost, uint32_t remotehost, uint16_t port,time_t connecttimeout)
{
	logPF();
	if ( localhost == INADDR_ANY && m_BindAddress != INADDR_ANY )
	{
		logDebug("Changed local Bind address from 0.0.0.0 to %s \n",inet_ntoa(*(in_addr *)&m_BindAddress));
		localhost = m_BindAddress;
	}

	UDPSocket *sock = new UDPSocket(getNepenthes(),localhost,remotehost,port,connecttimeout);
	sock->Init();
	m_Sockets.push_back(sock);
	return sock;
}

Socket *SocketManager::connectTCPHost(uint32_t localhost, uint32_t remotehost, uint16_t remoteport,time_t connecttimeout)
{
	logPF();
	if ( localhost == INADDR_ANY && m_BindAddress != INADDR_ANY )
	{
		logDebug("Changed local Bind address from 0.0.0.0 to %s \n",inet_ntoa(*(in_addr *)&m_BindAddress));
		localhost = m_BindAddress;
	}

	TCPSocket *sock = new TCPSocket(getNepenthes(),localhost,remotehost,remoteport,connecttimeout);
	sock->Init();
	m_Sockets.push_back(sock);
	return sock;
}

Socket *SocketManager::connectTCPHost(uint32_t localhost, uint32_t remotehost, uint16_t localport, uint16_t remoteport,time_t connecttimeout)
{
	logPF();
	if ( localhost == INADDR_ANY && m_BindAddress != INADDR_ANY )
	{
		logDebug("Changed local Bind address from 0.0.0.0 to %s \n",inet_ntoa(*(in_addr *)&m_BindAddress));
		localhost = m_BindAddress;
	}

	TCPSocket *sock = new TCPSocket(getNepenthes(),localhost,remotehost,localport,remoteport,connecttimeout);
	if ( sock->Init() != true )
	{
		delete sock;
		return NULL;
	}
	m_Sockets.push_back(sock);
	return sock;
}


Socket *SocketManager::addPOLLSocket(POLLSocket *sock)
{
	m_Sockets.push_back(sock);
	return sock;
}

bool SocketManager::removePOLLSocket(POLLSocket *sock)
{
	logPF();
	list <Socket *>::iterator it;
	for ( it = m_Sockets.begin();it != m_Sockets.end(); it++ )
	{
		if (sock == (*it))
		{
			/* this is really *bad*
			 * as it may change the order of pollfd's
			 * we should replace the socket to remove with a dead dummy socket who shares the same poll flags instead, and 
			 * let the poll loop remove the socket instead
			 * but this works for now, and does not make any problem, so ...
			 */
			m_Sockets.erase(it);
			return true;
		}
	}
	return false;
}

