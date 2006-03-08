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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "gotekCTRLDialogue.hpp"
#include "submit-gotek.hpp"

#include "Message.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Download.hpp"
#include "Download.cpp"
#include "DownloadUrl.hpp"
#include "DownloadUrl.cpp"

#include "DownloadBuffer.hpp"
#include "DownloadBuffer.cpp"

#include "Utilities.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * constructor for the gotekCTRLDialogue, creates a new gotekCTRLDialogue
 * 
 * does the control connection for the fabulous G.O.T.E.K. file submission
 * protocol
 * 
 * @param socket the Socket the Dialogue has to use
 */
gotekCTRLDialogue::gotekCTRLDialogue(Socket *socket, string serverHost, GotekSubmitHandler * handlingFather)
{
	m_Socket = socket;
	m_DialogueName = "gotekCTRLDialogue";
	m_DialogueDescription = "G.O.T.E.K. control connection dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_State = GCTRL_NULL;

	m_Buffer = new Buffer(128);
	
	m_ServerHost = serverHost;
	m_HandlingFather = handlingFather;
}

gotekCTRLDialogue::~gotekCTRLDialogue()
{
	delete m_Buffer;
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * callback called upon incoming control data from gotekd
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel gotekCTRLDialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getSize());

	switch (m_State)
	{
	case GCTRL_NULL:	// just connected
		if (m_Buffer->getSize() == 12)
		{
			m_Buffer->cut(4); // protocol version foobar

			unsigned char sessionkey[8];
			memcpy((char *)sessionkey,(char *)m_Buffer->getData(),8);
			g_Nepenthes->getUtilities()->hexdump(sessionkey,8);

			

			// send username
			unsigned char username[32];
			memset(username,0,32);
			string user = g_GotekSubmitHandler->getUser();
			memcpy(username,user.c_str(),user.size()); //size checked in Init()
			m_Socket->doRespond((char *)username,32);


			byte hash[64];
 			byte hashme[1032];
			memset(hashme,0,1032);

			g_Nepenthes->getUtilities()->hexdump(g_GotekSubmitHandler->getCommunityKey(),1024);
			memcpy(hashme,g_GotekSubmitHandler->getCommunityKey(),1024);
			memcpy(hashme+1024,sessionkey,8);
			g_Nepenthes->getUtilities()->hexdump(hashme, 1032);

			g_Nepenthes->getUtilities()->sha512(hashme, 1032, hash);
			g_Nepenthes->getUtilities()->hexdump(hash,64);

			m_Socket->doRespond((char *)hash,64);

			m_Buffer->clear();

			m_State = GCTRL_AUTH;
		}else
		if (m_Buffer->getSize() > 12)
		{
			// notify parent of a protocol problem, close socket
			return CL_DROP;
		}
		break;

	case GCTRL_AUTH:
		if (m_Buffer->getSize() == 1)
		{
			if (*(unsigned char *)m_Buffer->getData() == 0xaa)
			{
				logInfo("Logged into %s\n", m_ServerHost.c_str());
				unsigned char ctrlcon = 0x55;
				m_Socket->doRespond((char *)&ctrlcon,1);
				g_GotekSubmitHandler->setSocket(m_Socket);
				m_State = GCTRL_CTRL;
				m_Buffer->clear();
			}

		}
		break;

	case GCTRL_CTRL:
		while(m_Buffer->getSize() > 0)
		{
			if (*(unsigned char *)m_Buffer->getData() == 0xaa) 		// new file
			{
				g_GotekSubmitHandler->sendGote();
				m_Buffer->cut(1);
			}else
			if ( *(unsigned char *)m_Buffer->getData() == 0x55 )	// file is known
			{
				g_GotekSubmitHandler->popGote();
				m_Buffer->cut(1);
			}else
			if ( *(unsigned char *)m_Buffer->getData() == 0xff )	// ping
			{
				logSpam("%s\n", "G.O.T.E.K. PING");
				char c = 0xff;
				m_Socket->doRespond(&c,1);
				m_Buffer->cut(1);
			}else													// error
			{
				logCrit("got crap %i\n",msg->getSize());	
				m_Buffer->cut(1);
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
ConsumeLevel gotekCTRLDialogue::outgoingData(Message *msg)
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
ConsumeLevel gotekCTRLDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionLost(Message *)
 * since the submit-gotek module now has to etablish a new connection
 * we simply notify the father class and then get dropped
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel gotekCTRLDialogue::connectionLost(Message *msg)
{
	m_HandlingFather->childConnectionLost();	
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * does exactly the same as connectionLost by simply calling it
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel gotekCTRLDialogue::connectionShutdown(Message *msg)
{
	return connectionLost(msg);
}

