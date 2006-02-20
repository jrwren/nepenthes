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

#include "HTTPDialogue.hpp"


#include "Message.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "DownloadCallback.hpp"

#include "SubmitManager.hpp"

#include "Utilities.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_dia | l_hlr

using namespace nepenthes;


/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the HTTPDialogue, creates a new HTTPDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
HTTPDialogue::HTTPDialogue(Socket *socket, Download *down)
{
	m_Socket = socket;
    m_DialogueName = "HTTPDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_Download = down;
}

HTTPDialogue::~HTTPDialogue()
{
	delete m_Download;
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
ConsumeLevel HTTPDialogue::incomingData(Message *msg)
{
//	logSpam("HTTP GET\n%.*s\n",msg->getSize(),msg->getMsg());
//	g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());
	m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getSize());
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
ConsumeLevel HTTPDialogue::outgoingData(Message *msg)
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
ConsumeLevel HTTPDialogue::handleTimeout(Message *msg)
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
ConsumeLevel HTTPDialogue::connectionLost(Message *msg)
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
ConsumeLevel HTTPDialogue::connectionShutdown(Message *msg)
{
	logPF();
	char *start = m_Download->getDownloadBuffer()->getData();
	char *end =  NULL;

	uint32_t size = m_Download->getDownloadBuffer()->getSize();
	uint32_t i=0;

	while ( i < size )
	{
		if ( 				start[i]   == '\r' && 
			 i+1 < size &&  start[i+1] == '\n' &&
			 i+2 < size &&  start[i+2] == '\r' &&
			 i+3 < size &&	start[i+3] == '\n' )
		{
			end = start + i;
			break;
		}
		i++;

	}

	if ( end == NULL )
	{
		logWarn("HTTP ERROR header found %i\n", size);
		g_Nepenthes->getUtilities()->hexdump((byte *)start,size);
		return CL_DROP;
	}else
	if ( end != NULL )
	{
		end += 2;
		logSpam("FOUND HEADER (size %i)\n",end-start);
		logSpam("%.*s",end-start,start);
// FIXME PARSE HEADER
//		HTTPHeader *header = new HTTPHeader(start,(uint32_t)(end-start));
//		m_HTTPHeader = header;
	}
	m_Download->getDownloadBuffer()->cutFront((uint32_t)(end-start)+2);

	if (m_Download->getCallback() != NULL)
	{
		m_Download->getCallback()->downloadSuccess(m_Download);
	}else
	{
		g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
	}
	return CL_DROP;
}


ConsumeLevel HTTPDialogue::connectionEstablished()
{
	logPF();

	char *request;
	asprintf(&request,
			 "GET /%s HTTP/1.0\r\n"
			 "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
			 "Accept: */*\r\n"
			 "Host: %s:%i\r\n"
			 "Connection: close\r\n"
			 "\r\n",
			 m_Download->getDownloadUrl()->getPath().c_str(),
			 m_Download->getDownloadUrl()->getHost().c_str(), m_Download->getDownloadUrl()->getPort());

	m_Socket->doRespond(request,strlen(request));
	logSpam("HTTP REQ\n%s\n",request);
	free(request);


	return CL_ASSIGN;
}


