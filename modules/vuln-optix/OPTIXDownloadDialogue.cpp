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

#include "vuln-optix.hpp"
#include "OPTIXDownloadDialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"

#include "Utilities.hpp"

#include "DialogueFactoryManager.hpp"
#include "Download.hpp"
#include "Download.cpp"

#include "DownloadBuffer.hpp"
#include "DownloadBuffer.cpp"

#include "DownloadUrl.hpp"
#include "DownloadUrl.cpp"

#include "Buffer.hpp"

#include "SubmitManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the OPTIXDownloadDialogue, creates a new OPTIXDownloadDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
OPTIXDownloadDialogue::OPTIXDownloadDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "OPTIXDownloadDialogue";
	m_DialogueDescription = "Optix Shell Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	char oc192bindpcre[] = "((.*)\\r\\n(.*)\\r\\n)";

	logInfo("pcre is %s \n",oc192bindpcre);

	const char * pcreEerror;
	int32_t pcreErrorPos;
	if ((m_pcre = pcre_compile(oc192bindpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("OPTIXDownloadDialoguePCRE could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				oc192bindpcre, pcreEerror, pcreErrorPos);
	}

	m_State = OPTIX_DL_FILEINFO;
	m_Buffer = new Buffer(256);
	m_Download = NULL;
}

OPTIXDownloadDialogue::~OPTIXDownloadDialogue()
{
	if (m_Download != NULL)
	{
		delete m_Download;
	}
	delete m_Buffer;
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel OPTIXDownloadDialogue::incomingData(Message *msg)
{
	logPF();
	switch(m_State)
	{
	case OPTIX_DL_FILEINFO:
		{
			m_Buffer->add((char *)msg->getMsg(),msg->getSize());
			int32_t piOutput[10 * 3];
			int32_t iResult; 
			if ((iResult = pcre_exec(m_pcre, 0, (char *) m_Buffer->getData(), m_Buffer->getSize(), 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
			{
				const char *filepath;
				pcre_get_substring((char *) m_Buffer->getData(), piOutput, iResult, 2, &filepath);

				const char *filesize;
				pcre_get_substring((char *) m_Buffer->getData(), piOutput, iResult, 3, &filesize);

				m_FileSize = atoi(filesize);
				logInfo("OPTIX filetransferr path is %s size is %i \n",filepath,m_FileSize);
				
				

				msg->getResponder()->doRespond("+OK REDY",strlen("+OK REDY"));
				m_State = OPTIX_DL_FILETRANSFERR;
				m_Download = new Download(msg->getRemoteHost(),"optix://foo/bar",msg->getRemoteHost(),"some triggerline");
			}
			break;
		}
	case OPTIX_DL_FILETRANSFERR:
		{
			m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getSize());
			if (m_Download->getDownloadBuffer()->getSize() == m_FileSize)
			{
				msg->getResponder()->doRespond("+OK RECVD",strlen("+OK RECVD"));
				g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
				
			}

	
		}
		break;
	}
//	C:\\a.exe\r\n%d\r\n

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
ConsumeLevel OPTIXDownloadDialogue::outgoingData(Message *msg)
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
ConsumeLevel OPTIXDownloadDialogue::handleTimeout(Message *msg)
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
ConsumeLevel OPTIXDownloadDialogue::connectionLost(Message *msg)
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
ConsumeLevel OPTIXDownloadDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

