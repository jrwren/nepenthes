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

#include "LSASSDialogue.hpp"
#include "lsass-shellcodes.hpp"

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


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the LSASSDialogue, creates a new LSASSDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
LSASSDialogue::LSASSDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "LSASSDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_UNSURE;
	m_Buffer = new Buffer(1024);
	m_State = LSASS_HOD_STAGE1;
}

LSASSDialogue::~LSASSDialogue()
{
	switch (m_State)
	{
	case LSASS_DONE:
		break;

	default:
		logWarn("Unknown %s Shellcode (Buffer %i bytes) (State %i)\n","LSASS",m_Buffer->getSize(),m_State);
		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *)m_Buffer->getData(),m_Buffer->getSize());
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
 * @return CL_ASSIGN if we can be sure this is lsass, else
 *  CL_UNSURE
 */
ConsumeLevel LSASSDialogue::incomingData(Message *msg)
{
	logPF();

	 m_Buffer->add(msg->getMsg(),msg->getMsgLen());

 
	 char reply[512];
	 for (int i=0;i<512;i++)
	 {
		 reply[i] = rand()%255;
	 }
	 switch (m_State)
	 {
	 case LSASS_HOD_STAGE1:
		 if (m_Buffer->getSize() >= sizeof(lsass_hod_req1) -1)
		 {
			 if (memcmp(lsass_hod_req1,m_Buffer->getData(),sizeof(lsass_hod_req1) -1) == 0 )
			 {
				 logInfo("Valid LSASS HOD Stage #1 (%i %i)\n",sizeof(lsass_hod_req1), m_Buffer->getSize());
				 m_State = LSASS_HOD_STAGE2;
				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,64);
				 return CL_UNSURE;	// same as asn1 
			 }else
				 return CL_DROP;
		 }
		 break;

	 case LSASS_HOD_STAGE2:
		 if (m_Buffer->getSize() >= sizeof(lsass_hod_req2) -1)
		 {
			 if (memcmp(lsass_hod_req2,m_Buffer->getData(),sizeof(lsass_hod_req2) -1) == 0 )
			 {
				 logInfo("Valid LSASS HOD Stage #2 (%i)\n",sizeof(lsass_hod_req2));
				 m_State = LSASS_HOD_STAGE3;
				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,64);
				 return CL_ASSIGN;
			 }else
				 return CL_DROP;
		 }

		 break;
		 
	 case LSASS_HOD_STAGE3:
		 if (m_Buffer->getSize() >= sizeof(lsass_hod_req3) -1)
		 {
			 if (memcmp(lsass_hod_req3,m_Buffer->getData(),sizeof(lsass_hod_req3) -1) == 0 )
			 {
				 logInfo("Valid LSASS HOD Stage #3 (%i)\n",sizeof(lsass_hod_req3));
				 m_State = LSASS_HOD_STAGE4;
				 m_Buffer->clear();
                 char *osversion = "W i n d o w s   5 . 1 ";
				 memcpy(reply+48,osversion,strlen(osversion));
				 msg->getResponder()->doRespond(reply,256);
				 return CL_ASSIGN;
			 }else
				 return CL_DROP;
		 }
		 break;

	 case LSASS_HOD_STAGE4:
		 logInfo("Checking LSASS HOD Stage #4 (%i)\n",sizeof(lsass_hod_req4));
		 if (m_Buffer->getSize() >= 50)
		 {
			 if (1)//memcmp(lsass_hod_req4+10,((char *)m_Buffer->getData())+10,10) == 0 )
			 {
				 logInfo("Valid LSASS HOD Stage #4 (%i)\n",sizeof(lsass_hod_req4));
				 m_State = LSASS_HOD_STAGE5;
				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,64);
				 return CL_ASSIGN;
			 }else
				 return CL_DROP;
		 }
		 break;

	 case LSASS_HOD_STAGE5:
		 if (m_Buffer->getSize() >= sizeof(lsass_hod_req5) -1)
		 {
			 if (memcmp(lsass_hod_req5,m_Buffer->getData(),sizeof(lsass_hod_req5) -1) == 0 )
			 {
				 logInfo("Valid LSASS HOD Stage #5 (%i)\n",sizeof(lsass_hod_req5));
				 m_State = LSASS_HOD_STAGE6;
				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,64);
				 return CL_ASSIGN;

			 }else
				 return CL_DROP;
		 }
		 break;

	 case LSASS_HOD_STAGE6:
		 if (m_Buffer->getSize() >= sizeof(lsass_hod_req6) -1)
		 {
			 if (memcmp(lsass_hod_req6,m_Buffer->getData(),sizeof(lsass_hod_req6) -1) == 0 )
			 {
				 logInfo("Valid LSASS HOD Stage #6 (%i)\n",sizeof(lsass_hod_req6));
				 m_State = LSASS_HOD_REST;
				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,64);
				 return CL_ASSIGN;
			 }else
				 return CL_DROP;
		 }
		 break;

	 case LSASS_HOD_REST:
		 {
			 msg->getResponder()->doRespond(reply,64);
			 Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(), msg->getLocalPort(), msg->getRemotePort(),
						  msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());
//			  g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getMsgLen());
			  sch_result result = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg);
			  delete Msg;
			  if (result == SCH_DONE )
			  {
				  m_State = LSASS_DONE;
               	  return CL_ASSIGN_AND_DONE;
			  }
			  else
			  {
              	  return CL_ASSIGN;
			  }

		 }
		 break;

	 case LSASS_DONE:
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
ConsumeLevel LSASSDialogue::outgoingData(Message *msg)
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
ConsumeLevel LSASSDialogue::handleTimeout(Message *msg)
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
ConsumeLevel LSASSDialogue::connectionLost(Message *msg)
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
ConsumeLevel LSASSDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}


void LSASSDialogue::syncState(ConsumeLevel cl)
{
	logPF();

	switch (cl)
	{
	case CL_ASSIGN_AND_DONE:
	case CL_ASSIGN:
		if (getConsumeLevel() != cl)
		{
			logSpam(" STATE %i -> %i \n",m_State,LSASS_DONE);
			m_State = LSASS_DONE;
		}
		break;

	default:
		break;
	}
}

