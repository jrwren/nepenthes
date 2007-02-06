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

#include "CReceiveDialogue.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"

#include "Message.hpp"

#include "ShellcodeManager.hpp"
#include "Utilities.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"

#include "Download.cpp"
#include "DownloadUrl.cpp"
#include "DownloadBuffer.cpp"

#include "SubmitManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_dia | l_hlr

using namespace nepenthes;



/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the CReceiveDialogue, creates a new CReceiveDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
CReceiveDialogue::CReceiveDialogue(Socket *socket)//, Download *down)
{
	m_Socket = socket;
    m_DialogueName = "CReceiveDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;
	char *url;
	uint32_t host = socket->getRemoteHost();
	uint16_t port = socket->getRemotePort();
	asprintf(&url,"creceive://%s:%i",inet_ntoa(*(in_addr *)&host),port);
    m_Download = new Download(socket->getLocalHost(),url,socket->getRemoteHost(),url);
	free(url);
}

CReceiveDialogue::~CReceiveDialogue()
{
//	HEXDUMP(m_Socket,(byte *)m_Buffer->getData(),m_Buffer->getSize());
//	delete m_Buffer;
	delete m_Download;
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
ConsumeLevel CReceiveDialogue::incomingData(Message *msg)
{
	logSpam("... DATA ... FIXME %i bytes \n",msg->getSize());
	m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getSize());

	if (m_Download->getDownloadBuffer()->getSize() > 1024 * 1024 * 4)	// hardcoded 4mb limit for now (tm)
		return CL_DROP;

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
ConsumeLevel CReceiveDialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
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
ConsumeLevel CReceiveDialogue::handleTimeout(Message *msg)
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
ConsumeLevel CReceiveDialogue::connectionLost(Message *msg)
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
ConsumeLevel CReceiveDialogue::connectionShutdown(Message *msg)
{
	g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
	return CL_DROP;
}

