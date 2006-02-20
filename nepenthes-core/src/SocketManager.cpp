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
#include <linux/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif


#include <string>

#include "SocketManager.hpp"
#include "Socket.hpp"
#include "TCPSocket.hpp"
#include "FILESocket.hpp"
#include "UDPSocket.hpp"
#include "RAWSocket.hpp"
#include "POLLSocket.hpp"

#include "Nepenthes.hpp"

#include "LogManager.hpp"

#include "Config.hpp"

using namespace nepenthes;
using namespace std;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_net | l_mgr

SocketManager::SocketManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
    m_UseRawSockets = false;
}

SocketManager::~SocketManager()
{
	// FIXME CLOSE ALL SOCKETS
	while(m_Sockets.size() > 0)
	{
		if ( !(m_Sockets.front()->getType() & ST_NODEL) )
        	delete m_Sockets.front();
		m_Sockets.pop_front();
	}
}

#define PROC_NET_DEV "/proc/net/dev"

bool  SocketManager::Init()
{
    try {
        m_UseRawSockets = m_Nepenthes->getConfig()->getValInt("nepenthes.socketmanager.use_rawsockets");
        if (m_UseRawSockets)
        {
            logInfo("%s","Using Rawsockets\n");
        }
    } catch ( ... ) {
        logCrit("%s","Could not find nepenthes.socketmanager.use_rawsockets in config file, assuming no\n");
    }



#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		logCrit("%s\n","Could not find good Windows Socket dll");
		return false;
	}else
	{
		logInfo("%s\n","WSAStartup worked");
	}
#endif



    if (m_UseRawSockets == true)
    {
#ifdef WIN32
        // win32 raw socket interface lookup & adding here
        SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
        if (sd == SOCKET_ERROR)
        {
            logCrit("Failed to get a socket. Error %i\n", WSAGetLastError());
            return false;
        }

        INTERFACE_INFO InterfaceList[20];
        unsigned long nBytesReturned;
        if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
                     sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
        {
            logCrit("Failed calling WSAIoctl: error %i\n",WSAGetLastError());
            return false;
        }

        int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
        logInfo("There are %i interfaces \n",nNumInterfaces);
        int i;
        for (i = 0; i < nNumInterfaces; ++i)
        {
            logInfo("Interface %i \n", i);

            u_long nFlags = InterfaceList[i].iiFlags;
            if (nFlags & IFF_UP) 
                logInfo("Iface is %s\n","up");
            else                 
                logInfo("Iface is %s\n","down");



            sockaddr_in *pAddress;
            pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
            logInfo("\tip %s\n",inet_ntoa(pAddress->sin_addr));

            pAddress = (sockaddr_in *) & (InterfaceList[i].iiBroadcastAddress);
            logInfo("\tbcast %s\n",inet_ntoa(pAddress->sin_addr));

            pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
            logInfo("\tnetmask %s\n",inet_ntoa(pAddress->sin_addr));

            if (nFlags & IFF_POINTTOPOINT) 
                logInfo("%s\n","\tis point-to-point");
            if (nFlags & IFF_LOOPBACK)     
                logInfo("%s\n","\tis a loopback iface");
            

            string features = "";
            
            if (nFlags & IFF_BROADCAST) 
                features += "bcast ";
            if (nFlags & IFF_MULTICAST)
                features +=  "multicast ";
            logInfo("\tFeatures: %s \n", features.c_str());
        }


        for (i = 0; i < nNumInterfaces; ++i)
        {

            sockaddr_in *pAddress;
            pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);

            RAWSocketListener *sock = new RAWSocketListener(m_Nepenthes,*(unsigned long *)&(pAddress->sin_addr));
            if ( sock->Init() == true )
            {
                m_Sockets.push_back(sock);
            } else
            {
                return false;
            }

        }
