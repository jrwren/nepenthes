/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2006 Niklas Schiffler <nick@digitician.eu> 
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

#include "submit-http.hpp"
#include "Download.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"
#include "Event.hpp"
#include "EventManager.hpp"
#include "EventHandler.cpp" // das ist Mist!
#include "Config.hpp"
#include "ModuleManager.hpp"

#include "HTTPSession.hpp"

using namespace nepenthes;


Nepenthes *g_Nepenthes;


HTTPSubmitHandler::HTTPSubmitHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "submit-http";
	m_ModuleDescription = "HTTP submit handler";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;
	m_SubmitterName = "submit-http";
	m_SubmitterDescription = "submit binary file via HTTP POST request";
	g_Nepenthes = nepenthes;
}


HTTPSubmitHandler::~HTTPSubmitHandler()
{
}

bool HTTPSubmitHandler::Init()
{
	logPF();

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	try
	{
		m_URL = m_Config->getValString("submit-http.url");
	}
	catch ( ... )
	{
		logCrit("Error: Config property \"url\" missing\n");
		return false;
	}

	try
	{
		m_Email = m_Config->getValString("submit-http.email");
		m_User = m_Config->getValString("submit-http.user");
		m_Password = m_Config->getValString("submit-http.pass");
	}
	catch ( ... )
	{
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();

	if ( (m_CurlStack = curl_multi_init()) == NULL )
	{
		logCrit("Could not init Curl Multi Perform Stack %s\n",strerror(errno));
		return false;
	}

	REG_SUBMIT_HANDLER(this);
	REG_EVENT_HANDLER(this);
	return true;
}

bool HTTPSubmitHandler::Exit()
{
	curl_multi_cleanup(m_CurlStack);
	return true;
}


void HTTPSubmitHandler::Submit(Download *down)
{
	logPF();

	if ( m_Events.test(EV_TIMEOUT) == false )
		m_Events.set(EV_TIMEOUT);

	HTTPSession* session = new HTTPSession(m_URL, m_Email, m_User, m_Password, down);
	curl_multi_add_handle(m_CurlStack, session->getSubmitInfoHandle());
	m_Queued++;
}

void HTTPSubmitHandler::Hit(Download *down)
{
	Submit(down);
}


uint32_t HTTPSubmitHandler::handleEvent(Event *event)
{
	logPF();
	if ( event->getType() != EV_TIMEOUT )
	{
		logCrit("Unwanted event %i\n",event->getType());
		return 1;
	}

	// do file info submits
	int32_t iQueue = 0;
	while ( curl_multi_perform(m_CurlStack, (int *)&iQueue) == CURLM_CALL_MULTI_PERFORM );

	if ( m_Queued > iQueue )
	{
		logSpam("m_Queued  (%i) > (%i) iQueue\n", m_Queued, iQueue);
		CURLMsg * pMessage;

		while ( (pMessage = curl_multi_info_read(m_CurlStack, (int *)&iQueue)) )
		{
			if ( pMessage->msg == CURLMSG_DONE )
			{
				HTTPSession *session;
				char *cSession;

				curl_easy_getinfo(pMessage->easy_handle, CURLINFO_PRIVATE, (char**)&cSession);
				session = (HTTPSession *)cSession;

				uint8_t sessionState = session->getState();

				if ( sessionState == HTTPSession::S_FILEKNOWN || sessionState == HTTPSession::S_FILEREQUEST )
				{
					if ( pMessage->data.result )
					{
						logInfo("Error: Submitting file info (%s, %s) failed: %s\n", session->getMD5().c_str(), session->getFileSourceURL().c_str(), curl_easy_strerror(pMessage->data.result));
						delete session;
						curl_multi_remove_handle(m_CurlStack, pMessage->easy_handle);
						--m_Queued;
						continue;
					}
					logInfo("File info submitted (%s, %s)\n", session->getMD5().c_str(), session->getFileSourceURL().c_str());
				}

				switch ( sessionState )
				{
				case HTTPSession::S_FILEKNOWN:
					logInfo("File already known (%s, %s)\n", session->getMD5().c_str(), session->getFileSourceURL().c_str());
					break;
				case HTTPSession::S_FILEREQUEST:
					logInfo("File upload requested (%s, %s)\n", session->getMD5().c_str(), session->getFileSourceURL().c_str());
					session->setState(HTTPSession::S_FILEPENDING);
					curl_multi_add_handle(m_CurlStack, session->getSubmitFileHandle());
					break;      
				case HTTPSession::S_FILEOK:
					logInfo("File uploaded (%s, %s)\n", session->getMD5().c_str(), session->getFileSourceURL().c_str());
					break;
				case HTTPSession::S_ERROR:
					logInfo("Error handling file (%s, %s)\n", session->getMD5().c_str(), session->getFileSourceURL().c_str());
					break;
				}

				curl_multi_remove_handle(m_CurlStack, pMessage->easy_handle);

				if ( sessionState == HTTPSession::S_FILEKNOWN ||
					 sessionState == HTTPSession::S_FILEOK ||
					 sessionState == HTTPSession::S_ERROR )
				{
					delete session;
					--m_Queued;
				}
			}
		}
	}

	if ( m_Queued == 0 )
		m_Events.reset(EV_TIMEOUT);

	m_Timeout = time(NULL) + 1;
	return 0;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new HTTPSubmitHandler(nepenthes);
		return 1;
	}
	else
	{
		return 0;
	}
}
