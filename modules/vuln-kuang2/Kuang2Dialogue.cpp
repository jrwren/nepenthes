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

#include "Kuang2Dialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"


#include "Download.hpp"
#include "Download.cpp"

#include "DownloadBuffer.hpp"
#include "DownloadBuffer.cpp"

#include "DownloadUrl.hpp"
#include "DownloadUrl.cpp"

#include "SubmitManager.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the Kuang2Dialogue, creates a new Kuang2Dialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
Kuang2Dialogue::Kuang2Dialogue(Socket *socket)
{
	m_Socket = socket;
	m_DialogueName = "Kuang2Dialogue";
	m_DialogueDescription = "emulates the kuang2 backdoor";

	m_ConsumeLevel = CL_ASSIGN;

	m_State = KUANG2_NONE;
	m_Buffer = new Buffer(64);

	m_Download = NULL;

	Kuang2Message msg;
	memset(&msg,0,sizeof(Kuang2Message));
	msg.command = K2_HELO;
	memcpy(msg.sdata,"foo & bar",strlen("foo & bar"));

	m_Socket->doRespond((char *)&msg,12);

}

Kuang2Dialogue::~Kuang2Dialogue()
{
	delete m_Buffer;

	if ( m_Download != NULL )
	{
		delete m_Download;
	}
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
ConsumeLevel Kuang2Dialogue::incomingData(Message *msg)
{
	switch ( m_State )
	{
	case KUANG2_NONE:
		{

			m_Buffer->add((char *)msg->getMsg(),msg->getSize());
			Kuang2Message *kmsg = (Kuang2Message *)m_Buffer->getData();
			if ( kmsg->command == K2_UPLOAD_FILE )
			{
				// reply OK
				Kuang2Message rkmsg;
				memset(&rkmsg,0,sizeof(Kuang2Message));
				rkmsg.command = K2_DONE;
				msg->getResponder()->doRespond((char *)&rkmsg,4);

				m_FileSize = kmsg->param;
				m_FileName = kmsg->sdata;
				logInfo("Kuang2 File upload requested %s %i\n",m_FileName.c_str(),m_FileSize);
				m_State = KUANG2_FILETRANSFERR;
				m_Download = new Download(msg->getLocalHost(),(char*)"kuang2://foo/bar",msg->getRemoteHost(),(char*)"some triggerline");
				m_Buffer->clear();

			} else
			if ( kmsg->command == K2_RUN_FILE )
			{
				Kuang2Message rkmsg;
				memset(&rkmsg,0,sizeof(Kuang2Message));
				rkmsg.command = K2_DONE;
				msg->getResponder()->doRespond((char *)&rkmsg,4);

				logInfo("Kuang2 File execution requested %s \n",kmsg->sdata);
				m_Buffer->clear();
			} else
			if ( kmsg->command == K2_QUIT )
			{
				logInfo("Kuang2 QUIT requested %s \n",kmsg->sdata);
				return CL_DROP;
			} else
			if ( m_Buffer->getSize() > 128 )
			{
				logCrit("unhandeld kuang2 command \n");
            	return CL_DROP;
			}
		}
		break;

	case KUANG2_FILETRANSFERR:
		{
			m_Download->getDownloadBuffer()->addData((char *)msg->getMsg(),msg->getSize());
			if ( m_Download->getDownloadBuffer()->getSize() == m_FileSize )
			{
				// reply its drone
				Kuang2Message kmsg;
				memset(&kmsg,0,sizeof(Kuang2Message));
				kmsg.command = K2_DONE;
				msg->getResponder()->doRespond((char *)&kmsg,4);
				m_State = KUANG2_NONE;

				// submit file
				g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
			}
		}
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
ConsumeLevel Kuang2Dialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
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
ConsumeLevel Kuang2Dialogue::handleTimeout(Message *msg)
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
ConsumeLevel Kuang2Dialogue::connectionLost(Message *msg)
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
ConsumeLevel Kuang2Dialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

