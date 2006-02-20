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

#include <errno.h>
#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#endif

#include <errno.h>


#include "UDPSocket.hpp"
#include "DialogueFactory.hpp"
#include "Packet.hpp"
#include "Message.hpp"
#include "Dialogue.hpp"
#include "Nepenthes.hpp"

#include "LogManager.hpp"


using namespace nepenthes;


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_net | l_hlr


UDPSocket::UDPSocket(Nepenthes *nepenthes,uint32_t localhost, uint32_t remotehost, uint16_t remoteport, time_t connectiontimeout)
{
	m_Nepenthes = nepenthes;
	setLocalPort(0);
	setLocalHost(localhost);
	setRemoteHost(remotehost);
	setRemotePort(remoteport);

	m_TimeoutIntervall = connectiontimeout;
	m_LastAction = time(NULL);

	m_Type = ST_UDP|ST_CONNECT;

	m_CanSend = true;
	m_Status = SS_CONNECTED;
	m_Polled = false;
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
UDPSocket::UDPSocket(Nepenthes *nepenthes, uint32_t localhost, uint16_t port, time_t bindtimeout, time_t accepttimeout)
{
	setLocalHost(localhost);
	setLocalPort(port);

	setRemoteHost(inet_addr("0.0.0.0"));
	setRemotePort(0);

	m_BindTimeoutIntervall = bindtimeout;
	m_TimeoutIntervall = accepttimeout;
	m_LastAction = time(NULL);

	m_Type = ST_UDP | ST_BIND;

	m_CanSend = true;
	m_Status = SS_CONNECTED;
	m_Polled = false;
	m_Nepenthes = nepenthes;
}


UDPSocket::~UDPSocket()
{
	logPF();
	if( m_DialogueFactories.size() > 0 )
	{
		logSpam("%s clearing DialogueFactory List (%i entries) \n",getDescription().c_str(),m_DialogueFactories.size());

		while( m_DialogueFactories.size() > 0 )
		{
			logSpam("\tRemoving DialogFactory \"%s\" \n",m_DialogueFactories.back()->getFactoryName().c_str());
			m_DialogueFactories.pop_back();
		}
	}

	if( m_Dialogues.size() > 0 )
	{
		logSpam("%s clearing DialogueList (%i entries)\n",getDescription().c_str(), m_Dialogues.size());
		while( m_Dialogues.size() > 0 )
		{
			logSpam("\tRemoving Dialog \"%s\" \n",m_Dialogues.back()->getDialogueName().c_str());
			delete m_Dialogues.back();
			m_Dialogues.pop_back();
		}
	}
	Exit();
}


bool UDPSocket::bindPort()
{
	struct sockaddr_in addrBind;
	addrBind.sin_family = AF_INET;

	addrBind.sin_addr.s_addr = getLocalHost();
	addrBind.sin_port = htons(getLocalPort());


	if ( (m_Socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		logCrit("Error creating Socket to listen on UDP Port %i\n%s\n",m_LocalPort, strerror(errno));
		return false;
	}


	int32_t x=1;
#ifdef WIN32
	if ( setsockopt(m_Socket,SOL_SOCKET,SO_REUSEADDR,(char *)&x,sizeof(x)) == -1 )
#else
	if ( setsockopt(m_Socket,SOL_SOCKET,SO_REUSEADDR,&x,sizeof(x)) == -1 )
#endif
	{
		logCrit("setsockopt() to SO_REUSEADDR failed\n%s\n",strerror(errno));
		return false;
	}


	if ( bind(m_Socket, (struct sockaddr *) &addrBind, sizeof(addrBind)) < 0 )
	{
		logCrit("Could not Bind Socket to Port %i\n%s\n", m_LocalPort,strerror(errno));
		return false;
	}

	return true;
}

bool UDPSocket::Init()
{
	logPF();
	logSpam("%s\n",getDescription().c_str());
	if(isConnect())
	{
		
		return connectHost();
	}else
	if(isBind())
	{
		return bindPort();
	}

	return true;
}

bool UDPSocket::Exit()
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

bool UDPSocket::connectHost()
{
	logInfo("UDP 'connecting' %s:%i \n",inet_ntoa(* (in_addr *)&m_RemoteHost), m_RemotePort);
	
	m_Socket=socket(AF_INET, SOCK_DGRAM, 0);

#ifdef WIN32
	int32_t iMode = 0;
	ioctlsocket(m_Socket, FIONBIO, (u_long FAR*) &iMode);
#else
	fcntl(m_Socket, F_SETFL, O_NONBLOCK);
#endif


	sockaddr_in ssin; 

	ssin.sin_family=AF_INET;
	ssin.sin_port=htons(m_RemotePort);
	ssin.sin_addr.s_addr=m_RemoteHost;
    m_LastAction = time(NULL);

	if(m_Socket > 0)
		return true;
	logCrit("Error creating Socket %s \n",strerror(errno));
	return false;
}

Socket * UDPSocket::acceptConnection()
{
	return NULL;
}

bool UDPSocket::wantSend()
{
	if(m_TxPackets.size() > 0)
		return true;
	return false;
}


int32_t UDPSocket::doSend()
{
	struct sockaddr_in addrRemote;

	addrRemote.sin_family = AF_INET;
//	addrRemote.sin_port = htons(m_RemotePort);
//	addrRemote.sin_addr.s_addr = m_RemoteHost;

	while (m_TxPackets.size() > 0)
	{
		addrRemote.sin_port = htons((uint16_t)(*m_TxPackets.begin())->getPort());
		addrRemote.sin_addr.s_addr = (uint32_t)(*m_TxPackets.begin())->getHost();


		char *pszData = (char *)(*m_TxPackets.begin())->getData();
		size_t len = (*m_TxPackets.begin())->getSize();
//		logSpam("Sending %d %d bytes '%4x'\n",m_TxPackets.size(),len,(uint32_t)pszData);
		if ( sendto(m_Socket,pszData,len,0, (struct sockaddr *)  &addrRemote, sizeof(addrRemote)) == -1 )
		{

#ifdef WIN32
			if (errno != WSAEWOULDBLOCK)
#else
			if (errno != EWOULDBLOCK)
#endif
			{
				m_Status = SS_CLOSED;
            	return -1; // FIXME
			}else
			{
				return 0;
			}
				
		}
		delete (*m_TxPackets.begin());
		m_TxPackets.pop_front();
		m_LastAction = time(NULL);
	}
    return 0;
}

int32_t UDPSocket::doRecv()
{
	char szBuffer[2048];
	struct sockaddr_in addrRemote;
	int32_t iSize = sizeof(addrRemote);

	int32_t iLength = recvfrom(m_Socket, (char *) szBuffer, 2048, 0, (struct sockaddr *) &addrRemote, (socklen_t *) &iSize);

	setRemotePort(ntohs(((sockaddr_in *)&addrRemote)->sin_port));
	

	Message *Msg = new Message (szBuffer,iLength,m_LocalPort,m_RemotePort,m_LocalHost,(uint32_t) addrRemote.sin_addr.s_addr,this,this);

//	logSpam("Recv() %i '%s'\n",iLength,"foo");//szBuffer);

	if ( isBind() )
	{
		 list <DialogueFactory *>::iterator diaf;
		 for(diaf = m_DialogueFactories.begin();diaf != m_DialogueFactories.end(); diaf++)
		 {
			 logSpam("Adding Dialogue %s \n",(*diaf)->getFactoryName().c_str());
			 Dialogue *dia = (*diaf)->createDialogue(this);
			 if (dia != NULL)
			 {
				 addDialogue(dia);
			 }else
			 {
				 logWarn("%s returned NULL dialogue \n",(*diaf)->getFactoryName().c_str());
//				 socket->setStatus(SS_CLOSED);
			 }
		 }

	} 
	{
		list <Dialogue *>::iterator dia;
		bool bAssigned=false;

		for ( dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++ )
		{
			if ( (*dia)->getConsumeLevel() == CL_READONLY )
				m_CanSend = false;

			ConsumeLevel cl;
			if ( iLength > 0 )
			{
				if ( (cl = (*dia)->incomingData(Msg)) == CL_ASSIGN )
					bAssigned = true;
			} else
			{
				if ( (cl = (*dia)->connectionLost(Msg)) == CL_ASSIGN )
					bAssigned = true;
			}
			(*dia)->setConsumeLevel(cl);
			m_CanSend = true;
		}

		delete Msg;

		for ( dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++ )
		{
			if ( (bAssigned == true && (*dia)->getConsumeLevel() == CL_UNSURE ) || (*dia)->getConsumeLevel() == CL_DROP )
			{
				logSpam("%s removing Dialog %s as Dialogue returned CL_DROP \n",getDescription().c_str(),(*dia)->getDialogueName().c_str());
				Dialogue *deldia = *dia;
				m_Dialogues.erase(dia);
				delete deldia;
				dia = m_Dialogues.begin();
			}
		}

		m_LastAction = time(NULL);

		if ( (iLength == 0 || ( iLength == -1 && errno != EAGAIN )) && bAssigned == false )
		{
			logInfo("Connection %s CLOSED \n",getDescription().c_str());
			m_Status = SS_CLOSED;
		}
	}
	return iLength;
}

int32_t UDPSocket::doWrite(char *msg, uint32_t len)
{
//	logPF();
	if (m_CanSend == false)
	{
		logCrit("%s","Some read only attached Module wants to write on a Socket\n");
		return -1;
	}
	UDPPacket *packet = new UDPPacket(getRemoteHost(),getRemotePort(),msg,len);
	m_TxPackets.push_back(packet);
	return m_TxPackets.size();
}

int32_t UDPSocket::doWriteTo(uint32_t ip, uint16_t port, char *msg, uint32_t len)
{
//	logPF();
	if (m_CanSend == false)
	{
		logCrit("%s","Some read only attached Module wants to write on a Socket\n");
		return -1;
	}
	UDPPacket *packet = new UDPPacket(ip,port,msg,len);
	m_TxPackets.push_back(packet);
	return m_TxPackets.size();
}


bool UDPSocket::checkTimeout()
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

bool UDPSocket::handleTimeout()
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


bool UDPSocket::doRespond(char *msg, uint32_t len)
{
	return doWrite(msg,len);
}

