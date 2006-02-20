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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CTRLDialogue.hpp"
#include "FILEDialogue.hpp"

#include "FTPContext.hpp"

#include "download-ftp.hpp"

#include "Message.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Download.hpp"
#include "Download.cpp"
#include "DownloadUrl.hpp"
#include "DownloadUrl.cpp"

#include "DownloadBuffer.hpp"
#include "DownloadBuffer.cpp"



#include "Buffer.hpp"
#include "Buffer.cpp"

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the CTRLDialogue, creates a new CTRLDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
CTRLDialogue::CTRLDialogue(Socket *socket, Download *down)
{
	m_Socket = socket;
    m_DialogueName = "CTRLDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	char *msg;
	asprintf(&msg,"USER %s\r\n",down->getDownloadUrl()->getUser().c_str());
	logInfo("FTPSEND: '%s'\n",msg);
	m_Socket->doRespond(msg,strlen(msg));
	free(msg);

	m_Download = down;

	m_State = FTP_USER;

	m_Buffer = new Buffer(128);
}

CTRLDialogue::~CTRLDialogue()
{
	if (m_Download != NULL)
	{
    	delete m_Download;
		m_Download = NULL;
	}
	delete m_Buffer;

	g_FTPDownloadHandler->removeContext(m_Context);
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * a small and ugly shell where we can use
 * "download protocol://localction:port/path/to/file
 * to trigger a download
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel CTRLDialogue::incomingData(Message *msg)
{
//	logInfo("RX:\n%.*s\n",msg->getMsgLen(),msg->getMsg());
	if (m_Download == NULL )
	{
		logWarn("%s","broken ftp daemon \n");
		return CL_DROP;
	}

	m_Buffer->add(msg->getMsg(),msg->getMsgLen());

	uint32_t iStart=0;
	uint32_t iStopp=0;
	uint32_t endoflines=0;
	while ( iStopp<m_Buffer->getSize() )
	{
		if ( memcmp((char *)m_Buffer->getData()+iStopp,"\n",1) == 0 && iStopp < m_Buffer->getSize() )
		{
			logInfo("FTPLINE (%i %i %i): '%.*s' \n",iStart,iStopp,iStopp-iStart,iStopp-iStart,(char *)m_Buffer->getData()+iStart);
			

			switch (m_State)
			{
			case FTP_USER:
				if (strncmp((char *)m_Buffer->getData() + iStart,"331 ",4) == 0)
				{
					logInfo("User accepted, sending pass %s \n",m_Download->getDownloadUrl()->getPass().c_str());
					char *nmsg;
					asprintf(&nmsg,"PASS %s\r\n",m_Download->getDownloadUrl()->getPass().c_str());
					logInfo("FTPSEND: '%s'\n",nmsg);
					m_Socket->doRespond(nmsg,strlen(nmsg));
					free(nmsg);
					m_State = FTP_PASS;
				}
				break;
			case FTP_PASS:
				if (strncmp((char *)m_Buffer->getData() + iStart,"230 ",4) == 0)
				{
					logInfo("%s","Pass accepted, logged in \n");
					m_State = FTP_PORT;

					// get local ip
					int32_t sock = m_Socket->getSocket();

					// get name
					socklen_t len = sizeof(struct sockaddr_in);
					sockaddr_in addr;

					getsockname(sock, (struct sockaddr *)&addr,&len);

					logInfo("local ip is %s \n",inet_ntoa(addr.sin_addr));

					uint32_t ip = *(uint32_t *)&addr.sin_addr;
					uint16_t port = rand()%10000 +32000;

					Socket *socket;
//						 = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,30,30);
					if ( (socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,0,60,30)) == NULL )
					{
						logCrit("Could not bind port %u \n",port);
						return CL_DROP;
					}
					port = socket->getLocalPort();

					m_Context->setActiveFTPBindPort(port);
					socket->addDialogueFactory(g_FTPDownloadHandler);

					char *nmsg;
					asprintf(&nmsg,"PORT %d,%d,%d,%d,%d,%d\r\n",
							(int32_t)ip & 0xff,
							(int32_t)(ip >> 8) & 0xff,
							(int32_t)(ip >> 16) & 0xff,
							(int32_t)(ip >> 24) & 0xff,
							(int32_t)(port >> 8) & 0xff,
							(int32_t)port & 0xff);
                    logInfo("FTPSEND: '%s'\n",nmsg);
					m_Socket->doRespond(nmsg,strlen(nmsg));
					free(nmsg);

				}
				break;
			case FTP_PORT:
				if (strncmp((char *)m_Buffer->getData() + iStart,"200 ",4) == 0)
                {
					logInfo("%s","port command accepted\n");
					char *nmsg;
					asprintf(&nmsg,"RETR %s\r\n",m_Download->getDownloadUrl()->getPath().c_str());
					logSpam("Sending\n %s \n",nmsg);
					m_Socket->doRespond(nmsg,strlen(nmsg));
					free(nmsg);
					m_State = FTP_RETR;
				}

				break;
			case FTP_RETR:
//				logInfo("ftp said %s\n",msg->getMsg());
				break;


			default:
				break;
			}



			iStopp++;
			iStart=iStopp;
			endoflines = iStopp;

			
		} else
			iStopp++;
	}

	m_Buffer->cut(endoflines);
	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel CTRLDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

/**
 * Dialogue::handleTimeout(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel CTRLDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionLost(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel CTRLDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel CTRLDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void CTRLDialogue::setContext(FTPContext *context)
{
	m_Context = context;
}

void CTRLDialogue::setDownload(Download *down)
{
	m_Download = down;
}
