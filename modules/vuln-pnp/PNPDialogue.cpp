
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

#include "PNPDialogue.hpp"
#include "pnp-shellcodes.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "ShellcodeHandler.hpp"
#include "Utilities.hpp"

#include "Message.hpp"
#include "Message.cpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "EventManager.hpp"
#include "SocketEvent.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the PNPDialogue, creates a new PNPDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
PNPDialogue::PNPDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "PNPDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_UNSURE;
	m_Buffer = new Buffer(1024);
	m_State = PNP_HOD_STAGE1;
}

PNPDialogue::~PNPDialogue()
{
	delete m_Buffer;
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN if we can be sure this is lsass, else
 *  CL_UNSURE
 */
ConsumeLevel PNPDialogue::incomingData(Message *msg)
{
	logPF();

	 m_Buffer->add(msg->getMsg(),msg->getSize());

 
	 char reply[512];
	 for (int32_t i=0;i<512;i++)
	 {
		 reply[i] = rand()%255;
	 }
	 switch (m_State)
	 {
	 case PNP_HOD_STAGE1:
		if ( m_Buffer->getSize() >= sizeof(pnp_hod_req1) -1 )
		{
			if ( memcmp(pnp_hod_req1,m_Buffer->getData(),sizeof(pnp_hod_req1) -1) == 0 )
			{
				logDebug("Valid LSASS PNP Stage #1 (%i %i)\n",sizeof(pnp_hod_req1), m_Buffer->getSize());
				m_State = PNP_HOD_STAGE2;
				m_Buffer->clear();
//				reply[9]=0;
//				msg->getResponder()->doRespond(reply,64);
				return CL_UNSURE; // same as asn1 
			} else
				return CL_DROP;
		}

		break;

	 case PNP_HOD_STAGE2:
		 if ( m_Buffer->getSize() >= sizeof(pnp_hod_req2) -1 )
		 {
			  if ( memcmp(pnp_hod_req2,m_Buffer->getData(),sizeof(pnp_hod_req2) -1) == 0 )
			  {
				  logDebug("Valid LSASS PNP Stage #2 (%i %i)\n",sizeof(pnp_hod_req2), m_Buffer->getSize());
				  m_State = PNP_HOD_STAGE3;
				  m_Buffer->clear();
//				  reply[9]=0;
//				  msg->getResponder()->doRespond(reply,64);
				  return CL_UNSURE; // same as asn1 
			  } else
				  return CL_DROP;
		  }

		 break;
		 
	 case PNP_HOD_STAGE3:
		 if ( m_Buffer->getSize() >= sizeof(pnp_hod_req3) -1 )
		 {
			  if ( memcmp(pnp_hod_req3,m_Buffer->getData(),sizeof(pnp_hod_req3) -1) == 0 )
			  {
				  logDebug("Valid LSASS PNP Stage #3 (%i %i)\n",sizeof(pnp_hod_req3), m_Buffer->getSize());
				  m_State = PNP_HOD_STAGE4;
				  m_Buffer->clear();
				  reply[9]=0;
				  msg->getResponder()->doRespond(reply,64);
				  return CL_UNSURE; // same as asn1 
			  } else
				  return CL_DROP;
		  }
		 break;

	 case PNP_HOD_STAGE4:
		 logDebug("PNP Stage #4 %i\n",m_Buffer->getSize());
		 m_State = PNP_HOD_STAGE5;
		 m_Buffer->clear();
		 reply[9]=0;
		 msg->getResponder()->doRespond(reply,64);
		 return CL_UNSURE; // same as asn1 
		 break;

	 case PNP_HOD_STAGE5:
		 if ( m_Buffer->getSize() >= sizeof(pnp_hod_req5) -1 )
		  {
			   if ( memcmp(pnp_hod_req5,m_Buffer->getData(),sizeof(pnp_hod_req5) -1) == 0 )
			   {
				   logDebug("Valid LSASS PNP Stage #5 (%i %i)\n",sizeof(pnp_hod_req5), m_Buffer->getSize());
				   m_State = PNP_HOD_STAGE6;
				   m_Buffer->clear();
				   reply[9]=0;
				   msg->getResponder()->doRespond(reply,64);
				   return CL_ASSIGN; // same as asn1 
			   } else
				   return CL_DROP;
		   }

		 break;

	 case PNP_HOD_STAGE6:
		 if ( m_Buffer->getSize() >= sizeof(pnp_hod_req6) -1 )
		 {
			  if ( memcmp(pnp_hod_req6,m_Buffer->getData(),sizeof(pnp_hod_req6) -1) == 0 )
			  {
				  logDebug("Valid LSASS PNP Stage #6 (%i %i)\n",sizeof(pnp_hod_req6), m_Buffer->getSize());
				  m_State = PNP_HOD_REST;
				  m_Buffer->clear();
				  reply[9]=0;
				  msg->getResponder()->doRespond(reply,64);
				  return CL_ASSIGN; // same as asn1 
			  } else
				  return CL_DROP;
		  }

		 break;

	 case PNP_HOD_REST:
		 {
         	 msg->getResponder()->doRespond(reply,64);
			 Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
						  msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());
//			  g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());
			  sch_result result = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg);
			  delete Msg;
			  if (result == SCH_DONE )
			  {
				  m_State = PNP_DONE;
               	  return CL_ASSIGN_AND_DONE;
			  }
			  else
			  {
              	  return CL_ASSIGN;
			  }
		 }
		 break;

	 case PNP_DONE:
		 break;

	 }

	 return CL_UNSURE;
}

/**
 * Dialogue::outgoingData(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel PNPDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

/**
 * Dialogue::handleTimeout(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel PNPDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionLost(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel PNPDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel PNPDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}


void PNPDialogue::dump()
{
		logWarn("Unknown %s Shellcode (Buffer %i bytes) (State %i)\n","PNP",m_Buffer->getSize(),m_State);
		HEXDUMP(m_Socket,(byte *)m_Buffer->getData(),m_Buffer->getSize());
}
