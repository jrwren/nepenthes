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

#include <sstream>

#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
//#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)
#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif

#include <errno.h>

#include <fcntl.h>



#include "RAWSocket.hpp"
#include "DialogueFactory.hpp"
#include "Packet.hpp"
#include "Message.hpp"
#include "Dialogue.hpp"
#include "Nepenthes.hpp"
#include "SocketEvent.hpp"
#include "EventManager.hpp"
#include "Nepenthes.hpp"

#include "LogManager.hpp"

#include "Utilities.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_net | l_hlr


/**
 */
RAWSocketListener::RAWSocketListener(Nepenthes *nepenthes, char *nicinterface, unsigned long localhost, unsigned int protocoll)
{
	setLocalHost(localhost);
	setLocalPort(0);

	setRemoteHost(inet_addr("0.0.0.0"));
	setRemotePort(0);

	m_BindTimeoutIntervall = 0;
	m_TimeoutIntervall = 30;
	m_LastAction = time(NULL);

	m_Type = ST_RAW | ST_ACCEPT;

	m_Protocoll = protocoll;

	m_CanSend = true;
	m_Status = SS_NULL;
	m_Polled = false;
	m_Nepenthes = nepenthes;

	m_Interface = nicinterface;
}

RAWSocketListener::RAWSocketListener(Nepenthes *nepenthes, unsigned long localhost)
{
	setLocalHost(localhost);
	setLocalPort(10000);

	setRemoteHost(inet_addr("0.0.0.0"));
	setRemotePort(0);

	m_BindTimeoutIntervall = 0;
	m_TimeoutIntervall = 30;
	m_LastAction = time(NULL);

	m_Type = ST_RAW | ST_ACCEPT;

	m_CanSend = true;
	m_Status = SS_NULL;
	m_Polled = false;
	m_Nepenthes = nepenthes;

	m_Interface = "Not Yet";
}


RAWSocketListener::~RAWSocketListener()
{
	logPF();
	Exit();
}

bool RAWSocketListener::bindPort()
{
	struct sockaddr_in addrBind;
	addrBind.sin_family = AF_INET;

	addrBind.sin_addr.s_addr = getLocalHost();
	addrBind.sin_port = htons(12);

#ifdef WIN32
	m_Socket = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
#else
	m_Socket = socket(PF_INET,SOCK_RAW,IPPROTO_UDP);
#endif
//    m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket < 0 )
	{
		logCrit("Could not open raw socket for interface %s \n%s\n",m_Interface.c_str(),strerror(errno));
		return false;
	}else
    {
        logInfo("Socket is %i\n",m_Socket);
    }

#ifdef WIN32

   BOOL    bIsTrue        = TRUE;
   if(setsockopt(m_Socket,IPPROTO_IP,IP_HDRINCL, (char*) 
        &bIsTrue, sizeof(bIsTrue)) == SOCKET_ERROR)
    {
       logCrit("%s","Unable to set Header Included flag.\n");        
       return false;
    }


    if (bind(m_Socket, (struct sockaddr *)&addrBind, sizeof(addrBind)) == SOCKET_ERROR)
    {
        errno = WSAGetLastError();
        logCrit("Could not bind RAWSocket on to ip %s \n",inet_ntoa(*(struct in_addr *)&m_LocalHost));
        closesocket(m_Socket);
        return false;
    }else
    {
        logInfo("Got RAWSocket for ip %s \n",inet_ntoa(*(struct in_addr *)&m_LocalHost));
    }
    unsigned long crap=0;
    int optval = 1;
    DWORD dwBytesRet;
    if ( WSAIoctl(
        m_Socket, 
        SIO_RCVALL, 
        &optval, 
        sizeof(optval), 
        NULL, 
        0, 
        &dwBytesRet, 
        NULL, 
        NULL
       ) 
         == SOCKET_ERROR )
    {
        logCrit("WSAIoctl failed %i \n",WSAGetLastError());
        return false;
    }


#else
	struct ifreq __ifreq_;
	strncpy(__ifreq_.ifr_name,m_Interface.c_str(),m_Interface.size()+4);

	if ( (ioctl(m_Socket,SIOCGIFFLAGS,&__ifreq_))==-1 )
	{
		logCrit("I can't sniff this interface %s!\n", m_Interface.c_str());
		return false;
	}

    if ( (ioctl(m_Socket,SIOCSIFFLAGS,&__ifreq_))==-1 )
	{
		logCrit("Can't set promisc mode for interface %s\n",m_Interface.c_str());
		return false;
	}

	logInfo("PROMISC set for interface %s\n",m_Interface.c_str());
//	m_Type = ST_RAW | ST_ACCEPT;
#endif
    return true;
}