#else
		FILE *f = fopen(PROC_NET_DEV,"r");
		if (f== NULL)
		{
			logCrit("Could not open %s \n",PROC_NET_DEV);
			return false;
		}
		char line[512];
		memset(line,0,512);
		bool ifaceline=false;

		list <string> interfaces;

		while (fgets(line,512,f) != NULL)
		{
			if (ifaceline)
			{
//				printf("proc line is '%s' \n",line);
				char *ifacestopp=line;
				char *ifacestart=line;
				while(*ifacestopp != ':')
					ifacestopp++;

				while (*ifacestart == ' ')
					ifacestart++;

				logSpam("iface %.*s \n",ifacestopp-ifacestart,ifacestart);

                interfaces.push_back(string(ifacestart,ifacestopp-ifacestart));

			}else
			if (strstr(line,"bytes") != NULL)
			{
				ifaceline = true;
			}

			

			memset(line,0,512);
		}
        fclose(f);

		list<string>::iterator it;

		for (it=interfaces.begin();it!= interfaces.end();it++)
		{
			logInfo("Interface %s is availible for sniffing\n",it->c_str());
		}

		for ( it=interfaces.begin();it!= interfaces.end();it++ )
		{
			if (strstr(it->c_str(),"eth") == NULL)
			{
				logInfo("No sniffing on %s\n",it->c_str());
				continue;
			}


			struct ifreq ifr;
			memset(&ifr,0,sizeof(struct ifreq));
//			struct ifconf ifc;

			unsigned long localip;
			int fd = socket(AF_INET, SOCK_DGRAM, 0);
			if ( fd >= 0 )
			{
				strcpy(ifr.ifr_name, it->c_str());
				ifr.ifr_addr.sa_family = AF_INET;
				if ( ioctl(fd, SIOCGIFADDR, &ifr) == 0 )
				{
					struct sockaddr_in *ssin;
					ssin = (struct sockaddr_in *) &ifr.ifr_addr;
					logSpam("Interface %s has ip %s \n",it->c_str(),inet_ntoa(*(in_addr *)&ssin->sin_addr.s_addr));

					localip = ssin->sin_addr.s_addr;

/*                    logSpam("Interface %s has ip %s \n",it->c_str(),inet_ntoa(*(in_addr *)&ifr.ifr_addr));
					strcpy(ifr.ifr_name, it->c_str());
					if ( ioctl(fd, SIOCGIFDSTADDR, &ifr) >= 0 )
						logSpam("Interface %s has ... %s \n",it->c_str(),inet_ntoa(*(in_addr *)&ifr.ifr_dstaddr));

					strcpy(ifr.ifr_name, it->c_str());
					if ( ioctl(fd, SIOCGIFBRDADDR, &ifr) >= 0 )
						logSpam("Interface %s has ... %s \n",it->c_str(),inet_ntoa(*(in_addr *)&ifr.ifr_broadaddr));

					strcpy(ifr.ifr_name, it->c_str());
					if ( ioctl(fd, SIOCGIFNETMASK, &ifr) >= 0 )
						logSpam("Interface %s has ... %s \n",it->c_str(),inet_ntoa(*(in_addr *)&ifr.ifr_netmask));
*/					
				} 
//				else
//					memset(&ife->addr, 0, sizeof(struct sockaddr));
			}


			RAWSocketListener *sock;
			sock = new RAWSocketListener(m_Nepenthes,(char *)it->c_str(), localip, IPPROTO_TCP);
			if ( sock->Init() == true )
			{
				m_Sockets.push_back(sock);
			} else
			{
				return false;
			}
			
/*			sock = new RAWSocketListener(m_Nepenthes,(char *)it->c_str(), IPPROTO_UDP);
			if ( sock->Init() == true )
			{
				m_Sockets.push_back(sock);
			} else
			{
				return false;
			}
*/			
		}
		interfaces.clear();
#endif
		
	}
	return true;
}

bool  SocketManager::Exit()
{
	return true;
}

void SocketManager::doList()
{
	list <Socket *>::iterator socket;
	logSpam("=--- %-69s ---=\n","SocketManager");
	int i=0;
	for(socket = m_Sockets.begin();socket != m_Sockets.end();socket++,i++)
	{
		logSpam("  %i) %-8s \n",i,(*socket)->getDescription().c_str());
	}
	logSpam("=--- %2i %-66s ---=\n\n",i, "Sockets open");
}

/**
 * checks all sockets, and polls, handles the send and receive, socket timeouts, accepting new sockets, deleting dead sockets
 * 
 * @param polltimeout
 *               the polltimeout we want to use in milliseconds
 * 
 * @return returns true if something was polled
 *         else false
 */
