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

#include "WatchDialogue.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "ShellcodeManager.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"

#include "Utilities.hpp"

#include "EventManager.hpp"
#include "SocketEvent.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;


/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the WatchDialogue, creates a new WatchDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
WatchDialogue::WatchDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "WatchDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_Buffer = new Buffer(512);
}

WatchDialogue::~WatchDialogue()
{
	logWarn("Unknown WatchDialogue %i bytes, port %i\n",m_Buffer->getSize(), m_Socket->getLocalPort());		
	HEXDUMP(m_Socket,(byte *) m_Buffer->getData(), m_Buffer->getSize());
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
ConsumeLevel WatchDialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getSize());
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
ConsumeLevel WatchDialogue::outgoingData(Message *msg)
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
ConsumeLevel WatchDialogue::handleTimeout(Message *msg)
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
ConsumeLevel WatchDialogue::connectionLost(Message *msg)
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
ConsumeLevel WatchDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}


