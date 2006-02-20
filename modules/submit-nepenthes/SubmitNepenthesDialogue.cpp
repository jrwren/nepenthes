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
 
#include "SubmitNepenthesDialogue.hpp"
#include "Message.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the SubmitNepenthesDialogue, creates a new SubmitNepenthesDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
SubmitNepenthesDialogue::SubmitNepenthesDialogue(Socket *socket, char *file, unsigned int len, char *md5sum)
{
	m_Socket = socket;
    m_DialogueName = "SubmitNepenthesDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

    m_File = (char *)malloc(len);
	m_FileSize = len;
	memcpy(m_File,file,len);

	
	string req = md5sum;
	req += "\r\n";
	socket->doRespond((char *)req.c_str(),req.size());

	m_State = DOWN_N_MD5SUM;
}

SubmitNepenthesDialogue::~SubmitNepenthesDialogue()
{
	free(m_File);
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
ConsumeLevel SubmitNepenthesDialogue::incomingData(Message *msg)
{
	switch (m_State)
	{
	case DOWN_N_MD5SUM:
		if (msg->getMsgLen() == strlen("SENDFILE\r\n") && memcmp(msg->getMsg(),"SENDFILE\r\n",strlen("SENDFILE\r\n")) == 0)
		{
			m_State = DOWN_N_FILE;
			m_Socket->doRespond(m_File,m_FileSize);
			m_Socket->setStatus(SS_CLEANQUIT);
		};
		break;
	case DOWN_N_FILE:
		logCrit("server talking to me in wrong state, dropping\n'%s'\n", msg->getMsg());
		return CL_DROP;
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
ConsumeLevel SubmitNepenthesDialogue::outgoingData(Message *msg)
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
ConsumeLevel SubmitNepenthesDialogue::handleTimeout(Message *msg)
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
ConsumeLevel SubmitNepenthesDialogue::connectionLost(Message *msg)
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
ConsumeLevel SubmitNepenthesDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

