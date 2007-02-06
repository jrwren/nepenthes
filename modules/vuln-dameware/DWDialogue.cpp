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

#include "DWDialogue.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "Config.hpp"

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
 * construktor for the DWDialogue, creates a new DWDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
DWDialogue::DWDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "DWDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	// pretend to be winxp
	char buff[64];
	memset(buff,0,64);
	buff[8]=5;
	buff[12]=1;
	buff[37]=0;

	m_Socket->doRespond(buff,64);

	m_Buffer = new Buffer(512);

	m_State = DW_NULL;
}

DWDialogue::~DWDialogue()
{
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
ConsumeLevel DWDialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getSize());
//	g_Nepenthes->getUtilities()->hexdump(l_info,(byte *)m_Buffer->getData(),m_Buffer->getSize());
	switch ( m_State )
	{
	case DW_NULL:
		{
			// once again pretend to be winxp sp 0
			char buff[64];
			memset(buff,0,64);
			buff[8]=5;
			buff[12]=1;
			buff[37]=0;

			m_Buffer->clear();
			m_Socket->doRespond(buff,64);
			m_State = DW_STAGE_ONE;
		}
		break;


	case DW_STAGE_ONE:
		{

			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(),m_Socket->getLocalPort(), m_Socket->getRemotePort(),
									   m_Socket->getLocalHost(), m_Socket->getRemoteHost(), m_Socket, m_Socket);

			sch_result sch;
			sch = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg);
			delete Msg;

			if ( sch == SCH_DONE )
			{
				m_State = DW_DONE;
				return CL_ASSIGN_AND_DONE;
			}
			
		}
		break;

	case DW_DONE:
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
ConsumeLevel DWDialogue::outgoingData(Message *msg)
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
ConsumeLevel DWDialogue::handleTimeout(Message *msg)
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
ConsumeLevel DWDialogue::connectionLost(Message *msg)
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
ConsumeLevel DWDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void DWDialogue::dump()
{
	logWarn("Unknown %s Shellcode (Buffer %i bytes) (State %i)\n","DameWare",m_Buffer->getSize(),m_State);
	HEXDUMP(m_Socket,(byte *)m_Buffer->getData(),m_Buffer->getSize());
}
