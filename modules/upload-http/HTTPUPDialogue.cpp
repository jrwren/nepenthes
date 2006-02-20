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

#include "HTTPUPDialogue.hpp"
#include "UploadQuery.hpp"
#include "UploadResult.hpp"
#include "DownloadUrl.hpp"
#include "Socket.hpp"
#include "Message.hpp"

#include "LogManager.hpp"
#include "Nepenthes.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "UploadResult.hpp"
#include "UploadResult.cpp"
#include "UploadCallback.hpp"

using namespace nepenthes;

HTTPUPDialogue::HTTPUPDialogue(Socket *socket, UploadQuery *query)
{
	m_Socket = socket;
	m_UploadQuery = query;

	m_ConsumeLevel = CL_ASSIGN;
	m_Buffer = new Buffer(1024);

	m_State = HTTP_UPLOAD_STATE_NULL;
}

HTTPUPDialogue::~HTTPUPDialogue()
{
	switch ( m_State )
	{
	case HTTP_UPLOAD_STATE_SUCCESS:
		{
			char *start = (char *)m_Buffer->getData();
			char *end =  NULL;

			uint32_t size = m_Buffer->getSize();
			uint32_t i=0;

			while ( i < size )
			{
				if ( start[i]   == '\r' && 
					 i+1 < size &&  start[i+1] == '\n' &&
					 i+2 < size &&  start[i+2] == '\r' &&
					 i+3 < size &&  start[i+3] == '\n' )
				{
					end = start + i;
					break;
				}
				i++;

			}

			if ( end == NULL )
			{
				logWarn("HTTP ERROR header found %i\n", size);
				m_State = HTTP_UPLOAD_STATE_ERROR;
				{
					UploadResult *result = new UploadResult("",0,m_UploadQuery->getObject());
					m_UploadQuery->getCallback()->uploadFailure(result);
					delete result;

				}

			} else
				if ( end != NULL )
			{
				end += 2;
				logSpam("FOUND HEADER (size %i)\n",end-start);
				logSpam("%.*s",end-start,start);

				m_Buffer->cut((uint32_t)(end-start)+2);
				if ( m_UploadQuery->getCallback() != NULL )
				{
					UploadResult *result = new UploadResult((char *)m_Buffer->getData(),m_Buffer->getSize(),m_UploadQuery->getObject());
					m_UploadQuery->getCallback()->uploadSuccess(result);
					delete result;
				}
// FIXME PARSE HEADER
//		HTTPHeader *header = new HTTPHeader(start,(uint32_t)(end-start));
//		m_HTTPHeader = header;
			}
		}
		break;


	case HTTP_UPLOAD_STATE_NULL:
	case HTTP_UPLOAD_STATE_ERROR:
		{
			UploadResult *result = new UploadResult("",0,m_UploadQuery->getObject());
			m_UploadQuery->getCallback()->uploadFailure(result);
			delete result;

		}
		break;
	}

	delete m_Buffer;
	delete m_UploadQuery;
}

ConsumeLevel HTTPUPDialogue::incomingData(Message *msg)
{
	
	logPF();
	m_Buffer->add(msg->getMsg(),msg->getSize());
//	logSpam("%.*s",msg->getSize(),msg->getMsg());
	return CL_ASSIGN;
}

ConsumeLevel HTTPUPDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

ConsumeLevel HTTPUPDialogue::handleTimeout(Message *msg)
{
	m_State = HTTP_UPLOAD_STATE_ERROR;
	return CL_DROP;
}

ConsumeLevel HTTPUPDialogue::connectionLost(Message *msg)
{
	m_State = HTTP_UPLOAD_STATE_ERROR;
	return CL_DROP;
}

ConsumeLevel HTTPUPDialogue::connectionShutdown(Message *msg)
{
	logPF();
	m_State = HTTP_UPLOAD_STATE_SUCCESS;
	return CL_DROP;
}

ConsumeLevel HTTPUPDialogue::connectionEstablished()
{
	char *request;
	asprintf(&request,
			 "POST /%s HTTP/1.0\r\n"
			 "Host: %s\r\n"
			 "Accept: */*\r\n"
			 "Accept-Encoding: deflate\r\n"
			 "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
			 "Connection: close\r\n"
			 "Content-Length: %i\r\n"
			 "\r\n",
			 m_UploadQuery->getUploadUrl()->getPath().c_str(),
			 m_UploadQuery->getUploadUrl()->getHost().c_str(),
			 m_UploadQuery->getSize());

	m_Socket->doRespond(request,strlen(request));
	m_Socket->doRespond(m_UploadQuery->getBuffer(),m_UploadQuery->getSize());
	logSpam("SENDING %s%.*s\n",request,m_UploadQuery->getSize(),m_UploadQuery->getBuffer());
	free(request);
	
	return CL_ASSIGN;
}