#ifdef WIN32
bool SocketManager::doLoop(unsigned int polltimeout)
{// FIXME ..
	list <Socket *>::iterator itSocket;

// 	check socket timeouts and remove dead sockets
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{
		(*itSocket)->checkTimeout();
		if ((*itSocket)->getStatus() == SS_TIMEOUT )
		{
			logInfo("Deleting Socket %s due to timeout \n",(*itSocket)->getDescription().c_str());
			Socket *delsocket = *itSocket;
			m_Sockets.erase(itSocket);
			delete delsocket;

			itSocket = m_Sockets.begin(); // FIXME ?
			if(m_Sockets.size() == 0)
            	return false;
			

		}

		if ( (*itSocket)->getStatus() == SS_CLOSED )
		{
			logInfo("Deleting %s due to closed connection \n",(*itSocket)->getDescription().c_str());
			Socket *delsocket = *itSocket;
			m_Sockets.erase(itSocket);
			delete delsocket;
			itSocket = m_Sockets.begin(); // FIXME ?
		}
	}

	fd_set rfds;
	fd_set wfds;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);


	int i=0;
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{
		int iError = 0;
		int iSize = sizeof(iError);
		if((*itSocket)->getType() & ST_FILE)
		{
			(*itSocket)->setPolled();
		}else
		if ((*itSocket)->getsockOpt(SOL_SOCKET, SO_ERROR, &iError,(socklen_t *) &iSize) != 0 )
		{
			// socket is dead
			logSpam("Socket %s is Dead\n",(*itSocket)->getDescription().c_str());
			(*itSocket)->unsetPolled();

		} else
		{
			switch (iError)
			{
			case 0:	// der socket is soweit okay
//				logSpam("Socket %s is OK\n",(*itSocket)->getDescription().c_str());
				(*itSocket)->setPolled();
				break;

			case WSAEINPROGRESS: // der socket versuchts
				(*itSocket)->unsetPolled();
				break;

			case WSAEISCONN:
				(*itSocket)->setPolled();	// der socket ist am start
				break;


			default:
				(*itSocket)->unsetPolled();		// der is defekt
			}
		}
	}

	i=0;
	int maxsock=-1;
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{	
		if ((*itSocket)->isPolled() == true )
		{
			if ((*itSocket)->getSocket() > maxsock)
			{
				maxsock = (*itSocket)->getSocket();
			}

			FD_SET((*itSocket)->getSocket(),&rfds);

			if ((*itSocket)->wantSend() == true)
			{
				FD_SET((*itSocket)->getSocket(),&wfds);
			}/*else
				logSpam("polling %s readonly\n",(*itSocket)->getDescription().c_str());*/
			i++;
		}
	}

	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	int iPollRet = select(maxsock,&rfds,&wfds,NULL,&tv);

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
					if ( FD_ISSET((*itSocket)->getSocket(),&rfds) )
					{
						(*itSocket)->doRecv();
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
//				logSpam(" COuld write #%i\n",1);
				// doRecv() can close sockets
				// we need a valid way to verify we dont try to send on a closed socket, 
				// i think wantSend() is a good option here
				// getStatus i just a cheap fix
//				logInfo("SSS %s \n",(*itSocket)->getDescription().c_str());
				if (
                    ( (*itSocket)->getStatus() == SS_NULL || (*itSocket)->getStatus() == SS_CLEANQUIT ) &&  
				    (
				     (*itSocket)->isAccept() ||
				     (*itSocket)->isConnect() || 
				     (
				      (*itSocket)->isBind() && (*itSocket)->getType() & ST_UDP
				     )
				    )  
				   )
				{
//					if ( iPollRet == 0 )
//						continue;
//					logSpam(" COuld write #%i\n",2);
					if (FD_ISSET((*itSocket)->getSocket(),&wfds))
					{
//						logSpam(" COuld write #%i\n",3);
						(*itSocket)->doSend();

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
					if ( !((*itSocket)->getType() & ST_UDP) ) // bound udp sockets dont accept, they recvfrom
					{
						if (FD_ISSET((*itSocket)->getSocket(),&rfds))
						{
							logInfo("%s could Accept a Connection\n",(*itSocket)->getDescription().c_str());
							Socket * socket = (*itSocket)->acceptConnection();
							if ( socket == NULL )
							{
								logCrit("%s","Accept returned NULL ptr \n");
							} else
							{
								m_Sockets.push_back(socket);
								logDebug("Accepted Connection %s \n%i Sockets in list\n",socket->getDescription().c_str(), m_Sockets.size());
							}
						}
					}
				}
				i++;
			}
		}
	}
