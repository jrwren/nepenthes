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
 
#include <sys/types.h>
#include <sys/param.h>
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

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_dia | l_hlr

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

	m_State = FTP_CONNECTED;
	m_Download = down;

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
//	logDebug("RX:\n%.*s\n",msg->getSize(),msg->getMsg());
	if (m_Download == NULL && m_State < FTP_RETR)
	{
		logWarn("broken ftp daemon \n");
		return CL_DROP;
	}

	m_Buffer->add(msg->getMsg(),msg->getSize());

	uint32_t iStart=0;
	uint32_t iStopp=0;
	uint32_t endoflines=0;
	while ( iStopp<m_Buffer->getSize() )
	{
		if ( memcmp((char *)m_Buffer->getData()+iStopp,"\n",1) == 0 && iStopp < m_Buffer->getSize() )
		{
			logDebug("FTPLINE (%i %i %i): '%.*s' \n",iStart,iStopp,iStopp-iStart,iStopp-iStart,(char *)m_Buffer->getData()+iStart);
			

			// add ftp daemon fingerprinting here
			


			switch (m_State)
			{
			case FTP_CONNECTED:
				if (strncmp((char *)m_Buffer->getData() + iStart,"220",3) == 0)
				{
					sendUser();
					m_State = FTP_USER;
				};

			case FTP_USER:
				if (parseUser((char *)m_Buffer->getData() + iStart) == true)
				{
					sendPass();
					m_State = FTP_PASS;
				}
				
				break;

			case FTP_PASS:
				if (parsePass((char *)m_Buffer->getData() + iStart) == true)
				{
					if ( m_Download->getDownloadFlags() != 0 )
					{
						if ( m_Download->getDownloadFlags() & DF_TYPE_BINARY )
						{
							sendType();
							m_State = FTP_TYPE;
						}

					}else
					if ( m_Download->getDownloadUrl()->getDir() != "" )
					{
						sendCWD();
						m_State = FTP_CWD;
					}
					else
					{
						sendPort();
						m_State = FTP_PORT;
					}
					
				}
				break;


			case FTP_TYPE:
				if ( parseType((char *)m_Buffer->getData() + iStart)== true )
				{
					if ( m_Download->getDownloadUrl()->getDir() != "" )
					{
						sendCWD();
						m_State = FTP_CWD;
					} else
					{
						sendPort();
						m_State = FTP_PORT;
					}
				}
				break;

			case FTP_CWD:
				if (parseCWD((char *)m_Buffer->getData() + iStart)== true)
				{
					sendPort();
					m_State = FTP_PORT;
				}


			case FTP_PORT:
				if (parsePort((char *)m_Buffer->getData() + iStart) == true)
                {
					sendRetr();
					m_State = FTP_RETR;
				}
				break;

			case FTP_RETR:
				if (strncmp((char *)m_Buffer->getData() + iStart,"150",3) == 0)
				{
					logDebug("RETR accepted\n");
				}else
				if (strncmp((char *)m_Buffer->getData() + iStart,"226",3) == 0)
				{
					logDebug("Transferr finished\n");
					sendQuit();
					m_State = FTP_QUIT;
				}
				break;

			case FTP_QUIT:
				if (parseQuit((char *)m_Buffer->getData() + iStart) == true)
				{
					return CL_DROP;
				}
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
	if (m_State == FTP_RETR)
	{
    	sendQuit();
		m_State = FTP_QUIT;
		return CL_ASSIGN;
	}else
	{
		return CL_DROP;
	}
	
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


void CTRLDialogue::sendUser()
{
	char *msg;
	if (asprintf(&msg,"USER %s\r\n",m_Download->getDownloadUrl()->getUser().c_str()) == -1) {
		logCrit("Memory allocation error\n");
		exit(EXIT_FAILURE);
	}
	logDebug("FTPSEND: '%s'\n",msg);
	m_Socket->doRespond(msg,strlen(msg));
	free(msg);
}
bool CTRLDialogue::parseUser(char *msg)
{
	if (strncmp(msg,"331",3) == 0)
	{
		logDebug("User accepted .. \n",m_Download->getDownloadUrl()->getPass().c_str());
		return true;
	}else
	{
		return false;
	}
}


void CTRLDialogue::sendPass()
{
	char *nmsg;
	if (asprintf(&nmsg,"PASS %s\r\n",m_Download->getDownloadUrl()->getPass().c_str()) == -1) {
		logCrit("Memory allocation error\n");
		exit(EXIT_FAILURE);
	}
	logDebug("FTPSEND: '%s'\n",nmsg);
	m_Socket->doRespond(nmsg,strlen(nmsg));
	free(nmsg);
	
}
bool CTRLDialogue::parsePass(char *msg)
{
	if (strncmp(msg,"230",3) == 0)
	{
		logDebug("Pass accepted, logged in \n");
		return true;
	}else
	{
		return false;
	}


}



void CTRLDialogue::sendType()
{
	const char *nmsg = "TYPE I\r\n";
	m_Socket->doRespond(nmsg,strlen(nmsg));
	logDebug("FTPSEND: '%s'\n",nmsg);
}

bool CTRLDialogue::parseType(char *msg)
{
	if (strncmp(msg,"200",3) == 0)
	{
		logDebug("Type accepted \n");
		return true;
	}else
	{
		return false;
	}
}



void CTRLDialogue::sendPort()
{
	logDebug("System ... \n");

	uint32_t ip;
	uint16_t minport;
	uint16_t maxport;

	if ( g_FTPDownloadHandler->getRetrAddress() == 0 )
	{ // no NAT settings
		// get local ip
		int32_t sock = m_Socket->getSocket();

		// get name
		socklen_t len = sizeof(struct sockaddr_in);
		sockaddr_in addr;

		getsockname(sock, (struct sockaddr *)&addr,&len);

		logDebug("local ip is %s \n",inet_ntoa(addr.sin_addr));

		ip =  *(uint32_t *)&addr.sin_addr;

		minport = rand()%40000+1024;
		maxport = minport + 1000;
	} else
	{	// nat settings, use external ip
		ip = g_FTPDownloadHandler->getRetrAddress();
		minport = g_FTPDownloadHandler->getMinPort();
		maxport = g_FTPDownloadHandler->getMaxPort();

	}

	uint16_t port = 0;

	Socket *socket=NULL;


	for (uint16_t i =minport; i<maxport;i++)
	{
		/* workaround buggy PORT calculation in 'some' worm families */
		if ( ((i >> 4) & 0xf) == 0 )
			continue;

		if ( (socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,i,60,30)) != NULL )
		{
			if ( socket->getDialogst()->size() == 0 && socket->getFactories()->size() == 0 )
			{
				logInfo("Found unused bind socket on port %i\n",i);
				break;
			}
		}

	}

	if ( socket == NULL)
	{
		logCrit("Could not bind port in range %i -> %i \n",minport, maxport);
		return;
	}

	port = socket->getLocalPort();

	m_Context->setActiveFTPBindPort(port);
	socket->addDialogueFactory(g_FTPDownloadHandler);

	char *nmsg;
	

	if (asprintf(&nmsg,"PORT %d,%d,%d,%d,%d,%d\r\n",
#if BYTE_ORDER == BIG_ENDIAN
			(int32_t)(ip >> 24) & 0xff,
			(int32_t)(ip >> 16) & 0xff,
			(int32_t)(ip >> 8) & 0xff,
			(int32_t)ip & 0xff,
#else
			(int32_t)ip & 0xff,
			(int32_t)(ip >> 8) & 0xff,
			(int32_t)(ip >> 16) & 0xff,
			(int32_t)(ip >> 24) & 0xff,
#endif
			(int32_t)(port >> 8) & 0xff,
			(int32_t)port & 0xff) == -1) {
		logCrit("Memory allocation error\n");
		exit(EXIT_FAILURE);
	}
	logDebug("FTPSEND: '%s'\n",nmsg);
	m_Socket->doRespond(nmsg,strlen(nmsg));
	free(nmsg);

}

bool CTRLDialogue::parsePort(char *msg)
{
	if (strncmp(msg,"200",3) == 0)
	{
		logDebug("Port accepted\n");
		return true;
	}else
	{
		return false;
	}
	
}


void CTRLDialogue::sendRetr()
{
	
	char *nmsg;
	if (asprintf(&nmsg,"RETR %s\r\n",m_Download->getDownloadUrl()->getFile().c_str()) == -1) {
		logCrit("Memory allocation error\n");
		exit(EXIT_FAILURE);
	}
	logDebug("FTPSEND: '%s'\n",nmsg);
	m_Socket->doRespond(nmsg,strlen(nmsg));
	free(nmsg);
}

bool CTRLDialogue::parseRetr(char *msg)
{
	if (strncmp(msg,"150",3) == 0)
	{
		logDebug("Retr accepted\n");
		return true;
	}else
	{
		return false;
	}
}

void CTRLDialogue::sendQuit()
{
	
	const char *nmsg = "QUIT\r\n";
	
	logDebug("FTPSEND: '%s'\n",nmsg);
	m_Socket->doRespond(nmsg,strlen(nmsg));
//	free(nmsg);
}

bool CTRLDialogue::parseQuit(char *msg)
{
	if (strncmp(msg,"221",3) == 0)
	{
		logDebug("Quit accepted\n");
		return true;
	}else
	{
		return false;
	}
}

void CTRLDialogue::sendCWD()
{
	char *nmsg;
	if (asprintf(&nmsg,"CWD %s\r\n",m_Download->getDownloadUrl()->getDir().c_str()) == -1) {
		logCrit("Memory allocation error\n");
		exit(EXIT_FAILURE);
	}
	logDebug("FTPSEND: '%s'\n",nmsg);
	m_Socket->doRespond(nmsg,strlen(nmsg));
	free(nmsg);
}

bool CTRLDialogue::parseCWD(char *msg)
{
	if (strncmp(msg,"250",3) == 0)
	{
		logDebug("CWD accepted\n");
		return true;
	}else
	{
		return false;
	}
}

