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
#include <arpa/tftp.h>
#include <netinet/in.h>

#include "TFTPDialogue.hpp"

#include "UDPSocket.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "SubmitManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_dia | l_hlr


using namespace nepenthes;

TFTPDialogue::TFTPDialogue(Socket *socket)
{
	m_DialogueName = "TFTPDialogue";
	m_DialogueDescription = "download a file via tftp";

	m_Socket = socket;
	m_ConsumeLevel = CL_ASSIGN;
	m_Retries = 0;
	m_Blocks = 0;
	m_LastSendPacket = NULL;
}


TFTPDialogue::~TFTPDialogue()
{
	logPF();
	delete m_Download;
	if (m_LastSendPacket != NULL)
		free(m_LastSendPacket);
}

void TFTPDialogue::setDownload(Download *down)
{
	m_Download = down;
}


void TFTPDialogue::setMaxFileSize(uint32_t ul)
{
	m_MaxFileSize = ul;
}

void TFTPDialogue::setMaxRetries(uint32_t i)
{
	m_MaxRetries = i;
}

int32_t TFTPDialogue::setRequest(char *file)
{
	m_LastSendPacket = (char *) malloc(strlen(file) + 9);
	* ((uint16_t *) m_LastSendPacket) = 0x0100;          
	strcpy((char *) m_LastSendPacket + 2, file);
	strcpy((char *) m_LastSendPacket + strlen(file) + 3, "octet");
	m_LastSendLength = strlen(file) + 9;
	return strlen(file) + 9;
}

char *TFTPDialogue::getRequest()
{
	return m_LastSendPacket;
}

ConsumeLevel TFTPDialogue::incomingData(Message *msg)
{
/*	logDebug("read %i bytes from %s \n",msg->getSize(), m_Download->getDownloadUrl()->getPath().c_str());
	m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getSize());
	if(m_Download->getDownloadBuffer()->getSize() < m_Download->getDownloadUrl()->getPort())
        return CL_ASSIGN;

	msg->getSocket()->getNepenthes()->getSubmitMgr()->addSubmission(m_Download);
*/

	struct tftphdr *ptftphdr = (struct tftphdr *)msg->getMsg();
    
	switch ( ntohs(ptftphdr->th_opcode) )
	{
	case RRQ:
        break;
	case WRQ:
	case ACK:
//	case OACK:
		break;

	case ERROR:
        logInfo("Got Error \"%.*s\"  %s \n", msg->getSize()-4, ptftphdr->th_msg , m_Download->getUrl().c_str());
		m_Socket->setStatus(SS_CLOSED);
		break;

	case DATA:
		{
//			logInfo("got %i bytes \n",msg->getSize());
			m_Retries=0;
			uint32_t iBlockNum = ntohs(ptftphdr->th_block);
			if (iBlockNum != m_Blocks + 1)
			{
				logDebug("Got block out of order %i <-> %i %s \n",m_Blocks, iBlockNum, m_Download->getUrl().c_str());
				return CL_ASSIGN;
			}

			struct tftphdr stftphdr;
            stftphdr.th_opcode = htons(ACK);
			stftphdr.th_block = htons(iBlockNum);
            msg->getResponder()->doRespond((char *)&stftphdr,4);
			m_LastSendLength = 4;
			memcpy(m_LastSendPacket,&stftphdr,4);
			m_Blocks++;

			if (m_Download->getDownloadBuffer()->getSize() + msg->getSize() - 4 > m_MaxFileSize )
			{
				logWarn("Discarded downloading file %s  due to filesizelimit \n", m_Download->getUrl().c_str());
				m_Socket->setStatus(SS_CLOSED);
				return CL_DROP;
			}

			m_Download->getDownloadBuffer()->addData(msg->getMsg()+4,msg->getSize()-4);
			if (msg->getSize() < 512)
			{	// last packet   
				logInfo("Downloaded file %s %i bytes\n", m_Download->getUrl().c_str(), m_Download->getDownloadBuffer()->getSize());
				msg->getSocket()->getNepenthes()->getSubmitMgr()->addSubmission(m_Download);
                m_Socket->setStatus(SS_CLOSED);
			}
		}
		break;

	}
	return CL_ASSIGN;
}

ConsumeLevel TFTPDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

ConsumeLevel TFTPDialogue::handleTimeout(Message *msg)
{
	m_Retries++;
	if (m_Retries < m_MaxRetries)
	{// resend last packet
		logSpam("Resending Last Packet due to timeout (%i timeouts left) \n", m_MaxRetries - m_Retries );
		msg->getResponder()->doRespond(getRequest(),m_LastSendLength);
		return CL_ASSIGN;
	}else
	{
		logInfo("Max Timeouts reached (%i) %s \n", m_MaxRetries, m_Download->getUrl().c_str() );
		return CL_DROP;
	}
}

ConsumeLevel TFTPDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel TFTPDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}