int RAWSocketListener::getSocket()
{
    return m_Socket;
}

bool RAWSocketListener::Init()
{
	if(isAccept())
	{
        if(!bindPort())
		{
			logCrit("ERROR Could not init Socket %s\n", strerror(errno));
			return false;
		}
		return true;
	}else
	return false;
}

bool RAWSocketListener::Exit()
{
	if(isAccept())
	{
#ifdef WIN32
		closesocket(m_Socket);
#else
		close(m_Socket);
#endif
	}
	return true;
}


Socket * RAWSocketListener::acceptConnection()
{
	logPF();
	return NULL;
}

bool RAWSocketListener::connectHost()
{
	logPF();
	return false;
}

bool RAWSocketListener::wantSend()
{
//	logPF();
	return false;
}

int RAWSocketListener::doSend()
{
	logPF();
    return 0;
}


int RAWSocketListener::doRecv()
{
//	logPF();
//	logSpam("RAWSocketReader %s %i \n",m_Interface.c_str(),m_Protocoll);
	struct sockaddr servaddr;
    memset(&servaddr,0,sizeof(servaddr));
    char buf[70000];
	memset(buf,0,70000);
#ifdef WIN32
	int p=sizeof(servaddr);
    int i=recvfrom(m_Socket,buf,70000,0,&servaddr,&p);
#else
	socklen_t p =sizeof(servaddr);
	int i=recvfrom(m_Socket,buf,70000,0,&servaddr,&p);
#endif
    if (i <= 0 )
    {
#ifdef WIN32
        static char Message[1024];

        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                      FORMAT_MESSAGE_MAX_WIDTH_MASK,
                      NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPSTR)Message, 1024, NULL);
        
        logCrit("recv() was -1: %i %s \n",WSAGetLastError(),Message);
        WSASetLastError(0);
#else
		logCrit("RAWSocket got -1 bytes: %s\n",strerror(errno));
