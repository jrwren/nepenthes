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

#include "MydoomDialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"


#include "Download.hpp"
#include "Download.cpp"

#include "DownloadBuffer.hpp"
#include "DownloadBuffer.cpp"

#include "DownloadUrl.hpp"
#include "DownloadUrl.cpp"

#include "SubmitManager.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the MydoomDialogue, creates a new MydoomDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
MydoomDialogue::MydoomDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "MydoomDialogue";
	m_DialogueDescription = "emulates the mydoom backdoor";

	m_ConsumeLevel = CL_ASSIGN;

	m_State = MYDOOM_TRAILOR;
	m_Buffer = new Buffer(64);

	m_Download = NULL;

}

MydoomDialogue::~MydoomDialogue()
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
ConsumeLevel MydoomDialogue::incomingData(Message *msg)
{
	const char *MydoomTrailor = "\x85\x13\x3c\x9e\xa2";

	switch (m_State)
	{
	case MYDOOM_TRAILOR:
		m_Buffer->add((char *)msg->getMsg(),msg->getSize());
		if (m_Buffer->getSize() >= strlen(MydoomTrailor))
		{
			if (memcmp(m_Buffer->getData(),MydoomTrailor,strlen(MydoomTrailor)) == 0)
			{
				m_State = MYDOOM_FILETRANSFERR;
				m_Buffer->cut(strlen(MydoomTrailor));

				string url = "mydoom://";
				uint32_t remote = msg->getRemoteHost();
				url += inet_ntoa(*(struct in_addr *)&remote);

				m_Download = new Download(msg->getLocalHost(),(char *)url.c_str(),msg->getRemoteHost(),"some triggerline");
				m_Download->getDownloadBuffer()->addData((char *)m_Buffer->getData(),m_Buffer->getSize());
				m_Buffer->clear();
				return CL_ASSIGN_AND_DONE;
			}
		}
		if (m_Buffer->getSize() > 128 )
			return CL_DROP;

		break;
	case MYDOOM_FILETRANSFERR:
		{
			m_Download->getDownloadBuffer()->addData((char *)msg->getMsg(),msg->getSize());
			return CL_ASSIGN;
		}
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
ConsumeLevel MydoomDialogue::outgoingData(Message *msg)
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
ConsumeLevel MydoomDialogue::handleTimeout(Message *msg)
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
ConsumeLevel MydoomDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * a closed connection indicates a successfull mydoom transferr
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel MydoomDialogue::connectionShutdown(Message *msg)
{
	logPF();
	if (m_Download != NULL)
	{
    	g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
	}
	return CL_DROP;
}