//	free(polls);
//	sleep(1);
	return true;
}
#else

bool SocketManager::doLoop(unsigned int polltimeout)
{// FIXME ..




	list <Socket *>::iterator itSocket;

// 	check socket timeouts and remove dead sockets
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{
		(*itSocket)->checkTimeout();
		if ((*itSocket)->getStatus() == SS_TIMEOUT )
		{
			logInfo("Deleting Socket %s due to timeout \n",(*itSocket)->getDescription().c_str());
			Socket *delsocket = *itSocket;
			m_Sockets.erase(itSocket);
			delete delsocket;

			itSocket = m_Sockets.begin(); // FIXME ?
			if(m_Sockets.size() == 0)
            	return false;
			

		}

		if ( (*itSocket)->getStatus() == SS_CLOSED )
		{
			logInfo("Deleting %s due to closed connection \n",(*itSocket)->getDescription().c_str());
			Socket *delsocket = *itSocket;
			m_Sockets.erase(itSocket);
			delete delsocket;
			itSocket = m_Sockets.begin(); // FIXME ?
		}
	}

	pollfd *polls = (pollfd *) malloc( (m_Sockets.size())* sizeof(pollfd));
	memset(polls,0,(m_Sockets.size())* sizeof(pollfd));
	int i=0;
	for (itSocket = m_Sockets.begin();itSocket != m_Sockets.end(); itSocket++)
	{
		int iError = 0;
		int iSize = sizeof(iError);
		if((*itSocket)->getType() & ST_FILE)
		{
			(*itSocket)->setPolled();
		}else
		if ((*itSocket)->getsockOpt(SOL_SOCKET, SO_ERROR, &iError,(socklen_t *) &iSize) != 0 )
		{
			// socket is dead
			logSpam("Socket %s is Dead\n",(*itSocket)->getDescription().c_str());
			(*itSocket)->unsetPolled();

		} else
		{
			switch (iError)
			{
			case 0:	// der socket is soweit okay
//				logSpam("Socket %s is OK\n",(*itSocket)->getDescription().c_str());
				(*itSocket)->setPolled();
				break;

			case EINPROGRESS: // der socket versuchts
				(*itSocket)->unsetPolled();
				break;

			case EISCONN:
				(*itSocket)->setPolled();	// der socket ist am start
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
//				logSpam("polling %s read|write\n",(*itSocket)->getDescription().c_str());
					
				polls[i].events |= POLLOUT;
			}/*else
				logSpam("polling %s readonly\n",(*itSocket)->getDescription().c_str());*/
			i++;
		}
	}

	int iPollRet = poll(polls,i, 1500);

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
//				logSpam(" COuld write #%i\n",1);
				// doRecv() can close sockets
				// we need a valid way to verify we dont try to send on a closed socket, 
				// i think wantSend() is a good option here
				// getStatus i just a cheap fix
//				logInfo("SSS %s \n",(*itSocket)->getDescription().c_str());
				if (
                    ( (*itSocket)->getStatus() == SS_NULL || (*itSocket)->getStatus() == SS_CLEANQUIT ) &&  
				    (
				     (*itSocket)->isAccept() ||
				     (*itSocket)->isConnect() || 
				     (
				      (*itSocket)->isBind() && (*itSocket)->getType() & ST_UDP
				     )
				    )  
				   )
				{
//					if ( iPollRet == 0 )
//						continue;
//					logSpam(" COuld write #%i\n",2);
					if ( polls[i].revents & POLLOUT && polls[i].events & POLLOUT )
					{
//						logSpam(" COuld write #%i\n",3);
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
							logInfo("%s could Accept a Connection\n",(*itSocket)->getDescription().c_str());
							Socket * socket = (*itSocket)->acceptConnection();
							if ( socket == NULL )
							{
								logCrit("%s","Accept returned NULL ptr \n");
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
//	sleep(1);
	return true;
}
#endif

/**
 * bind a given port to a given local ip address
 * 
 * @param localHost the localhost ipv4 address we want to bint the port
 * @param Port      the port we want to bind
 * @param bindtimeout
 *                  the timeout for the bind socket in seconds
 * @param accepttimeout
 *                  the timeout for the accepted connections sockets in seconds
 * 
 * @return returns the bound Socket if binding was successfull, else NULL
 */
Socket *SocketManager::bindTCPSocket(unsigned long localhost, unsigned int port,time_t bindtimeout,time_t accepttimeout)
{
	logSpam("bindTCPSocket %li %i %li %li\n",localhost,port,bindtimeout,accepttimeout);
	TCPSocket *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_TCP && (*socket)->isBind() && (*socket)->getLocalPort() == (int)port )
		{
			return (*socket);
		}
	}

	if(sock == NULL)
	{
		if ((sock = new TCPSocket(getNepenthes(), localhost, port, bindtimeout, accepttimeout)) == NULL )
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


Socket *SocketManager::bindTCPSocket(unsigned long localhost, unsigned int port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory)
{
	logSpam("bindTCPSocket %li %i %li %li %lx\n",localhost,port,bindtimeout,accepttimeout, dialoguefactory);
	TCPSocket *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_TCP && (*socket)->isBind() && (*socket)->getLocalPort() == (int)port )
		{
			(*socket)->addDialogueFactory(dialoguefactory);
			return (*socket);
		}
	}

	if(sock == NULL)
	{
		if ((sock = new TCPSocket(getNepenthes(), localhost, port, bindtimeout, accepttimeout)) == NULL )
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


Socket *SocketManager::bindUDPSocket(unsigned long localhost, unsigned int port,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory)
{
	logSpam("bindUDPSocket %li %i %li %li\n",localhost,port,bindtimeout,accepttimeout);
	UDPSocket *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_UDP && (*socket)->isBind() && (*socket)->getLocalPort() == (int)port )
		{
			(*socket)->addDialogueFactory(dialoguefactory);
			return (*socket);
		}
	}

	if(sock == NULL)
	{
		if ((sock = new UDPSocket(getNepenthes(), localhost, port, bindtimeout, accepttimeout)) == NULL )
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



Socket *SocketManager::bindTCPSocket(unsigned long localHost, unsigned int Port,time_t bindtimeout,time_t accepttimeout, char *dialoguefactoryname)
{
	return NULL;
}


Socket *SocketManager::openFILESocket(char *filepath, int flags)
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

Socket *SocketManager::connectUDPHost(unsigned long localhost, unsigned long remotehost, unsigned int port,time_t connecttimeout)
{
	logPF();
	UDPSocket *sock = new UDPSocket(getNepenthes(),localhost,remotehost,port,connecttimeout);
	sock->Init();
	m_Sockets.push_back(sock);
	return sock;
}

Socket *SocketManager::connectTCPHost(unsigned long localhost, unsigned long remotehost, unsigned int port,time_t connecttimeout)
{
	logPF();
	TCPSocket *sock = new TCPSocket(getNepenthes(),localhost,remotehost,port,connecttimeout);
	sock->Init();
	m_Sockets.push_back(sock);
	return sock;
}

Socket *SocketManager::addPOLLSocket(POLLSocket *sock)
{
	m_Sockets.push_back(sock);
	return sock;
}

Socket *SocketManager::createRAWSocketUDP(unsigned int localport, unsigned int remoteport, time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory)
{
	logSpam("createRAWPSocketUDP %i %i %i %i \n",localport,remoteport,bindtimeout,accepttimeout);
	//RAWSocketListener *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_RAW )
		{
			((RAWSocketListener *)(*socket))->addListenFactory(localport,remoteport,IPPROTO_UDP,dialoguefactory);
//			return (*socket);
		}
	}
	return NULL;
}

Socket *SocketManager::createRAWSocketTCP(unsigned int localport,unsigned int remoteport,time_t bindtimeout,time_t accepttimeout, DialogueFactory *dialoguefactory)
{
	logSpam("createRAWPSocketTCP %i %i %i %i \n",localport,remoteport,bindtimeout,accepttimeout);
	//RAWSocketListener *sock = NULL;

	list <Socket *>::iterator socket;
	for(socket = m_Sockets.begin();socket != m_Sockets.end(); socket++)
	{
		if((*socket)->getType() & ST_RAW )
		{
			((RAWSocketListener *)(*socket))->addListenFactory(localport,remoteport,IPPROTO_TCP,dialoguefactory);
//			return (*socket);
		}
	}
	return NULL;
}
