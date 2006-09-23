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

#include "BridgeDialogueConnect.hpp"
#include "BridgeDialogueAccept.hpp"

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
 * construktor for the BridgeDialogueConnect, creates a new BridgeDialogueConnect
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
BridgeDialogueConnect::BridgeDialogueConnect(Socket *socket, Socket *bridgesocket)
{
	m_Socket = socket;
	m_AcceptSocket = bridgesocket;
	m_ConsumeLevel = CL_ASSIGN;
	m_DialogueName = "BridgeDialogueConnect";
	m_DialogueDescription = "connects the remote for the bridge";
	m_State = 0;

	m_Buffer = new Buffer;

}

BridgeDialogueConnect::~BridgeDialogueConnect()
{
	if (m_AcceptDialogue != NULL)
	{
		((BridgeDialogueAccept*) m_AcceptDialogue)->setBridge(NULL);
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
ConsumeLevel BridgeDialogueConnect::incomingData(Message *msg)
{
	logPF();
//	g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());

//	m_Buffer->add(msg->getMsg(),msg->getSize());

	if (m_AcceptDialogue != NULL)
	{
//		((BridgeDialogueAccept *)m_AcceptDialogue)->receivePartCompleted();
		m_AcceptDialogue->getSocket()->doWrite(msg->getMsg(),msg->getSize());
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
ConsumeLevel BridgeDialogueConnect::outgoingData(Message *msg)
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
ConsumeLevel BridgeDialogueConnect::handleTimeout(Message *msg)
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
ConsumeLevel BridgeDialogueConnect::connectionLost(Message *msg)
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
ConsumeLevel BridgeDialogueConnect::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void BridgeDialogueConnect::setBridge(Dialogue *dia)
{
	m_AcceptDialogue = dia;
}

void BridgeDialogueConnect::receivePartCompleted()
{
/*	logPF();
	void *buf = m_Buffer->getData();
	int32_t len = m_Buffer->getSize();
	if ( len > 0 )
	{
		string hash = g_Nepenthes->getUtilities()->md5sum((char *)buf,len);
		logInfo("Sended Part %i size %i hash %s\n",m_State,len,hash.c_str());
		m_State++;
		m_Buffer->clear();
	}
*/	
}
