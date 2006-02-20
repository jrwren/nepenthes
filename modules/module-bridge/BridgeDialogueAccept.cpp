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

#include "BridgeDialogueAccept.hpp"
#include "BridgeDialogueConnect.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"

#include "Message.hpp"


#include "ShellcodeManager.hpp"

#include "Config.hpp"

#include "Download.hpp"

#include "Utilities.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_dia | l_hlr

using namespace nepenthes;


/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the BridgeDialogueAccept, creates a new BridgeDialogueAccept
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
BridgeDialogueAccept::BridgeDialogueAccept(Socket *socket, Socket *bridgesocket)
{
	m_Socket = socket;
	m_BridgeSocket = bridgesocket;
	m_DialogueName = "BridgeDialogueAccept";
	m_DialogueDescription = "accepts the connection for the bridge";

	m_ConsumeLevel = CL_ASSIGN;
	m_Buffer = new Buffer;
	m_State = 0;
}

BridgeDialogueAccept::~BridgeDialogueAccept()
{
	if ( m_BridgeDialogue != NULL )
	{
		((BridgeDialogueAccept*) m_BridgeDialogue)->setBridge(NULL);
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
ConsumeLevel BridgeDialogueAccept::incomingData(Message *msg)
{
	logPF();

	g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());         

	ConsumeLevel cl = CL_ASSIGN;

	m_Buffer->add(msg->getMsg(),msg->getSize());
	Message *Msg = new Message(
							  (char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
							  msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket()
							  );

	sch_result res = msg->getSocket()->getNepenthes()->getShellcodeMgr()->handleShellcode(&Msg);
	delete Msg;
	if ( res == SCH_DONE )
	{
		cl= CL_ASSIGN_AND_DONE;
	}
	if ( m_BridgeDialogue != NULL )
	{
		((BridgeDialogueConnect *)m_BridgeDialogue)->receivePartCompleted();
		m_BridgeDialogue->getSocket()->doWrite(msg->getMsg(),msg->getSize());
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
ConsumeLevel BridgeDialogueAccept::outgoingData(Message *msg)
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
ConsumeLevel BridgeDialogueAccept::handleTimeout(Message *msg)
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
ConsumeLevel BridgeDialogueAccept::connectionLost(Message *msg)
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
ConsumeLevel BridgeDialogueAccept::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void BridgeDialogueAccept::setBridge(Dialogue *dia)
{
	m_BridgeDialogue = dia;
}

void BridgeDialogueAccept::receivePartCompleted()
{
	logPF();
	void *buf = m_Buffer->getData();
	int32_t len = m_Buffer->getSize();
	if (len > 0)
	{
    	string hash = g_Nepenthes->getUtilities()->md5sum((char *)buf,len);
		logInfo("Received Part %i size %i hash %s\n",m_State,len,hash.c_str());
		m_State++;
		m_Buffer->clear();
	}
}
