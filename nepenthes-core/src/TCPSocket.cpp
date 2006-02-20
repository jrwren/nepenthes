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

#include "config.h"

#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#endif

#include <errno.h>
#include <fcntl.h>

#include "TCPSocket.hpp"
#include "DialogueFactory.hpp"
#include "Packet.hpp"
#include "Message.hpp"
#include "Dialogue.hpp"
#include "Nepenthes.hpp"
#include "SocketEvent.hpp"
#include "EventManager.hpp"
#include "Nepenthes.hpp"

#include "LogManager.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_net | l_hlr



/**
 * set the sockets status
 * if we are
 * - a connect socket
 * - in state SS_CONNECTING and set to state SS_CONNECTED
 * -> inform the attached dialogues about it
 * 
 * @param i      the new socket status
 */
void  TCPSocket::setStatus(socket_state i)
{
	logPF();
	if (isConnect() && i == SS_CONNECTED && m_Status == SS_CONNECTING)
	{
		list <Dialogue *>::iterator dia;
		for ( dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++ )
		{
			(*dia)->connectionEstablished();
		}
	}
	m_Status = i;
	return;
}




/**
 * contructor for bind sockets
 * 
 * @param nepenthes ptr to the nepenthes
 * @param localhost localhosts ip in network byte order if we dont want to bind to INADDR_ANY
 *                  
 * @param port      the port we want to bind
 * @param bindtimeout
 *                  the timeoutintervall for the bind socket
 * @param accepttimeout
 *                  the timeout intervall for all sockets getting acepted by this bind socket
 */
TCPSocket::TCPSocket(Nepenthes *nepenthes, uint32_t localhost, int32_t port, time_t bindtimeout, time_t accepttimeout)
{
	setLocalHost(localhost);
	setLocalPort(port);

	setRemoteHost(inet_addr("0.0.0.0"));
	setRemotePort(0);

	m_BindTimeoutIntervall = bindtimeout;
	m_TimeoutIntervall = accepttimeout;
	m_LastAction = time(NULL);

	m_Type = ST_TCP | ST_BIND;

	m_CanSend = true;
	m_Status = SS_CONNECTED;
	m_Polled = false;
	m_Nepenthes = nepenthes;

	m_HighestConsumeLevel = CL_DROP;
}

/**
 * constructor for a accept()'ed connection
 * 
 * @param nepenthes  ptr to the nepenthes
 * @param socket     the socket accept() gave us
 * @param localhost  the localhosts address the bind socket was bound to
 * @param localport  the localport
 * @param remotehost the remotehosts address accept() gave us
 * @param remoteport the remoteport accept() gave us
 * @param accepttimeout
 *                   the accept timeout fromt he bind socket who accepted this connection
 */
TCPSocket::TCPSocket(Nepenthes *nepenthes, int32_t socket, uint32_t localhost, int32_t localport, uint32_t  remotehost,int32_t remoteport, time_t accepttimeout)
{
	m_Nepenthes = nepenthes;
	setSocket(socket);

	setLocalHost(localhost);
	setLocalPort(localport);

	setRemoteHost(remotehost);
	setRemotePort(remoteport);

	m_BindTimeoutIntervall = 0;
	m_TimeoutIntervall = accepttimeout;
	m_LastAction = time(NULL);

	m_Type = ST_TCP | ST_ACCEPT;

	m_CanSend = true;
	m_Status = SS_CONNECTED;
	m_Polled = false;

	m_HighestConsumeLevel = CL_DROP;
}



/**
 * constructor for connect sockets
 * 
 * @param nepenthes  the nepenthes
 *                   
 * @param localhost  the local ip address to bind the socket to
 * @param remotehost the remote hosts ip address
 * @param remoteport the port to connect to
 *                   
 * @param connectiontimeout
 *                   the timeout before we drop this try
 */
TCPSocket::TCPSocket(Nepenthes *nepenthes,uint32_t localhost, uint32_t remotehost, int32_t remoteport, time_t connectiontimeout)
{
	m_Nepenthes = nepenthes;
	setLocalPort(0);
	setLocalHost(localhost);
	setRemoteHost(remotehost);
	setRemotePort(remoteport);

	m_TimeoutIntervall = connectiontimeout;
	m_LastAction = time(NULL);

	m_Type = ST_TCP|ST_CONNECT;

	m_CanSend = true;
	m_Status = SS_CONNECTING;
	m_Polled = false;

	m_HighestConsumeLevel = CL_DROP;
}




