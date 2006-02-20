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

#include <ctype.h>

#include "BagleDialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Download.hpp"
#include "Download.cpp"
#include "DownloadBuffer.hpp"
#include "DownloadBuffer.cpp"
#include "DownloadUrl.hpp"
#include "DownloadUrl.cpp"


#include "Utilities.hpp"

#include "SubmitManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the BagleDialogue, creates a new BagleDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
BagleDialogue::BagleDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "BagleDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_State = BAGLE_AUTH;
	m_Buffer = new Buffer(64);
	m_Download = NULL;
}

BagleDialogue::~BagleDialogue()
{
	delete m_Buffer;
	if (m_Download != NULL)
	{
		delete m_Download;
	}
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel BagleDialogue::incomingData(Message *msg)
{
	const char *BagleAuths[2] = {   
		"\x43\xFF\xFF\xFF\x30\x30\x30\x01\x0A\x1F\x2B\x28\x2B\xA1\x32\x01",
		"\x43\xFF\xFF\xFF\x30\x30\x30\x01\x0A\x28\x91\xA1\x2B\xE6\x60\x2F\x32\x8F\x60\x15\x1A\x20\x1A"
	};

	switch (m_State)
	{
	case BAGLE_AUTH:
		m_Buffer->add((char *)msg->getMsg(),msg->getMsgLen());
		for (int i=0;i<=1;i++)
		{
			if (m_Buffer->getSize() >= strlen(BagleAuths[i]))
			{
				if (memcmp(m_Buffer->getData(),BagleAuths[i],strlen(BagleAuths[i])) == 0)
				{
					logInfo("Successfull Bagle Auth (auth %i) \n",i);
					msg->getResponder()->doRespond("12345678",8);
					m_State = BAGLE_REFERRER;
					m_Buffer->clear();
					return CL_ASSIGN;
				}
			}
		}
		
		logCrit("Unknown Bagle Auth (%i)\n",m_Buffer->getSize());
		g_Nepenthes->getUtilities()->hexdump(l_crit | STDTAGS ,(byte *)m_Buffer->getData(),m_Buffer->getSize());
		if (m_Buffer->getSize() > 128 )
			return CL_DROP;

		break;
	case BAGLE_REFERRER:
        {
			if ( (msg->getMsgLen() > 4 && strncasecmp(msg->getMsg(),"http",4) == 0 ) ||
			(msg->getMsgLen() > 3 && strncasecmp(msg->getMsg(),"ftp",3) == 0 ) )
			{
				// we simply hope the url does not get fragmented
				char *url = (char *)malloc(msg->getMsgLen()+1);
				memset(url,0,msg->getMsgLen()+1);
				memcpy(url,msg->getMsg(),msg->getMsgLen());

				for (unsigned int i=0;i<=strlen(url);i++)
				{
					if (isprint(url[i]) == 0)
					{
						url[i] = '\0';
					}
				}
				logInfo("Bagle URL %s \n",url);
				g_Nepenthes->getDownloadMgr()->downloadUrl(url,msg->getRemoteHost(),url);
				free(url);
				return CL_DROP;
				
			}else
			if ( msg->getMsgLen() >= 4 )
			{
				m_FileSize = ntohs (*(unsigned int *)msg->getMsg());
				logInfo("Unexpected but detected: Bagle Binary Stream (%i bytes)\n",m_FileSize);
				m_State = BAGLE_BINARY;
				m_Download = new Download("bagle://",m_Socket->getRemoteHost(),"bagle://foo/bar");
				m_Download->getDownloadBuffer()->addData(msg->getMsg()+4,msg->getMsgLen()-4);
			}
		}
		break;
		
	case BAGLE_BINARY:
		// FIXME m_MaxFileSize
		m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getMsgLen());
		break;

	}



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
ConsumeLevel BagleDialogue::outgoingData(Message *msg)
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
ConsumeLevel BagleDialogue::handleTimeout(Message *msg)
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
ConsumeLevel BagleDialogue::connectionLost(Message *msg)
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
ConsumeLevel BagleDialogue::connectionShutdown(Message *msg)
{
	if ( m_Download != NULL )
	{
		if ( m_Download->getDownloadBuffer()->getLength() == m_FileSize )
		{
			g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
			// destructor will delete it
		}
	}
	return CL_DROP;
}

