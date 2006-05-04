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


#include "submit-norman.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"
#include "EventManager.hpp"

#include "Config.hpp"

using namespace nepenthes;


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;


/**
 * Constructor
 * creates a new SubmitNorman Module, where SubmitNorman is public Module, public SubmitHanvler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the SubmitHandlerName
 * - sets the SubmitHandlerDescription
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
SubmitNorman::SubmitNorman(Nepenthes *nepenthes)
{
	m_ModuleName        = "submit-norman";
	m_ModuleDescription = "submit files to sandbox.norman.no";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_SubmitterName = "submit-norman";
	m_SubmitterDescription = "submit files to sandbox.norman.no";

	m_EventHandlerName = "submit-norman";
	m_EventHandlerDescription = "timeout handler for submit-norman";

	g_Nepenthes = nepenthes;

	m_Timeout = time(NULL);
	m_Queued = 0;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
SubmitNorman::~SubmitNorman()
{

}

/**
 * Module::Init()
 * register the submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a module loading error
 */
bool SubmitNorman::Init()
{
	logPF();

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	try
	{
		m_Email = m_Config->getValString("submit-norman.email");
    } catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();

    if((m_CurlStack = curl_multi_init()) == NULL )
	{
		 logCrit("Could not init Curl Multi Perform Stack %s\n",strerror(errno));
		 return false;
	}

	REG_SUBMIT_HANDLER(this);
	REG_EVENT_HANDLER(this);
	return true;
}


/**
 * Module::Exit()
 * 
 * unregister the Submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a heavy error
 */
bool SubmitNorman::Exit()
{
	curl_multi_cleanup(m_CurlStack);
	return true;
}


/**
 * SubmitHandler::Submit(Download *down)
 * 
 * get and submit a file.
 * here we just hexdump it to shell
 * 
 * @param down   the download to hexdump
 */
void SubmitNorman::Submit(Download *down)
{
	logPF();

	if(m_Events.test(EV_TIMEOUT) == false)
    	m_Events.set(EV_TIMEOUT);
	CURL *curl;

	NormanContext *norm = new NormanContext((char *)m_Email.c_str(),down->getDownloadUrl()->getFile(),down->getDownloadBuffer()->getSize(),
										   down->getDownloadBuffer()->getData(), (char *)down->getMD5Sum().c_str());
	curl = curl_easy_init();
	if ( curl )
	{
		/* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, norm->m_HeaderList);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, norm->m_FormPost);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_URL           , "http://sandbox.norman.no/live_4.html");//"http://localhost:8888/examplepost.cgi");//
		curl_easy_setopt(curl, CURLOPT_USERAGENT     , "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)");
		curl_easy_setopt(curl, CURLOPT_PRIVATE		 , (char *) norm);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA		, norm);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION	, SubmitNorman::WriteCallback);

        curl_multi_add_handle(m_CurlStack, curl);
		m_Queued++;
	}

}


size_t SubmitNorman::WriteCallback(char *buffer, size_t size, size_t nitems, void *userp)
{
	int32_t iSize = size * nitems;
	return iSize;
}

/**
 * SubmitHandler::Hitt(Download *down)
 * 
 * get and submit a file.
 * 
 * 
 * @param down   the download to hexdump
 */
void SubmitNorman::Hit(Download *down)
{
	return;
}

uint32_t SubmitNorman::handleEvent(Event *event)
{
	logPF();
	if(event->getType() != EV_TIMEOUT)
	{
		logCrit("Unwanted event %i\n",event->getType());
		return 1;
	}

	int32_t iQueue=0;
	while ( curl_multi_perform(m_CurlStack, (int *)&iQueue) == CURLM_CALL_MULTI_PERFORM );

	if ( m_Queued > iQueue )
	{
		logSpam("m_Queued  (%i) > (%i) iQueue\n", m_Queued, iQueue);
		CURLMsg * pMessage;

		while ( (pMessage = curl_multi_info_read(m_CurlStack, (int *)&iQueue)) )
		{
			if ( pMessage->msg == CURLMSG_DONE )
			{
				NormanContext *norm;
				char *cnorm;
				char * szUrl;

                curl_easy_getinfo(pMessage->easy_handle, CURLINFO_PRIVATE, (char * *) &cnorm);
                norm = (NormanContext *)cnorm;
                
				if ( pMessage->data.result )
				{
                    logInfo("Upload Error %s on getting file %s \n", curl_easy_strerror(pMessage->data.result), norm->getMD5Sum());
				} else
				{
					curl_easy_getinfo(pMessage->easy_handle, CURLINFO_EFFECTIVE_URL, &szUrl);
					logInfo("Submitted file %s to sandbox \n",norm->getMD5Sum());
				}
				CURL *curl = pMessage->easy_handle;
				curl_multi_remove_handle(m_CurlStack, pMessage->easy_handle);
				delete norm;
				curl_easy_cleanup(curl);
				--m_Queued;
			}
		}
	}

	if(m_Queued == 0)
    	m_Events.reset(EV_TIMEOUT);
	
	m_Timeout = time(NULL) + 1;
	return 0;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if(version == MODULE_IFACE_VERSION)
	{
		*module = new SubmitNorman(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}