TCPSocket::~TCPSocket()
{
	logPF();
	if( m_DialogueFactories.size() > 0 )
	{
		logSpam("%s clearing DialogueFactory List (%i entries) \n",getDescription().c_str(),m_DialogueFactories.size());

		while( m_DialogueFactories.size() > 0 )
		{
			logSpam("\tRemoving DialogFactory \"%s\" \n",m_DialogueFactories.back()->getFactoryName().c_str());
			m_DialogueFactories.back()->socketClosed(this);
			m_DialogueFactories.pop_back();
		}
	}

	if( m_Dialogues.size() > 0 )
	{
		logSpam("%s clearing DialogueList (%i entries)\n",getDescription().c_str(), m_Dialogues.size());
		while( m_Dialogues.size() > 0 )
		{
			logSpam("\tRemoving Dialog \"%s\" \n",m_Dialogues.back()->getDialogueName().c_str());

			if (m_HighestConsumeLevel != CL_ASSIGN_AND_DONE )
			{
				m_Dialogues.back()->dump();
			}

			delete m_Dialogues.back();
			m_Dialogues.pop_back();
		}
	}
	Exit();

	SocketEvent sEvent(this,EV_SOCK_TCP_CLOSE);
	g_Nepenthes->getEventMgr()->handleEvent(&sEvent);
	
	
}

