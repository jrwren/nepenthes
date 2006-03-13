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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>


#include "gotekDATADialogue.hpp"
#include "submit-gotek.hpp"

#include "Message.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Download.hpp"

#include "DownloadUrl.hpp"


#include "DownloadBuffer.hpp"


#include "Utilities.hpp"

#include "Buffer.hpp"


using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the gotekDATADialogue, creates a new gotekDATADialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
gotekDATADialogue::gotekDATADialogue(GotekContext *ctx)
{
    	m_DialogueName = "gotekDATADialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_State = GDATA_NULL;

	m_Buffer = new Buffer(128);

	m_GotekContext = ctx;
	
	m_FileBuffer = NULL;
}

bool gotekDATADialogue::loadFile()
{
	logPF();
	
	if(!m_GotekContext->m_DataBuffer)
	{		
		FILE * filePointer = fopen(m_GotekContext->m_FileName.c_str(), "rb");
		
		m_FileBuffer = (unsigned char *) malloc(m_GotekContext->m_Length);
		assert(m_FileBuffer != NULL);
		
		if(!filePointer || fread(m_FileBuffer, 1, m_GotekContext->m_Length, filePointer) != m_GotekContext->m_Length)
		{
			logCrit("Failed to read data from cached spool file \"%s\"!", m_GotekContext->m_FileName.c_str());
			
			if(filePointer)
			{
				fclose(filePointer);
			}
			
			return false;
		}
		
		fclose(filePointer);
		
		return true;
	} else
	{
		m_FileBuffer = m_GotekContext->m_DataBuffer;
		return true;
	}
}

gotekDATADialogue::~gotekDATADialogue()
{
	if(m_FileBuffer)
	{
		free(m_FileBuffer);
	}
	
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
ConsumeLevel gotekDATADialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getSize());

	switch (m_State)
	{
	case GDATA_NULL:	// just connected
		if (m_Buffer->getSize() == 12)
		{
			m_Buffer->cut(4); // protocol version foobar

			uint64_t sessionkey=0;
			memcpy((char *)&sessionkey,(char *)m_Buffer->getData(),8);

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
			memcpy(hashme+1024,&sessionkey,8);
			g_Nepenthes->getUtilities()->sha512(hashme, 1032, hash);

			m_Socket->doRespond((char *)hash,64);

			m_Buffer->clear();

			m_State = GDATA_AUTH;
		}else
		if (m_Buffer->getSize() > 12)
		{
			// notify parent of a protocol problem, close socket
			return CL_DROP;
		}
		break;

	case GDATA_AUTH:
		if (m_Buffer->getSize() == 1)
		{
			if (* (unsigned char *) m_Buffer->getData() == 0xaa)
			{
				assert(m_FileBuffer != NULL);
			
				uint32_t len = htonl(m_GotekContext->m_Length);
				
				logDebug("Data connection to %s etablished.\n", "UNIMPLEMENTED");
				m_Socket->doRespond("\xaa", 1);
				
				m_Socket->doRespond((char *) &m_GotekContext->m_EvCID, 8);
				m_Socket->doRespond((char *) &len, 4);
				m_Socket->doRespond((char *) m_FileBuffer, m_GotekContext->m_Length);

				m_State = GDATA_DONE;
				m_Socket->setStatus(SS_CLEANQUIT);
			}
		}
		break;

	case GDATA_DONE:
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
ConsumeLevel gotekDATADialogue::outgoingData(Message *msg)
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
ConsumeLevel gotekDATADialogue::handleTimeout(Message *msg)
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
ConsumeLevel gotekDATADialogue::connectionLost(Message *msg)
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
ConsumeLevel gotekDATADialogue::connectionShutdown(Message *msg)
{
	if(m_State == GDATA_DONE)
	{
		if(unlink(m_GotekContext->m_FileName.c_str()) < 0)
		{
			logCrit("Deleting submitted file \"%s\" from spool failed: %s!\n", m_GotekContext->m_FileName.c_str(), strerror(errno));
		}
	}
	
	return CL_DROP;
}

