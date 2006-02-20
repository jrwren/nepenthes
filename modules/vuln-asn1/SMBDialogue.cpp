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

#include "SMBDialogue.hpp"
#include "asn1-shellcodes.h"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "Buffer.hpp"


#include "Utilities.hpp"
#include "ShellcodeManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the SMBDialogue, creates a new SMBDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
SMBDialogue::SMBDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "SMBDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_Buffer = new Buffer(1024);

	m_State = SMB_NEGOTIATE;
}

SMBDialogue::~SMBDialogue()
{
	switch (m_State)
	{
	case SMB_DONE:
		break;

	default:
		logWarn("Unknown %s Shellcode (Buffer %i bytes) (State %i)\n","ASN1_SMB",m_Buffer->getSize(),m_State);
		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *)m_Buffer->getData(),m_Buffer->getSize());
	}

	delete m_Buffer;
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
ConsumeLevel SMBDialogue::incomingData(Message *msg)
{
	logPF();
	m_Buffer->add(msg->getMsg(),msg->getMsgLen());	
	switch ( m_State )
	{
	case SMB_NEGOTIATE:

		if ( m_Buffer->getSize() >= sizeof(smb_request1) && 
			 memcmp(smb_request1, m_Buffer->getData(), 30 ) == 0 &&
			 memcmp(smb_request1+32, (char *)m_Buffer->getData()+32, 137-32) == 0 )
		{
			logInfo("Got ASN1 SMB exploit Stage #1(%i)\n",msg->getMsgLen());
			m_Buffer->cut(sizeof(smb_request1));
			m_State = SMB_SESSION_SETUP;
			return CL_UNSURE;	// same as lsass bindstr
		}else
		{
			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
									   msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());

			sch_result res = msg->getSocket()->getNepenthes()->getShellcodeMgr()->handleShellcode(&Msg);
			delete Msg;
			if ( res == SCH_DONE )
			{
				m_State = SMB_DONE;
				return CL_ASSIGN_AND_DONE;
			}

//			return CL_DROP; // ignore possible mtu problems here
		}
		break;

	case SMB_SESSION_SETUP:
		if ( m_Buffer->getSize() >= sizeof(smb_request2) &&
			 memcmp(smb_request2, m_Buffer->getData(), 30 ) == 0 &&
			 memcmp(smb_request2+32, (char *)m_Buffer->getData()+32, 4291-32) == 0 )
		{
			logInfo("Got ASN1 SMB exploit Stage #2(%i) Binding Port 8721\n",m_Buffer->getSize());
			m_Buffer->cut(sizeof(smb_request1));

			Socket *socket;
			if ( (socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,8721,60,30)) == NULL )
			{
				logCrit("%s","Could not bind socket 8721 \n");
				return CL_DROP;
			}

			DialogueFactory *diaf;
			if ( (diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL )
			{
				logCrit("%s","No WinNTShell DialogueFactory availible \n");
				return CL_DROP;
			}

			socket->addDialogueFactory(diaf);
			return CL_DROP;
		}else
		{
			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
									   msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());

			sch_result res = msg->getSocket()->getNepenthes()->getShellcodeMgr()->handleShellcode(&Msg);
			delete Msg;
			if ( res == SCH_DONE )
			{
				m_State = SMB_DONE;
				return CL_ASSIGN_AND_DONE;
			}
		}
		break;

	case SMB_DONE:
		break;
	}

	return CL_UNSURE;
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
ConsumeLevel SMBDialogue::outgoingData(Message *msg)
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
ConsumeLevel SMBDialogue::handleTimeout(Message *msg)
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
ConsumeLevel SMBDialogue::connectionLost(Message *msg)
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
ConsumeLevel SMBDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void SMBDialogue::syncState(ConsumeLevel cl)
{
	logPF();
	switch (cl)
	{
	case CL_ASSIGN_AND_DONE:
	case CL_ASSIGN:
		if (getConsumeLevel() != cl)
		{
			m_State = SMB_DONE;
		}
		break;

	default:
		break;
	}
	
}