bool TCPSocket::bindPort()
{
	logPF();
	struct sockaddr_in addrBind;
	addrBind.sin_family = AF_INET;

	addrBind.sin_addr.s_addr = getLocalHost();
	addrBind.sin_port = htons(getLocalPort());


	if ( (m_Socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		logCrit("Error creating Socket %i to listen on TCP Port %i\n%s\n",m_Socket, m_LocalPort, strerror(errno));
		return false;
	}

#ifdef WIN32
	int32_t iMode = 0;
	ioctlsocket(m_Socket, FIONBIO, (u_long FAR*) &iMode);
#else
	fcntl(m_Socket, F_SETFL, O_NONBLOCK);
#endif

	int32_t x=1;

#ifdef WIN32
	if ( setsockopt(m_Socket,SOL_SOCKET,SO_REUSEADDR,(char *)&x,sizeof(x)) == -1 )
#else
	if ( setsockopt(m_Socket,SOL_SOCKET,SO_REUSEADDR, (void *)&x,sizeof(x)) == -1 )
#endif
	{
		logCrit("setsockopt() to SO_REUSEADDR failed\n%s\n",strerror(errno));
		return false;
	}

#ifdef HAVE_SO_NOSIGPIPE
	if(setsockopt(m_Socket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&x, sizeof(x)) < 0)
	{
		logCrit("setsockopt() to SO_NOSIGPIPE failed - everything will screw up on the long run!\n%s\n",strerror(errno));
		return false;
	}
#endif

	if ( bind(m_Socket, (struct sockaddr *) &addrBind, sizeof(addrBind)) < 0 )
	{
		logCrit("Could not Bind Socket to Port %i\n%s\n", m_LocalPort,strerror(errno));
		return false;
	}

	if ( listen(m_Socket, 16) < 0 )
	{
		logCrit("Unable to set listener to Port %i\n%s\n", m_LocalPort,strerror(errno));
		return false;
	}

	int32_t iSize = sizeof(addrBind);
	getsockname(m_Socket, (struct sockaddr *) &addrBind, (socklen_t *) &iSize);
	m_LocalPort = ntohs( ( (sockaddr_in *)&addrBind)->sin_port ) ;
	logDebug("Success binding Port %i\n", m_LocalPort);

    return true;
}

bool TCPSocket::Init()
{
	if(isBind())
	{
        if(!bindPort())
		{
			logCrit("ERROR Could not init Socket %s\n", strerror(errno));
			return false;
		}
		return true;
	}else
	if(isConnect())
	{
		if(!connectHost())
		{
			logCrit("ERROR Could not connect host %s\n",strerror(errno));
			return false;
		}
		return true;
	}
	return false;
}

bool TCPSocket::Exit()
{
	while(m_TxPackets.size() > 0)
	{
		delete m_TxPackets.front();
		m_TxPackets.pop_front();
	}

#ifdef WIN32
	closesocket(m_Socket);
#else
	close(m_Socket);
#endif
	return true;
}

bool TCPSocket::connectHost()
{
	logDebug("Connecting %s:%i \n",inet_ntoa(* (in_addr *)&m_RemoteHost), m_RemotePort);
	
	m_Socket=socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addrBind;
	addrBind.sin_family = AF_INET;

	addrBind.sin_addr.s_addr = getLocalHost();
	addrBind.sin_port = 0;

	if ( bind(m_Socket, (struct sockaddr *) &addrBind, sizeof(addrBind)) < 0 )
	{
		logCrit("Could not Bind Socket for connectHost %i\n%s\n", m_LocalPort,strerror(errno));
		return false;
	}

	int32_t iSize = sizeof(addrBind);
	getsockname(m_Socket, (struct sockaddr *) &addrBind, (socklen_t *) &iSize);
	m_LocalPort = ntohs( ( (sockaddr_in *)&addrBind)->sin_port ) ;
    

	

#ifdef WIN32
	int32_t iMode = 0;
	ioctlsocket(m_Socket, FIONBIO, (u_long FAR*) &iMode);
#else
	fcntl(m_Socket, F_SETFL, O_NONBLOCK);
#endif

	int32_t x=1;

#ifdef HAVE_SO_NOSIGPIPE
	if(setsockopt(m_Socket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&x, sizeof(x)) < 0)
		logCrit("setsockopt() to SO_NOSIGPIPE failed - everything will screw up on the long run!\n%s\n",strerror(errno));
#endif

	sockaddr_in ssin; 

	ssin.sin_family=AF_INET;
	ssin.sin_port=htons(m_RemotePort);
	ssin.sin_addr.s_addr=m_RemoteHost;
	
	m_LastAction = time(NULL);
	int32_t iRes=-1;

	iRes=connect(m_Socket, (sockaddr*)&ssin, sizeof(sockaddr_in));

	if ( iRes == -1 )
	{
#ifdef WIN32
		if (errno == WSAEINPROGRESS)
#else
		if (errno == EINPROGRESS)
#endif
		{
			return true;
		}else
		{
			return false;
		}
	}
	return true;
	
    
}

Socket * TCPSocket::acceptConnection()
{
	logPF();

	struct sockaddr_in addrNew;
	int32_t iSize = sizeof(addrNew);

	int32_t sock = accept(m_Socket, (struct sockaddr *) &addrNew, (socklen_t *) &iSize);

	if(sock < 0)
	{
		//logCrit( ... )
		return NULL;
	}

	int32_t RemotePort = ntohs( ( (sockaddr_in *)&addrNew)->sin_port ) ;
	uint32_t RemoteHost =  addrNew.sin_addr.s_addr;

	getsockname(sock, (struct sockaddr *) &addrNew, (socklen_t *) &iSize);
	uint32_t LocalHost =   addrNew.sin_addr.s_addr;

	Socket *socket = new TCPSocket(m_Nepenthes, sock ,LocalHost, m_LocalPort, RemoteHost, RemotePort, m_TimeoutIntervall);
	logSpam("%s \n",socket->getDescription().c_str());

	list <DialogueFactory *>::iterator diaf;
	for(diaf = m_DialogueFactories.begin();diaf != m_DialogueFactories.end(); diaf++)
	{
		logSpam("Adding Dialogue %s \n",(*diaf)->getFactoryName().c_str());
		Dialogue *dia = (*diaf)->createDialogue(socket);
		if (dia != NULL)
		{
        	socket->addDialogue(dia);
		}else
		{
			logWarn("%s returned NULL dialogue \n",(*diaf)->getFactoryName().c_str());
			socket->setStatus(SS_CLOSED);
		}
	}

	SocketEvent sEvent(socket,EV_SOCK_TCP_ACCEPT);
	m_Nepenthes->getEventMgr()->handleEvent(&sEvent);
	return socket;
}


bool TCPSocket::wantSend()
{
//	logSpam("%s \n",getDescription().c_str());
	if(m_TxPackets.size() > 0)
		return true;
	return false;
}


/**
 * this tries to send as much 'Packets' as possible from our private queue
 * if a packets is sended complete, it gets removed from the queue, 
 * else the packet is cut so we can send the missing stuff later
 * we pass the data we send into a Message and 
 * provided this information to our Dialogues::outgoingData
 * as we dont want to let Dialogues send during this read only session 
 * we set the socket CanSend false.
 * 
 * @return returns the number of successfully sended bytes
 */
int32_t TCPSocket::doSend()
{
	if(m_TxPackets.size() == 0)
		return -1;

	Packet *packet;
	bool sendon = true;
	uint32_t sumsended = 0;
	while(m_TxPackets.size() > 0 && sendon == true )
	{
		packet = m_TxPackets.front();
		int32_t onoff = 1;

#ifdef HAVE_MSG_NOSIGNAL
		int32_t sended = send(m_Socket,packet->getData(), packet->getLength(), MSG_NOSIGNAL);
#else
		int32_t sended = send(m_Socket,packet->getData(), packet->getLength(), 0);
#endif
		if(sended > 0)
		{
			sumsended += sended;
// create a msg and get it to our dialogues
// set the socket writing disabled

			Message Msg(packet->getData(),sended,m_LocalPort,m_RemotePort,m_LocalHost, m_RemoteHost,this,this);
			m_CanSend = false;
			list <Dialogue *>::iterator dia;
            for( dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++ )
			{
				if ((*dia)->getConsumeLevel() == CL_DROP)
				{
					continue;
				}

				logDebug("giving data to %s \n",(*dia)->getDialogueName().c_str());
				ConsumeLevel cl;
				cl = (*dia)->outgoingData(&Msg);
				(*dia)->setConsumeLevel(cl);
			}
			m_CanSend = true;

// check if we sended the full packet, cut the packet if we did not, else remove the packet from queue
			logDebug("sended %i from %i bytes \n",sended,(int32_t)packet->getLength());
			if(sended < (int32_t)packet->getLength())
			{
                packet->cut(sended);
				logDebug("cutted packet has size %i\n",(int32_t)packet->getLength());
				sendon = false;
			}else
			{
				delete m_TxPackets.front();
            	m_TxPackets.pop_front();
			}

		} else
		{

			switch(errno)
			{
#ifdef WIN32
			case WSAEWOULDBLOCK:
#else
			case EWOULDBLOCK:
#endif
				logDebug("Socket would Block '%s' \n",strerror(errno));
				sendon = false;
				break;

			case EPIPE:
				{

					logDebug("Socket has broken pipe '%s' \n",strerror(errno));
					Message Msg(NULL,0,m_LocalPort,m_RemotePort,m_LocalHost, m_RemoteHost,this,this);
					m_CanSend = false;
					list <Dialogue *>::iterator dia;
					for ( dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++ )
					{
						logDebug("Broken Pipe to %s \n",(*dia)->getDialogueName().c_str());
						ConsumeLevel cl;
						cl = (*dia)->connectionLost(&Msg);
						(*dia)->setConsumeLevel(cl);
					}
					m_Status = SS_CLOSED;
					return 0;
				}
				break;
			}
		}
	}
	logSpam("done sending %i bytes \n",sumsended);

	if (m_Status == SS_CLEANQUIT && m_TxPackets.size() == 0)
	{
		logSpam("%s\n\tsended last packet on socket, closing now\n",getDescription().c_str());
		m_Status = SS_CLOSED;
		Exit();
	}

	m_LastAction = time(NULL);
    return sumsended;
}


/**
 * this reads max 2048 bytes from a socket,
 * makes a Messages, and gets the Message to all registerd Dialogues ::incomingData
 * if a Dialogue returns CL_ASSIGN, the socket drops all dialogues in state C_UNSURE
 * CL_READONLY dialogues will be kept, so they can continue reading whats going on.
 * 
 * if we recv() returns 0 or -1, the conection got lost, we use the Dialogues ::connectionLost()
 * 
 * if a dialogue returns CL_ASSIGN on a lost connection
 * - maybe as he tries to reconnect the socket -
 * the socket does not get SS_CLOSED, the state keeps untouched
 * (f.e. if the Dialogue ran Init() on a connect socket, they new state will be kept)
 * 
 * else the socket gets status SS_CLOSED and will be removed by SocketManager::doLoop()
 * 
 * @return returns the length we could read from a socket
 */
int32_t TCPSocket::doRecv()
{
	logPF();
	char szBuffer[2048];
	memset(szBuffer,0,sizeof(char)*2048);
	int32_t length = recv(m_Socket, (char *) szBuffer, sizeof(szBuffer), 0);

	Message *Msg = new Message (szBuffer,length,m_LocalPort,m_RemotePort,m_LocalHost, m_RemoteHost,this,this);

	MessageEvent mEvent(Msg,EV_SOCK_TCP_RX);
    g_Nepenthes->getEventMgr()->handleEvent(&mEvent);

	logSpam("doRecv() %i\n",length);
	list <Dialogue *>::iterator dia, dib;

	for(dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++)
	{

		if ((*dia)->getConsumeLevel() == CL_DROP)
			continue;

		if((*dia)->getConsumeLevel() == CL_READONLY || (*dia)->getConsumeLevel() == CL_ASSIGN_AND_DONE )
			m_CanSend = false;

		ConsumeLevel cl = CL_UNSURE;


		if(length > 0)		// we actually received something
		{
			cl = (*dia)->incomingData(Msg);
		} else
		if(length == 0)		// connection was closed properly
		{
			cl = (*dia)->connectionShutdown(Msg);
		} else				// connection broke
		{
			cl = (*dia)->connectionLost(Msg);
		}

	    if (cl > m_HighestConsumeLevel )
		{
			m_HighestConsumeLevel = cl;
		}

		switch (cl)
		{
		case CL_ASSIGN_AND_DONE:

			for(dib = m_Dialogues.begin(); dib != m_Dialogues.end(); dib++)
			{
				if ((*dib)->getConsumeLevel() != CL_READONLY && *dib != *dia)
				{
					logDebug("setting Dialogue %s inactive as %s did it already\n",(*dib)->getDialogueName().c_str(),(*dia)->getDialogueName().c_str());
                	(*dib)->setConsumeLevel(CL_DROP);
				}
			}
			break;

		case CL_DROP:
			logDebug("Dialogue %s inactive, returned CL_DROP\n",(*dia)->getDialogueName().c_str());
			break;

		default:
			break;
		}

		(*dia)->setConsumeLevel(cl);
		m_CanSend = true;
	}

	delete Msg;

	uint16_t activeDialogues=0;
	for ( dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++ )
	{
		switch((*dia)->getConsumeLevel())
		{

		case CL_ASSIGN:
		case CL_ASSIGN_AND_DONE:
		case CL_UNSURE:
			activeDialogues++;
			break;

		default:
			break;
		}
	}

	if ( activeDialogues == 0 )
	{
		logDebug("%s\n has no active Dialogues left, closing \n",getDescription().c_str());
    	m_Status = SS_CLOSED;
	}

	m_LastAction = time(NULL);

	if((length == 0 || ( length == -1 && errno != EAGAIN )) && activeDialogues == 0)
	{
		logDebug("Connection %s CLOSED \n",getDescription().c_str());
		m_Status = SS_CLOSED;
	}
	return length;
}

/**
 * write a msg to the socket
 * this will store the msg and its size in a "Packet" and put in into our queue
 * 
 * @param msg    the message to send
 * @param len    the messages len
 * 
 * @return returns the queues size if the socket is allowed to send at this point of time, else -1
 */
int32_t TCPSocket::doWrite(char *msg, uint32_t len)
{
	logPF();
	if (m_CanSend == false)
	{
		logCrit("%s","Some read only attached Module wants to write on a Socket\n");
		return -1;
	}
	Packet *packet = new Packet(msg,len);
	m_TxPackets.push_back(packet);
	return m_TxPackets.size();
}



bool TCPSocket::checkTimeout()
{
	if ( isBind() )
	{
		if ( m_BindTimeoutIntervall != 0 )
		{

			if ( time(NULL) - m_LastAction > m_BindTimeoutIntervall )
			{
				if ( handleTimeout() == false )
				{
                	setStatus(SS_TIMEOUT);
					return false;
				}else
					return true;
			}
		}
	} else
	{
		if ( m_TimeoutIntervall != 0 )
		{

			if ( time(NULL) - m_LastAction > m_TimeoutIntervall )
			{
				if ( handleTimeout() == false )
				{
                	setStatus(SS_TIMEOUT);
					return false;
				}else
					return true;
			}
		}
	}
	return false;
}

bool TCPSocket::handleTimeout()
{
	bool bAssigned=false;

	Message Msg(m_LocalPort,m_RemotePort,m_LocalHost, m_RemoteHost,this,this);

	list <Dialogue *>::iterator dia;
	for ( dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++ )
	{
		if ( (*dia)->getConsumeLevel() == CL_READONLY )
			m_CanSend = false;

		ConsumeLevel cl;
		if ( (cl = (*dia)->handleTimeout(&Msg)) == CL_ASSIGN )
			bAssigned = true;
        
		(*dia)->setConsumeLevel(cl);
		
		m_CanSend = true;
	}

	if (bAssigned == false)
		return false;
	else
		return true;
}


bool TCPSocket::doRespond(char *msg, uint32_t len)
{
	logPF();
	if (doWrite(msg, len) > 0)
		return true;
	else
		return false;
}

