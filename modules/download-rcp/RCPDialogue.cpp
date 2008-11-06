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
 
#include <sys/types.h>
#include <netinet/in.h>
#include <ctype.h>

#include "RCPDialogue.hpp"


#include "UDPSocket.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "SubmitManager.hpp"
#include "Nepenthes.hpp"
#include "Buffer.hpp"
#include "Utilities.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_dia | l_hlr


#include <cstdlib>
#include <cstring>

using namespace nepenthes;



RCPDialogue::RCPDialogue(Socket *socket, Download *down)
{
	m_DialogueName = "RCPDialogue";
	m_DialogueDescription = "download a file via rcp - remote file copy";

	m_Socket = socket;
	m_ConsumeLevel = CL_ASSIGN;

	m_ExpectedFileSize = 0;
	m_Download = down;
	m_Buffer = NULL;

	m_State = RCP_STATE_REQUEST;


}


RCPDialogue::~RCPDialogue()
{
	logPF();

	if (m_Download != NULL)
	{
    	delete m_Download;
	}

	if (m_Buffer != NULL)
	{
		delete m_Buffer;
	}
}

ConsumeLevel RCPDialogue::connectionEstablished()
{
	logPF();
	m_Buffer = new Buffer(1024);

	char zerobyte = 0;
	const char *request ="rcp -f ";
    m_Socket->doWrite((char *)&zerobyte,1);

	m_Buffer->add((void *)m_Download->getDownloadUrl()->getUser().c_str(),m_Download->getDownloadUrl()->getUser().size());
	m_Buffer->add(&zerobyte,1);
	m_Buffer->add((void *)m_Download->getDownloadUrl()->getUser().c_str(),m_Download->getDownloadUrl()->getUser().size());
	m_Buffer->add(&zerobyte,1);
	m_Buffer->add((void *)request,strlen(request));
	m_Buffer->add((void *)m_Download->getDownloadUrl()->getPath().c_str(),m_Download->getDownloadUrl()->getPath().size());
	m_Buffer->add(&zerobyte,1);

	m_Socket->doWrite((char *)m_Buffer->getData(),m_Buffer->getSize());

	m_Buffer->clear();
	return CL_ASSIGN;
}

ConsumeLevel RCPDialogue::incomingData(Message *msg)
{
	logPF();
	if (m_Download == NULL)
	{
		return CL_ASSIGN;
	}
//	g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());
	
	switch (m_State)
	{
	case RCP_STATE_REQUEST:
		logSpam("RCP STATE_REQUEST\n");
		m_Buffer->add(msg->getMsg(),msg->getSize());
		if (m_Buffer->getSize() == 1 && *(char *)m_Buffer->getData() == 0)
		{
			char zerobyte = 0;
			m_Socket->doWrite(&zerobyte,1);
			m_State = RCP_STATE_FILESTATS;
			m_Buffer->clear();
		}else
		{
			logInfo("RCP error %.*s\n",msg->getSize()-1,msg->getMsg()+1);
			return CL_DROP;
		}
		break;

	case RCP_STATE_FILESTATS:
		m_Buffer->add(msg->getMsg(),msg->getSize());
		logSpam("RCP STATE_FILESTATS\n");
		{
			// "C0644 98 7819 foo.exe\n"
			char *buf = (char *)m_Buffer->getData();
			int size = m_Buffer->getSize();

			// skip file permissions
			if (*buf == 'C')
            {
				while (*buf != ' ' && size > 0)
				{
					size--;
					buf++;
				}
   			}

			// skip whitespaces
			while (*buf == ' ' && size > 0)
			{
				size--;
				buf++;
			}

            char *sizebuf = buf;
			int sizesize = size;

			// get length of filesize
			while (isdigit(*sizebuf) && sizesize > 0)
			{
				sizebuf++;
				sizesize--;
			}

            char *filesize = (char *)malloc(size-sizesize+2);
			memset(filesize,0,size-sizesize+2);
			memcpy(filesize,buf,size-sizesize);
			logInfo("filesize is '%s'\n",filesize);

			m_ExpectedFileSize = atoi(filesize);

			free(filesize);

			char zerobyte = 0;
			m_Socket->doWrite(&zerobyte,1);
			m_State = RCP_STATE_FILE;
			m_Buffer->clear();
		}
		break;

	case RCP_STATE_FILE:
		logSpam("rcp %i bytes\n",msg->getSize());

		// the last char is 00 and additional
		if (m_Download->getDownloadBuffer()->getSize() + msg->getSize() >= m_ExpectedFileSize)
		{
			m_Download->getDownloadBuffer()->addData(msg->getMsg(),m_ExpectedFileSize - m_Download->getDownloadBuffer()->getSize());
			g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
			delete m_Download;
			m_Download = NULL;

		}else
		{
			m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getSize());
			if ( m_Download->getDownloadBuffer()->getSize() > 1024 * 1024 * 4)	// hardcoded 4mb limit for now (tm)
			{
				return CL_DROP;
			}
		}
		break;

	}

	return CL_ASSIGN;
}

ConsumeLevel RCPDialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

ConsumeLevel RCPDialogue::handleTimeout(Message *msg)
{
	logPF();
	logInfo("RCP Filetransferr failed, expected %i bytes, got %i bytes\n",m_ExpectedFileSize,m_Download->getDownloadBuffer()->getSize());
	return CL_DROP;
}

ConsumeLevel RCPDialogue::connectionLost(Message *msg)
{
    return CL_DROP;
}

ConsumeLevel RCPDialogue::connectionShutdown(Message *msg)
{
	logPF();
	if ( m_Download != NULL )
	{
		if ( m_ExpectedFileSize > 0 )
		{

			if ( m_Download->getDownloadBuffer()->getSize() != m_ExpectedFileSize )
			{
				logInfo("RCP Filetransferr failed, expected %i bytes, got %i bytes\n",m_ExpectedFileSize,m_Download->getDownloadBuffer()->getSize());
				return CL_DROP;
			}
		}
		g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
	}
    return CL_DROP;
}