#endif
        return 0;
    }else
    {
//        logInfo("got %i bytes via raw socket \n",i);
    }

	struct iphdr *ip = (iphdr *)buf;
	struct tcphdr *tcp;
	struct udphdr *udp;
	char *payload;
	unsigned int payloadsize;

	if ( ip->ip_version == 4 )
	{

		unsigned int localhost = getLocalHost();
		unsigned int remotehost;
		if (ip->ip_source == localhost)
		{
			remotehost = ip->ip_dest;
		}else
		if (ip->ip_dest == localhost)
		{
			remotehost = ip->ip_source;
		}else
		{
			logSpam("dropping packet as it does not match our host %s \n",inet_ntoa(*(in_addr *)&localhost));
			return 0;
		}
        

		switch( ip->ip_protocol )
		{
        case 1:
            logInfo("%s","ICMP Packet \n");
            break;

        case 2:
            logInfo("%s","IGMP Packet \n");
            break;


        case 6: // TCP
            {
//				return 0;
				logInfo("%s","TCP Packet \n");

                tcp = (struct tcphdr *)((char *)ip + ip->ip_length * 4);
                payloadsize = ntohs(ip->ip_total_length) - ip->ip_length*4 - tcp->tcp_hlen*4;

                if ( payloadsize == 0 )
                {
					logInfo("payload is %i\n",payloadsize);
                    return 0;
                }
                payload = (char *)((char *)ip + ip->ip_length * 4 + tcp->tcp_hlen*4);


                unsigned short localport = ntohs(tcp->tcp_dest_port);
                unsigned short remoteport = ntohs(tcp->tcp_source_port);


                // drop packets to ports we dont need
                list<ListenDialogueFactoryMap *>::iterator jt;
                bool listenport = false;
                for ( jt=m_ListenFactories.begin();jt!=m_ListenFactories.end();jt++ )
                {
					logInfo("ListenFactory %i i %i %i\n",(*jt)->m_LocalPort, (*jt)->m_RemotePort, (*jt)->m_Protocoll, (*jt)->m_DialogueFactories.size());
                    if ( 
						((*jt)->m_LocalPort == 0   || (*jt)->m_LocalPort == localport ) 	&&
						((*jt)->m_RemotePort == 0  || (*jt)->m_RemotePort == remoteport ) && 
						(*jt)->m_Protocoll == IPPROTO_TCP )
                    {
                        listenport = true;
						break;
                    }
                }

                if ( listenport == false )
					return 0;
				else
					logSpam(" m_Listen.. %i \n",m_ListenFactories.size());

                printf("\t %-15s:%i -> ",inet_ntoa(*(in_addr *)&remotehost),remoteport);
                printf("%-15s:%i \n",inet_ntoa(*(in_addr *)&localhost),localport);
//                g_Nepenthes->getUtilities()->hexdump((byte *)payload,payloadsize);


                list <RAWSocketReader *>::iterator it;
                bool match = false;
                for ( it=m_Sockets.begin();it!=m_Sockets.end();it++ )
                {
                    if ( (*it)->getRemoteHost() == remotehost &&
                         (*it)->getLocalHost() == localhost && 
                         (*it)->getRemotePort() == remoteport &&
                         (*it)->getLocalPort() == localport ) // FIXME add protocoll
                    {
                        logInfo("Found Matching session \n%s\n",(*it)->getDescription().c_str());
                        match = true;
                        break;
                    }
                }

                RAWSocketReader *socket;

                if ( match == true )
                {
                    socket = (*it);
                    (*it)->doRead(payload,payloadsize);
                    if ( socket->getStatus() != SS_NULL )
                    {
                        m_Sockets.erase(it);
                        delete socket;
                    }

                } else
                {
                    socket = new RAWSocketReader(m_Nepenthes,localhost,localport,remotehost,remoteport,30, IPPROTO_TCP);

					list <DialogueFactory *>::iterator diaff;
                    for(diaff = (*jt)->m_DialogueFactories.begin();diaff != (*jt)->m_DialogueFactories.end(); diaff++)
					{
						socket->addDialogue((*diaff)->createDialogue(socket));
					}


                    socket->doRead(payload,payloadsize);
                    if ( socket->getStatus() != SS_NULL )
                    {
                        delete socket;
                    } else
                    {
                        m_Sockets.push_back(socket);
                    }
                }

            }
            break;

        case 17:   // UDP
			{
//				logInfo("%s","UDP Packet \n");

				udp = (struct udphdr *)((char *)ip + ip->ip_length * 4);
				payloadsize = ntohs(ip->ip_total_length) - ip->ip_length*4 - 8;

				if ( payloadsize == 0 )
				{
					logInfo("payload is %i\n",payloadsize);
					return 0;
				}
				payload = (char *)((char *)ip + ip->ip_length * 4 + 8);
				unsigned int localhost = ip->ip_dest;
				unsigned int remotehost = ip->ip_source;

				unsigned short localport = ntohs(udp->udp_dest_port);
				unsigned short remoteport = ntohs(udp->udp_source_port);


				// drop packets to ports we dont need
				list<ListenDialogueFactoryMap *>::iterator jt;
				bool listenport = false;
				for ( jt=m_ListenFactories.begin();jt!=m_ListenFactories.end();jt++ )
				{
//					logInfo("ListenFactory %i %i %i\n",(*jt)->m_Port, (*jt)->m_Protocoll, (*jt)->m_DialogueFactories.size());
					if ( 
						((*jt)->m_LocalPort == 0   || (*jt)->m_LocalPort == localport ) 	&&
						((*jt)->m_RemotePort == 0  || (*jt)->m_RemotePort == remoteport ) && 
						(*jt)->m_Protocoll == IPPROTO_UDP )
					{
						listenport = true;
						break;
					}
				}

				if ( listenport == false )
					return 0;
//				else
//					logSpam(" m_Listen.. %i \n",m_ListenFactories.size());

				printf("\t %-15s:%i -> ",inet_ntoa(*(in_addr *)&remotehost),remoteport);
				printf("%-15s:%i \n",inet_ntoa(*(in_addr *)&localhost),localport);
                g_Nepenthes->getUtilities()->hexdump((byte *)payload,payloadsize);


				list <RAWSocketReader *>::iterator it;
				bool match = false;
				logSpam("Debug %i \n",m_Sockets.size());
				for ( it=m_Sockets.begin();it!=m_Sockets.end();it++ )
				{
					logSpam("debug %s \n",(*it)->getDescription().c_str());
					if ( (*it)->getRemoteHost() == remotehost &&
						 (*it)->getLocalHost() == localhost && 
						 (*it)->getRemotePort() == remoteport &&
						 (*it)->getLocalPort() == localport ) // FIXME add protocoll
					{
						logInfo("Found Matching session \n%s\n",(*it)->getDescription().c_str());
						match = true;
						break;
					}
				}

				RAWSocketReader *socket;

				if ( match == true )
				{
					socket = (*it);
					(*it)->doRead(payload,payloadsize);
					if ( socket->getStatus() != SS_NULL )
					{
						m_Sockets.erase(it);
						delete socket;
					}

				} else
				{
					socket = new RAWSocketReader(m_Nepenthes,localhost,localport,remotehost,remoteport,30, IPPROTO_UDP);

					list <DialogueFactory *>::iterator diaff;
					for(diaff = (*jt)->m_DialogueFactories.begin();diaff != (*jt)->m_DialogueFactories.end(); diaff++)
					{
						socket->addDialogue((*diaff)->createDialogue(socket));
					}


					socket->doRead(payload,payloadsize);
					if ( socket->getStatus() != SS_NULL )
					{
						logSpam("NOT Adding RAWSocketReader %i\n",m_Sockets.size());
						delete socket;
					} else
					{
						m_Sockets.push_back(socket);
						logSpam("Adding RAWSocketReader %i\n",m_Sockets.size());
					}
				}

			}

            break;

        default:
            logInfo("hrm, protokoll? %x\n",ip->ip_protocol);
            break;
        }
    }else
	{
		logSpam("Unknown Protokoll %i\n",i);
		g_Nepenthes->getUtilities()->hexdump((byte *)buf,i);
	}
