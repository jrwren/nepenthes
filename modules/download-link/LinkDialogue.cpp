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

#include "LinkDialogue.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Utilities.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "DownloadUrl.hpp"

#include "Download.cpp"
#include "DownloadBuffer.cpp"
#include "DownloadUrl.cpp"


#include "SubmitManager.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia | l_hlr

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the LinkDialogue, creates a new LinkDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
LinkDialogue::LinkDialogue(Socket *socket, Download *down, uint32_t maxfilesize)
{

	unsigned char *response = g_Nepenthes->getUtilities()->b64decode_alloc((unsigned char *)down->getDownloadUrl()->getPath().c_str());

	unsigned char authKey[4];
	memcpy(authKey,response,4);
	memcpy(m_Challenge,response,4);

	
//	logInfo("LinkDialogue key #1 0x%02x%02x%02x%02x.\n",authKey[0], authKey[1], authKey[2], authKey[3]);
	logDebug("LinkDialogue key #2 0x%02x%02x%02x%02x.\n",response[0], response[1], response[2], response[3]);
	free(response);
	m_Socket = socket;
    m_DialogueName = "LinkDialogue";
	m_DialogueDescription = "connect linkbots and download files";

	m_ConsumeLevel = CL_ASSIGN;

	m_Buffer = new Buffer(512);

	m_State = LINK_NULL;
    m_Download = down;

	m_MaxFileSize = maxfilesize;
}

LinkDialogue::~LinkDialogue()
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
 * a small and ugly shell where we can use
 * "download protocol://localction:port/path/to/file
 * to trigger a download
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel LinkDialogue::incomingData(Message *msg)
{
	logPF();
	switch (m_State)
	{
	case LINK_NULL:
		{
			m_Buffer->add(msg->getMsg(),msg->getSize());
//			g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *)m_Buffer->getData(),m_Buffer->getSize());
			msg->getResponder()->doRespond((char *)&m_Challenge,4);

			m_State = LINK_FILE;
		}
		break;

	case LINK_FILE:
		m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getSize());
		break;
	}



//	msg->getResponder()->doRespond("deine mutter\n",strlen("deine mutter\n"));
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
ConsumeLevel LinkDialogue::outgoingData(Message *msg)
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
ConsumeLevel LinkDialogue::handleTimeout(Message *msg)
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
ConsumeLevel LinkDialogue::connectionLost(Message *msg)
{
	logWarn("Download via linkbot filetransferr failed (connection lost) ! ( download %i bytes, buffer is %i bytes)\n",m_Download->getDownloadBuffer()->getSize(),m_Buffer->getSize());
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
ConsumeLevel LinkDialogue::connectionShutdown(Message *msg)
{
	switch (m_State)
	{
	case LINK_FILE:
		if (m_Download->getDownloadBuffer()->getSize() > 0)
		{
			logInfo("Download via linkbot filetransferr done! ( download is %i bytes)\n",m_Download->getDownloadBuffer()->getSize());
        	g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
		}else
		{
			logDebug("Download via linkbot filetransferr failed! ( download %i bytes, buffer is %i bytes)\n",m_Download->getDownloadBuffer()->getSize(),m_Buffer->getSize());
		}
		break;
	case LINK_NULL:
		logDebug("Download via linkbot filetransferr failed! ( buffer is %i bytes)\n",m_Buffer->getSize());
		break;

	}

	return CL_DROP;
}



