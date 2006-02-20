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
#include "OPTIXShellDialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"
#include "Utilities.hpp"

#include "DialogueFactoryManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the OPTIXShellDialogue, creates a new OPTIXShellDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
OPTIXShellDialogue::OPTIXShellDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "OPTIXShellDialogue";
	m_DialogueDescription = "Optix Shell Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

//	m_Socket->doRespond("Welcome to dong Shell\n",strlen("Welcome to dong Shell\n"));

	m_Buffer = new Buffer(256);
	m_State = OPTIX_CONNECTED;
}

OPTIXShellDialogue::~OPTIXShellDialogue()
{
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
ConsumeLevel OPTIXShellDialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getMsgLen());

	switch(m_State)
	{
	case OPTIX_CONNECTED:
		if (m_Buffer->getSize() > 4)
		{	
			// we could do this with pcre ...
       		if (memcmp(m_Buffer->getData(),"022¬",4) == 0)
           	{
				m_State = OPTIX_AUTHED;

				// dont know what exactly the optix replies
				msg->getResponder()->doRespond("001¬ YOhoo your mum\r\n",strlen("001¬ YOhoo your mum\r\n"));
				m_Buffer->clear();
			}
        }
		break;

	case OPTIX_AUTHED:
		if (m_Buffer->getSize() >= 6)
		{	
			g_Nepenthes->getUtilities()->hexdump((byte *)m_Buffer->getData(),m_Buffer->getSize());
			// we could do this with pcre ...
       		if (memcmp(m_Buffer->getData(),"019¬\r\n",6) == 0)
           	{
                msg->getResponder()->doRespond("020¬\r\n",strlen("020¬\r\n"));
				m_Buffer->clear();

				// this will just open the optix downloadmanagers bind socket it its closed
				
				g_Nepenthes->getDownloadMgr()->downloadUrl("optix://localhost:500/file",msg->getRemoteHost(),"optix foobar");
			}
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
ConsumeLevel OPTIXShellDialogue::outgoingData(Message *msg)
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
ConsumeLevel OPTIXShellDialogue::handleTimeout(Message *msg)
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
ConsumeLevel OPTIXShellDialogue::connectionLost(Message *msg)
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
ConsumeLevel OPTIXShellDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

