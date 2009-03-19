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

#include "WINSDialogue.hpp"
#include "Message.hpp"
#include "Message.cpp"
#include "LogManager.hpp"
#include "Buffer.hpp"
#include "Buffer.cpp"
#include "vuln-wins.hpp"
//#include "wins-shellcodes.h"
#include "ShellcodeManager.hpp"

#include "Utilities.hpp"

#include "Socket.hpp"

#include "EventManager.hpp"
#include "SocketEvent.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia | l_hlr


using namespace nepenthes;


WINSDialogue::WINSDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "WINSDialogue";
	m_DialogueDescription = "WINS Vuln Dialogue";

	m_ConsumeLevel = CL_UNSURE;
	m_Buffer = new Buffer(1024);

	m_State = WINS_NULL;
}

WINSDialogue::~WINSDialogue()
{
	delete m_Buffer;
}

ConsumeLevel WINSDialogue::incomingData(Message *msg)
{
	logPF();
	m_Buffer->add(msg->getMsg(),msg->getSize());


	switch (m_State)
	{
	case WINS_NULL:
		{
			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(),m_Socket->getLocalPort(), m_Socket->getRemotePort(),
					m_Socket->getLocalHost(), m_Socket->getRemoteHost(), m_Socket, m_Socket);
			sch_result res = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg, "Generic Microsoft Windows WINS");
			delete Msg;

			if ( res == SCH_DONE )
			{
				m_State = WINS_DONE;
				return CL_ASSIGN_AND_DONE;
			}
			

		}
		break;

	case WINS_DONE:
		break;

	}
	return CL_ASSIGN;
}

ConsumeLevel WINSDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

ConsumeLevel WINSDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel WINSDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel WINSDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void WINSDialogue::dump()
{

	logWarn("WINS unknown shellcode %i bytes State 0\n",m_Buffer->getSize());
	HEXDUMP(m_Socket,(byte *) m_Buffer->getData(), m_Buffer->getSize());
}
