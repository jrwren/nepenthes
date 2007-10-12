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

#include "SUB7Dialogue.hpp"

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

#include "SubmitManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;



/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the SUB7Dialogue, creates a new SUB7Dialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
SUB7Dialogue::SUB7Dialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "SUB7Dialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;
	m_Socket->doRespond("PWD",strlen("PWD"));

	m_State = SUB7_PWD;

	m_Buffer = new Buffer(256);

	m_Download = NULL;

}

SUB7Dialogue::~SUB7Dialogue()
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
 * a small and ugly shell where we can use
 * "download protocol://localction:port/path/to/file
 * to trigger a download
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel SUB7Dialogue::incomingData(Message *msg)
{
	switch(m_State)
	{
	case SUB7_PWD:
		m_Buffer->add(msg->getMsg(),msg->getSize());
		if (strncmp("PWD",(char *)m_Buffer->getData(),3) == 0)
		{
			m_State = SUB7_TID;
			msg->getResponder()->doRespond("You connected.",strlen("You connected."));	// fixme
			m_Buffer->clear();
		}
		break;
	case SUB7_TID:
		m_Buffer->add(msg->getMsg(),msg->getSize());
		if (strncmp("TID",(char *)m_Buffer->getData(),3) == 0)
		{
			m_State = SUB7_FILEINFO;
			msg->getResponder()->doRespond("UPS",strlen("UPS"));
			m_Buffer->clear();
		}
		break;

	case SUB7_FILEINFO:

		m_Buffer->add(msg->getMsg(),msg->getSize());
		if (strncmp("SFT05",(char *)m_Buffer->getData(),5) == 0)
		{
			char *filesize = (char *)malloc(m_Buffer->getSize()-4);
			memset(filesize,0,m_Buffer->getSize()-2);

			memcpy(filesize,(char *)m_Buffer->getData()+5,m_Buffer->getSize()-5);
			logInfo("Sub7 Filetransferr Size is %s \n",filesize);
			m_FileSize = atoi(filesize);
			m_State = SUB7_FILETRANSFERR;
			m_Buffer->clear();
			m_Download = new Download(msg->getRemoteHost(),(char *)"sub7://foo/bar",msg->getRemoteHost(),(char *)"some triggerline");
			free(filesize);
		}
		break;
	case SUB7_FILETRANSFERR:
		m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getSize());
		if (m_Download->getDownloadBuffer()->getSize() == m_FileSize)
		{
			msg->getResponder()->doRespond("+OK RECVD",strlen("+OK RECVD"));
			g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
		}
		logInfo("got %i bytes \n",msg->getSize());
		break;

	}
	
	logInfo("got %i bytes data\n",msg->getSize());
	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel SUB7Dialogue::outgoingData(Message *msg)
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
ConsumeLevel SUB7Dialogue::handleTimeout(Message *msg)
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
ConsumeLevel SUB7Dialogue::connectionLost(Message *msg)
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
ConsumeLevel SUB7Dialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

