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

#include "DCOMDialogue.hpp"
#include "Message.hpp"
#include "Message.cpp"
#include "LogManager.hpp"
#include "Buffer.hpp"
#include "Buffer.cpp"
#include "vuln-dcom.hpp"
#include "dcom-shellcodes.h"
#include "ShellcodeManager.hpp"
#include "Utilities.hpp"


#include "EventManager.hpp"
#include "SocketEvent.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia | l_hlr


using namespace nepenthes;


DCOMDialogue::DCOMDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "DCOMDialogue";
	m_DialogueDescription = "DCOM Vuln Dialogue";

	m_ConsumeLevel = CL_UNSURE;
	m_State = DCOM_STATE_NULL;

	m_Buffer = new Buffer(1024);
}

DCOMDialogue::~DCOMDialogue()
{
	delete m_Buffer;
}

ConsumeLevel DCOMDialogue::incomingData(Message *msg)
{
	logPF();

	m_Buffer->add(msg->getMsg(),msg->getSize());

//	g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());
	char reply[512];
	for (int32_t i=0;i<512;i++)
	{
		reply[i] = rand()%255;
	}


	ConsumeLevel cl = CL_UNSURE;

	switch(m_State)
	{
	case DCOM_STATE_NULL:
		if(m_Buffer->getSize() >= sizeof(dcom_bindstr) - 1 && memcmp(dcom_bindstr, m_Buffer->getData(), sizeof(dcom_bindstr) - 1) == 0)
		{
			logSpam("Valid classic DCOM BindString (%i).\n",sizeof(dcom_bindstr));
			m_Buffer->clear();
			m_State = DCOM_STATE_BINDSTR;

			// we have to reply that we accept the bind string
			reply[2] = DCE_PKT_BINDACK;
			msg->getResponder()->doRespond(reply,64);

			return CL_ASSIGN;
		} else
		if(m_Buffer->getSize() >= sizeof(dcom2_bindstr) && 
		   memcmp(dcom2_bindstr, m_Buffer->getData(), sizeof(dcom2_bindstr) ) == 0)
		{
			logSpam("Valid DCOM2 BindString.\n");
			m_Buffer->cut(sizeof(dcom2_bindstr));
			m_State = DCOM_STATE_BINDSTR;

			// we have to reply that we accept the bind string
			reply[2] = DCE_PKT_BINDACK;
			msg->getResponder()->doRespond(reply,64);

			return CL_ASSIGN;
		}else
		if	(m_Buffer->getSize() >= sizeof(sol2k_request) -1  && 
			 memcmp(sol2k_request, m_Buffer->getData(), sizeof(sol2k_request)-1 ) == 0)
		{	
				logSpam("Valid sol2k request %i.\n", sizeof(sol2k_request) -1);
				m_State = DCOM_SOL2k_REQUEST;

		}else
		if ( m_Buffer->getSize() >= sizeof(unknown_req1)  &&
			 memcmp(unknown_req1, m_Buffer->getData(), sizeof(unknown_req1) ) == 0 )
		{
			logDebug("Valid UNKNOWN request #1 %i.\n", sizeof(unknown_req1) );
			m_State = DCOM_STATE_BINDSTR;
			m_Buffer->cut(sizeof(unknown_req1));

			reply[2] = DCE_PKT_BINDACK;
			reply[8] = 64;
			msg->getResponder()->doRespond(reply,64);
		}else
		if ( m_Buffer->getSize() >= sizeof(ntscan_req1)  &&
			 memcmp(ntscan_req1, m_Buffer->getData(), sizeof(ntscan_req1) ) == 0 )
		{
			logSpam("Valid NTSCAN request #1 %i  (dropping this shit).\n", sizeof(ntscan_req1));
			return CL_DROP;
		} else
		{
			logInfo("Unknown DCOM request, dropping\n");
			return CL_DROP;
/*

            logSpam("Unknown DCOM request, repling %i bytes crap\n",64);
			reply[2] = DCE_PKT_BINDACK;
			reply[8] = 64;
			msg->getResponder()->doRespond(reply,64);

			Message *Msg = new Message(
				(char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
				msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket()
				);

			sch_result res = msg->getSocket()->getNepenthes()->getShellcodeMgr()->handleShellcode(&Msg);
			delete Msg;
			if ( res == SCH_DONE )
			{
				m_State = DCOM_DONE;
				cl = CL_ASSIGN_AND_DONE;
			}
*/			
		}

		break;

	case DCOM_STATE_BINDSTR:
		{
			if ( m_Buffer->getSize() >= sizeof(rpcfp_inqifids) -1  && 
				 memcmp(sol2k_request, m_Buffer->getData(), sizeof(rpcfp_inqifids)-1 ) == 0 )
			{
				logDebug("recognized OS version check\n");
				// we have to send a valid os response reply
				reply[2] = DCE_PKT_RESPONSE;
				memcpy(reply+47,w2kuuid_sig,sizeof(w2kuuid_sig));
				msg->getResponder()->doRespond(reply,364);
				cl =  CL_ASSIGN;
			}else
			if ( m_Buffer->getSize() >= sizeof(dcom_unknown_req2)  && 
				 memcmp(dcom_unknown_req2, m_Buffer->getData(), sizeof(dcom_unknown_req2) ) == 0 )
			{
				
				logSpam("Got DCOM Bindstr followup with %i %i bytes \n",sizeof(dcom_unknown_req2),m_Buffer->getSize());
				m_Buffer->clear();
				msg->getResponder()->doRespond(dcom_unknown_rep2,sizeof(dcom_unknown_rep2));
			}


			Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
									   msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());

			sch_result res = msg->getSocket()->getNepenthes()->getShellcodeMgr()->handleShellcode(&Msg, "Generic Microsoft Windows DCOM");
			delete Msg;

			if ( res == SCH_DONE )
			{
				reply[2] = DCE_PKT_FAULT;
				memcpy(reply+47,w2kuuid_sig,sizeof(w2kuuid_sig));
				msg->getResponder()->doRespond(reply,364);
				m_State = DCOM_DONE;
				cl =CL_ASSIGN_AND_DONE;

			}
		}
		break;


	case DCOM_SOL2k_REQUEST:
		break;

	case DCOM_DONE:
		break;

	}
	return cl;
}

ConsumeLevel DCOMDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

ConsumeLevel DCOMDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel DCOMDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel DCOMDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void DCOMDialogue::dump()
{
	logWarn("Unknown %s Shellcode (Buffer %i bytes) (State %i)\n","DCOM",m_Buffer->getSize(),m_State);
	HEXDUMP(m_Socket,(byte *)m_Buffer->getData(),m_Buffer->getSize());
}
