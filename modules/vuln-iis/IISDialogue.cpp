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
#include "vuln-iis.hpp"
#include "iis-shellcodes.h"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"

#include "Utilities.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "ShellcodeManager.hpp"

#include "Message.hpp"
#include "Message.cpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

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
	m_DialogueDescription = "the ssl waekness in iis";

	m_ConsumeLevel = CL_ASSIGN;

	m_Buffer = new Buffer(512);

	m_State = IIS_NULL;
}

IISDialogue::~IISDialogue()
{
	switch (m_State)
	{
	case IIS_NULL:
	case IIS_SSL:
		logWarn("Unknown IIS SSL exploit %i bytes State %i\n",m_Buffer->getSize(), m_State);
		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *) m_Buffer->getData(), m_Buffer->getSize());
		break;

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
	m_Buffer->add(msg->getMsg(),msg->getMsgLen());
//	g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getMsgLen());

	ConsumeLevel cl = CL_ASSIGN;

	switch (m_State)
	{
	case IIS_NULL:
		if (m_Buffer->getSize() >= sizeof(thc_sslshit)  &&
			memcmp(m_Buffer->getData(),thc_sslshit,sizeof(thc_sslshit)) == 0)
		{
			m_State = IIS_SSL;
			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
						msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());
//			g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getMsgLen());

			if ( g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg)  == SCH_DONE )
			{
				m_State = IIS_DONE;
				cl = CL_ASSIGN_AND_DONE;
			}
			delete Msg;
		}
		break;

	case IIS_SSL:
		{
			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
									   msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());
//			g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getMsgLen());

    		if ( g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg)  == SCH_DONE )
			{
				m_State = IIS_DONE;
				cl = CL_ASSIGN_AND_DONE;
			}
			delete Msg;
			

		}
		break;

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