//	logInfo("got %i bytes data from raw socket \n",i);
	return 0;
}


int RAWSocketListener::doWrite(char *msg, unsigned int len)
{
	logPF();
	return 0;
}



bool RAWSocketListener::checkTimeout()
{
	list <RAWSocketReader *>::iterator it;


	for ( it=m_Sockets.begin();it!=m_Sockets.end();it++ )
	{
		if ((*it)->checkTimeout() == true)
		{
			(*it)->setStatus(SS_CLOSED);
		}
	}

	for ( it=m_Sockets.begin();it!=m_Sockets.end();it++ )
	{
		
		if ( (*it)->getStatus() == SS_CLOSED )
		{
			logCrit("%s \n has a timeout\n",(*it)->getDescription().c_str());
			Socket *delme = *it;
			m_Sockets.erase(it);
			delete delme;
			it=m_Sockets.begin();
		}
	}

	return false;
}

bool RAWSocketListener::handleTimeout()
{
	return false;
}


bool RAWSocketListener::doRespond(char *msg, unsigned int len)
{
	return false;
}

string RAWSocketListener::getDescription()
{
	string sDesc="Socket ";
	sDesc += "RAWListener ";

	sDesc += " (" + m_Interface + ") ";
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




bool RAWSocketListener::addListenPort(unsigned int port)
{
	m_ListenPorts.push_back(port);
	return true;
}

bool RAWSocketListener::addListenFactory(unsigned int localport, unsigned int remoteport, unsigned short protocoll, DialogueFactory *diaf)
{
	logPF();
	list<ListenDialogueFactoryMap *>::iterator it;
	for (it=m_ListenFactories.begin();it!=m_ListenFactories.end();it++)
	{
		if ( ((*it)->m_LocalPort == 0 || localport == 0 || (*it)->m_LocalPort == localport ) && 
			 ((*it)->m_RemotePort == 0 || remoteport == 0 || (*it)->m_RemotePort == remoteport )
			   && (*it)->m_Protocoll == protocoll)
		{
			(*it)->m_DialogueFactories.push_back(diaf);
			return true;
		}
	}

	ListenDialogueFactoryMap *ldia = new ListenDialogueFactoryMap;
	ldia->m_DialogueFactories.push_back(diaf);
	ldia->m_LocalPort = localport;
	ldia->m_RemotePort = remoteport;
	ldia->m_Protocoll = protocoll;
	m_ListenFactories.push_back(ldia);
	return true;
}

//////////////////////////////////////////////////
// 
// 
// 



RAWSocketReader::RAWSocketReader(Nepenthes *nepenthes,unsigned long localhost,
								 unsigned short localport,unsigned long remotehost,
								 unsigned short remoteport, time_t connecttimeout, 
								 unsigned int protocoll)
{
	logPF();
	
	setLocalHost(localhost);
	setLocalPort(localport);
	setRemoteHost(remotehost);
	setRemotePort(remoteport);

	m_Type = ST_RAW | ST_CONNECT;
	m_Status = SS_NULL;

	logSpam("%s\n",getDescription().c_str());

	m_LastAction = time(NULL);

	m_TimeoutIntervall = 30;

	switch(protocoll)
	{
	case IPPROTO_TCP:
		m_Type |= ST_RAW_TCP;
		break;

	case IPPROTO_UDP:
		logInfo("Creating UDP Reader %i\n",protocoll);
		m_Type |= ST_RAW_UDP;
		break;
	}
}

RAWSocketReader::~RAWSocketReader()
{
	logPF();
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

bool RAWSocketReader::bindPort()
{
    return true;
}

bool RAWSocketReader::Init()
{
	return true;
}

bool RAWSocketReader::Exit()
{
	return true;
}


Socket * RAWSocketReader::acceptConnection()
{
	logPF();
	return NULL;
}

bool RAWSocketReader::connectHost()
{
	logPF();
	return false;
}

bool RAWSocketReader::wantSend()
{
	logPF();
	return false;
}



int RAWSocketReader::doSend()
{
    return 0;
}


int RAWSocketReader::doRecv()
{
	return 0;
}


socket_state RAWSocketReader::doRead(char *msg,unsigned int len)
{
	logPF();
//	g_Nepenthes->getUtilities()->hexdump((byte *)msg,len);
	m_LastAction = time(NULL);

	Message *Msg = new Message (msg,len,m_LocalPort,m_RemotePort,m_LocalHost, m_RemoteHost,this,this);

	logSpam("doRead(..) %i\n",len);
	list <Dialogue *>::iterator dia;
	bool bAssigned=false;

	for(dia = m_Dialogues.begin(); dia != m_Dialogues.end(); dia++)
	{
		if((*dia)->getConsumeLevel() == CL_READONLY)
			m_CanSend = false;

		ConsumeLevel cl;
		if(len > 0)
		{
			if( (cl = (*dia)->incomingData(Msg)) == CL_ASSIGN )
				bAssigned = true;
		} else
		if(len == 0)
		{
			if( (cl = (*dia)->connectionShutdown(Msg)) == CL_ASSIGN )
				bAssigned = true;

		} else
		{
			if( (cl = (*dia)->connectionLost(Msg)) == CL_ASSIGN )
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
			m_Dialogues.erase(dia);
			delete *dia;
			dia = m_Dialogues.begin();
		}
	}

	if (m_Dialogues.size() == 0)
	{
		logInfo("%s\n has no Dialogues left, closing \n",getDescription().c_str());
    	m_Status = SS_CLOSED;
	}

	m_LastAction = time(NULL);

/*	if((len ==  ))//|| ( len == -1 && errno != EAGAIN )) && bAssigned == false)
	{
		logInfo("Connection %s CLOSED \n",getDescription().c_str());
		m_Status = SS_CLOSED;
	}
*/
	return m_Status;
}


int RAWSocketReader::doWrite(char *msg, unsigned int len)
{
	return 0;
}



bool RAWSocketReader::checkTimeout()
{
	if ( time(NULL) - m_LastAction > m_TimeoutIntervall )
	{
		return true;
	}else
	{
		return false;
	}
	
}

bool RAWSocketReader::handleTimeout()
{
	return false;
}


bool RAWSocketReader::doRespond(char *msg, unsigned int len)
{
	return false;
}

string RAWSocketReader::getDescription()
{
	string sDesc="Socket ";
	sDesc += "RAWReader ";

	sDesc += " (read) ";
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

	return sDesc;
}
