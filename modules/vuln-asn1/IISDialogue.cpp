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

#include "IISDialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "ShellcodeManager.hpp"

#include "Buffer.hpp"
#include "Utilities.hpp"

#include "Socket.hpp"

#include "EventManager.hpp"
#include "SocketEvent.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia

#include <cstring>

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the IISDialogue, creates a new IISDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
IISDialogue::IISDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "IISDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_Buffer = new Buffer(0);

	m_State = IIS_NULL;
}

IISDialogue::~IISDialogue()
{
	switch(m_State)
	{
	case IIS_NULL:
	case IIS_POST:
	case IIS_GET:
		logWarn("Unknown IIS %i bytes State %i\n",m_Buffer->getSize(), m_State);		
		HEXDUMP(m_Socket,(byte *) m_Buffer->getData(), m_Buffer->getSize());
		break;

	case IIS_SEARCH:
	case IIS_DONE:
	
		break;
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
ConsumeLevel IISDialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getSize());
//	HEXDUMP(m_Socket,(byte *) m_Buffer->getData(), m_Buffer->getSize());

	// FIXME this can only recognize urldownloadtofile foobar

	ConsumeLevel cl = CL_ASSIGN;

	if(m_State ==  IIS_NULL)
	{
    		if ( m_Buffer->getSize() >= 6 &&  strncmp((char *)m_Buffer->getData(),"SEARCH",6) == 0 )
			{
				m_State = IIS_SEARCH;
			} else
			if ( m_Buffer->getSize() >= 4 &&  strncmp((char *)m_Buffer->getData(),"POST",4) == 0 )
			{
				m_State = IIS_POST;

			} else
			if ( m_Buffer->getSize() >= 3 &&  strncmp((char *)m_Buffer->getData(),"GET",3) == 0 )
			{
				m_State = IIS_GET; // we fake this here
			} else
			{
				return CL_DROP;
			}
	}

	switch (m_State)
	{
    
	case IIS_POST:
	case IIS_GET:
		{
			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
							   msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());
			if ( msg->getSocket()->getNepenthes()->getShellcodeMgr()->handleShellcode(&Msg) == SCH_DONE )
			{
				m_State = IIS_DONE;
				cl=CL_ASSIGN_AND_DONE;
			}
			delete Msg;
		}
		break;

	case IIS_NULL:
	case IIS_SEARCH:
	case IIS_DONE:
		break;

	}

	return cl;
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
ConsumeLevel IISDialogue::outgoingData(Message *msg)
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
ConsumeLevel IISDialogue::handleTimeout(Message *msg)
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
ConsumeLevel IISDialogue::connectionLost(Message *msg)
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
ConsumeLevel IISDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}


void IISDialogue::syncState(ConsumeLevel cl)
{
	logPF();
	switch (cl)
	{
	case CL_ASSIGN_AND_DONE:
	case CL_ASSIGN:
		if (getConsumeLevel() != cl)
		{
			m_State = IIS_DONE;
		}
		break;

	default:
		break;
	}
}
