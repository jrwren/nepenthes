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

#include "MSDTCDialogue.hpp"
#include "Message.hpp"
#include "Message.cpp"
#include "LogManager.hpp"
#include "Buffer.hpp"
#include "Buffer.cpp"
#include "vuln-msdtc.hpp"
#include "msdtc-shellcodes.hpp"
#include "ShellcodeManager.hpp"
#include "Utilities.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia | l_hlr


using namespace nepenthes;


MSDTCDialogue::MSDTCDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "MSDTCDialogue";
	m_DialogueDescription = "MSDTC Vuln Dialogue";

	m_ConsumeLevel = CL_UNSURE;
	m_State = MSDTC_STATE_NULL;

	m_Buffer = new Buffer(1024);
}

MSDTCDialogue::~MSDTCDialogue()
{
	delete m_Buffer;
}

ConsumeLevel MSDTCDialogue::incomingData(Message *msg)
{
	logPF();
	
	m_Buffer->add(msg->getMsg(),msg->getSize());

	ConsumeLevel cl = CL_UNSURE;
//	g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());
	char reply[512];
	for (int32_t i=0;i<512;i++)
	{
		reply[i] = rand()%255;
	}




	switch(m_State)
	{
	case MSDTC_STATE_NULL:
		if(m_Buffer->getSize() >= sizeof(msdtc_request_0) ) 
		{
			if ( memcmp(msdtc_request_0, m_Buffer->getData(), sizeof(msdtc_request_0)) == 0 )
			{
				logSpam("MSDTC STATE #1 packet %i %i\n",m_Buffer->getSize(),sizeof(msdtc_request_0));
				m_State = MSDTC_STATE_REQUEST_1;
				cl = CL_ASSIGN;
				m_Buffer->cut(sizeof(msdtc_request_0));
				m_Socket->doRespond(reply,64);
			}
		}else
		{
			logSpam("MSDTC dropping in state %i\n",m_State);
			cl = CL_DROP;
		}
		break;

	case MSDTC_STATE_REQUEST_1:
		if(
		   m_Buffer->getSize() >= sizeof(msdtc_request_1) 
		   && 
		   memcmp(msdtc_request_1, m_Buffer->getData(), 15*8 ) == 0 && 
		   memcmp(msdtc_request_1+15*8+4, (char *)m_Buffer->getData()+15*8+4, sizeof(msdtc_request_1)-15*8-4 ) == 0 
		   )
		{
			logSpam("MSDTC STATE #2.1 packet %i %i %i\n",m_Buffer->getSize(),sizeof(msdtc_request_1), *(int32_t *)m_Buffer->getData()+15*8);
			m_Buffer->cut(sizeof(msdtc_request_1));
		}

		if ( 
		   m_Buffer->getSize() >= sizeof(msdtc_request_2) &&
		   memcmp(msdtc_request_2, m_Buffer->getData(), sizeof(msdtc_request_2)  ) == 0 )
		{
			logSpam("MSDTC STATE #2.2 packet %i %i\n",m_Buffer->getSize(),sizeof(msdtc_request_2));

			m_Buffer->cut(sizeof(msdtc_request_2));

			reply[8] = 0x5c;
			m_Socket->doRespond(reply,64);

			m_State = MSDTC_STATE_DONE;
			cl = CL_ASSIGN_AND_DONE;

		}
		else
		{
			Message *Msg = new Message(
				(char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
				msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket()
				);

			sch_result res = msg->getSocket()->getNepenthes()->getShellcodeMgr()->handleShellcode(&Msg);
			delete Msg;
			if ( res == SCH_DONE )
			{
				m_State = MSDTC_STATE_DONE;
				cl = CL_ASSIGN_AND_DONE;

				reply[8] = 0x5c;
				m_Socket->doRespond(reply,64);
			}
		}


		break;

	case MSDTC_STATE_DONE:
		break;

	}
	return cl;
}

ConsumeLevel MSDTCDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

ConsumeLevel MSDTCDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel MSDTCDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel MSDTCDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void MSDTCDialogue::dump()
{
	logWarn("Unknown %s Shellcode (Buffer %i bytes) (State %i)\n","MSDTC",m_Buffer->getSize(),m_State);
	g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *)m_Buffer->getData(),m_Buffer->getSize());
}
