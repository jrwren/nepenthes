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



#include "MSMQDialogue.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "Utilities.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"


using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the MSMQDialogue, creates a new MSMQDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
MSMQDialogue::MSMQDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "MSMQDialogue";
	m_DialogueDescription = "handles MSMQ Connections";

	m_ConsumeLevel = CL_ASSIGN;

	m_State = MSMQ_NULL;
	m_Buffer = new Buffer(2048);
}

MSMQDialogue::~MSMQDialogue()
{
	switch (m_State)
	{
	case MSMQ_NULL:
	case MSMQ_SHELLCODE:
		logWarn("Unknown MSMQ exploit %i bytes State %i\n",m_Buffer->getSize(), m_State);
		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *) m_Buffer->getData(), m_Buffer->getSize());
		break;

	case MSMQ_DONE:
		break;

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
ConsumeLevel MSMQDialogue::incomingData(Message *msg)
{

	m_Buffer->add(msg->getMsg(),msg->getMsgLen());

	switch (m_State)
	{
	case MSMQ_NULL:
		{
			char reply[64];
			memset(reply,0,64);
			reply[0]=0x82;
			msg->getResponder()->doRespond(reply,64);
			m_State = MSMQ_SHELLCODE;
			m_Buffer->clear();

		}
		break;

	case MSMQ_SHELLCODE:
		{
			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(),m_Socket->getLocalPort(), m_Socket->getRemotePort(),
									   m_Socket->getLocalHost(), m_Socket->getRemoteHost(), m_Socket, m_Socket);
			sch_result sch = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg);
			delete Msg;

			if ( sch == SCH_DONE )
			{
				m_Buffer->clear();
				m_State = MSMQ_DONE;
				return CL_ASSIGN_AND_DONE;
			}
			

		}
		break;

	case MSMQ_DONE:
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
ConsumeLevel MSMQDialogue::outgoingData(Message *msg)
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
ConsumeLevel MSMQDialogue::handleTimeout(Message *msg)
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
ConsumeLevel MSMQDialogue::connectionLost(Message *msg)
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
ConsumeLevel MSMQDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}
