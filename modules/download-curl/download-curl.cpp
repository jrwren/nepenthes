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

#include "download-curl.hpp"
#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SocketEvent.hpp"
#include "Socket.hpp"
#include "SubmitManager.hpp"
#include "DownloadManager.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadCallback.hpp"

#include "Config.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_hlr



using namespace nepenthes;
Nepenthes *g_Nepenthes;
CurlDownloadHandler::CurlDownloadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "Curl Download Module";
	m_ModuleDescription = "provides widly used protocols (http/ftp)";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = "CurlDownloadHandlerEventHandler";
	m_EventHandlerDescription = "printf some events to console if they get fired";

	m_DownloadHandlerDescription = "curl based ftp & http downloadhandler";
	m_DownloadHandlerName  = "curl download handler";

	m_Timeout = time(NULL);
	m_Queued = 0;
	g_Nepenthes = nepenthes;
}

CurlDownloadHandler::~CurlDownloadHandler()
{
	curl_multi_cleanup(m_CurlStack);
}

bool CurlDownloadHandler::Init()
{
	logPF();

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	bool ftpsupport=false;
	try
	{
		m_MaxFileSize = m_Config->getValInt("download-curl.max-filesize");
		ftpsupport = m_Config->getValInt("download-curl.enable-ftp");
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

	REG_EVENT_HANDLER(this);

	if (ftpsupport == true)
    {
		REG_DOWNLOAD_HANDLER(this,"ftp");
	}

	REG_DOWNLOAD_HANDLER(this,"http");


	return true;
}

bool CurlDownloadHandler::Exit()
{
	

	return true;
}

uint32_t CurlDownloadHandler::handleEvent(Event *event)
{
//	logPF();
	logSpam("<in %s> (%i downloads in queue)\n",__PRETTY_FUNCTION__,m_Queued);
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
				Download *pDown;
				char *cDown;
				char * szUrl;

                curl_easy_getinfo(pMessage->easy_handle, CURLINFO_PRIVATE, (char * *) &cDown);
                pDown = (Download *)cDown;
				if ( pMessage->data.result )
				{
                    logWarn("Download error %s on getting file %s \n", curl_easy_strerror(pMessage->data.result), pDown->getUrl().c_str());
					if (pDown->getCallback() != NULL)
					{
						pDown->getCallback()->downloadFailure(pDown);
					}
				} else
				{
					curl_easy_getinfo(pMessage->easy_handle, CURLINFO_EFFECTIVE_URL, &szUrl);
					logInfo("Downloading of %s (%s) %i Bytes successful.\n", pDown->getUrl().c_str(), 
							szUrl, pDown->getDownloadBuffer()->getSize());

					if (pDown->getCallback() != NULL)
					{
						pDown->getCallback()->downloadSuccess(pDown);
					}else
					{
                    	m_Nepenthes->getSubmitMgr()->addSubmission(pDown);
					}
				}
				CURL *curl = pMessage->easy_handle;

				curl_multi_remove_handle(m_CurlStack, pMessage->easy_handle);
				delete pDown;
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

bool CurlDownloadHandler::download(Download *down)
{
	if(m_Events.test(EV_TIMEOUT) == false)
    	m_Events.set(EV_TIMEOUT);

	CURL * pCurlHandle = curl_easy_init();
	curl_easy_setopt(pCurlHandle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(pCurlHandle, CURLOPT_FORBID_REUSE	, 1);
	curl_easy_setopt(pCurlHandle, CURLOPT_MAXFILESIZE	, m_MaxFileSize); //FIXME config
	curl_easy_setopt(pCurlHandle, CURLOPT_MAXREDIRS		, 3);
	curl_easy_setopt(pCurlHandle, CURLOPT_NOPROGRESS	, 1);
	curl_easy_setopt(pCurlHandle, CURLOPT_NOSIGNAL		, 1);
	curl_easy_setopt(pCurlHandle, CURLOPT_PRIVATE		, (char *) down);
	curl_easy_setopt(pCurlHandle, CURLOPT_SSL_VERIFYHOST, false);
	curl_easy_setopt(pCurlHandle, CURLOPT_SSL_VERIFYPEER, false);
	
	curl_easy_setopt(pCurlHandle, CURLOPT_USERAGENT		, "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	curl_easy_setopt(pCurlHandle, CURLOPT_WRITEDATA		, down);
    curl_easy_setopt(pCurlHandle, CURLOPT_WRITEFUNCTION	, CurlDownloadHandler::WriteCallback);


	curl_easy_setopt(pCurlHandle, CURLOPT_NOSIGNAL 		, 1);	// disable curl signaling curl
	curl_easy_setopt(pCurlHandle, CURLOPT_TIMEOUT 		, 600 ); // 10 min
	curl_easy_setopt(pCurlHandle, CURLOPT_LOW_SPEED_LIMIT , 1);	// 2 min under 1 byte /s and we break it
	curl_easy_setopt(pCurlHandle, CURLOPT_LOW_SPEED_TIME, 120);

    if (down->getDownloadUrl()->getProtocol() == "http")
	{
    	curl_easy_setopt(pCurlHandle, CURLOPT_URL			, down->getUrl().c_str());
		logInfo("HTTP DOWNLOAD %s \n",down->getUrl().c_str());
	}else
	if (down->getDownloadUrl()->getProtocol() == "ftp")
	{
		char *url;
		if (asprintf(&url,"ftp://%s:%i/%s",
				 down->getDownloadUrl()->getHost().c_str(),
				 down->getDownloadUrl()->getPort(),
				 down->getDownloadUrl()->getPath().c_str()) == -1) {
			logCrit("Memory allocation error\n");
			return false;
		}
//        string auth = down->getDownloadUrl()->getUser() + ":" + down->getDownloadUrl()->getPass();
#if LIBCURL_VERSION_NUM < 0x071000
		curl_easy_setopt(pCurlHandle, CURLOPT_SOURCE_USERPWD,(char *)down->getDownloadUrl()->getAuth().c_str());
#endif
		curl_easy_setopt(pCurlHandle, CURLOPT_USERPWD,(char *)down->getDownloadUrl()->getAuth().c_str());
		curl_easy_setopt(pCurlHandle, CURLOPT_URL			, url);
		curl_easy_setopt(pCurlHandle, CURLOPT_FTP_RESPONSE_TIMEOUT, 120);	// 2 min ftp timeout
//		curl_easy_setopt(pCurlHandle, 
		logInfo("FTP DOWNLOAD %s %s \n",url,down->getDownloadUrl()->getAuth().c_str());
//		free(url);
	}

	curl_multi_add_handle(m_CurlStack, pCurlHandle);
	m_Queued++;
	return true;
}

size_t CurlDownloadHandler::WriteCallback(char *buffer, size_t size, size_t nitems, void *userp)
{
	int32_t iSize = size * nitems;
	Download *down = (Download *)userp;
	down->getDownloadBuffer()->addData(buffer,iSize);
	return iSize;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new CurlDownloadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
