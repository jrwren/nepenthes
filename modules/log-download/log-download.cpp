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

#include "log-download.hpp"
#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SocketEvent.hpp"
#include "Socket.hpp"
#include "SubmitEvent.hpp"
#include "Config.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_ev | l_hlr


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * Constructor
 * creates a new LogDownload Module, where x% is public Module, public EventHandler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the EventHandlerName
 * - sets the EventHandlerDescription
 * - sets the EventHandlers Timeout
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
LogDownload::LogDownload(Nepenthes *nepenthes)
{
	m_ModuleName        = "log-download";
	m_ModuleDescription = "logs all downloads to a file";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = "LogDownloadEventHandler";
	m_EventHandlerDescription = "log download attempts and successfull downloads";

	m_Timeout = 0;
	g_Nepenthes = nepenthes;

	m_DownloadFile = NULL;
	m_SubmitFile = NULL;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
LogDownload::~LogDownload()
{

}



/**
 * bool Module::Init()
 * setup Module specific values 
 * here:
 * - register als EventHandler
 * - set wanted events
 * 
 * @return returns true if everything was fine, else false
 *         returning false will showup errors in warning a module
 */
bool LogDownload::Init()
{
	
	if ( m_Config == NULL )
	 {
		 logCrit("%s","I need a config\n");
		 return false;
	 }

	 try
	 {
		 string path = m_Config->getValString("log-download.downloadfile");
		 if ((m_DownloadFile = fopen(path.c_str(),"a")) == NULL)
		 {
			 logCrit("Could not open logfile %s \n",path.c_str());
			 return false;
		 }

		 path = m_Config->getValString("log-download.submitfile");
		 if ((m_SubmitFile = fopen(path.c_str(),"a")) == NULL)
		 {
			 logCrit("Could not open logfile %s \n",path.c_str());
			 return false;
		 }
			 

	 } catch ( ... )
	 {
		 logCrit("%s","Error setting needed vars, check your config\n");
		 return false;
	 }

	m_ModuleManager = m_Nepenthes->getModuleMgr();
    m_Events.set(EV_DOWNLOAD);
	m_Events.set(EV_SUBMISSION);
    REG_EVENT_HANDLER(this);
	return true;
}


/**
 * unregister as EventHandler
 * 
 * @return returns true if everything was fine
 */
bool LogDownload::Exit()
{
	if (m_DownloadFile != NULL)
	{
		fclose(m_DownloadFile);
	}
	if (m_SubmitFile != NULL)
	{
		fclose(m_SubmitFile);
	}

	return true;
}


/**
 * the handleEvent method is called whenever an event occurs 
 * the EventHandler wanted to have.
 * 
 * @param event  the Event
 * 
 * @return return 0
 */
uint32_t LogDownload::handleEvent(Event *event)
{
	logPF();
//	logInfo("Event %i\n",event->getType());
	struct tm       t;
	time_t          stamp;
	time(&stamp);

	localtime_r(&stamp, &t);


	switch(event->getType())
	{
	case EV_DOWNLOAD:
		{
			SubmitEvent *se = (SubmitEvent *)event;
			Download *down = se->getDownload();
			// we use ISO 8601 %Y-%m-%dT%H:%M:%S
			fprintf(m_DownloadFile, "[%04d-%02d-%02dT%02d:%02d:%02d] %s\n", 
					t.tm_year + 1900,
					t.tm_mon + 1, 
					t.tm_mday, 
					t.tm_hour, 
					t.tm_min, 
					t.tm_sec,
					down->getUrl().c_str()
					);
			fflush(m_DownloadFile);
		}
		break;

	case EV_SUBMISSION:
		{
			SubmitEvent *se = (SubmitEvent *)event;
			Download *down = se->getDownload();
			fprintf(m_SubmitFile, "[%04d-%02d-%02dT%02d:%02d:%02d] %s %s\n", 
					t.tm_year + 1900,
					t.tm_mon + 1, 
					t.tm_mday, 
					t.tm_hour, 
					t.tm_min, 
					t.tm_sec,
					down->getUrl().c_str(),
					down->getMD5Sum().c_str()
					);
			fflush(m_SubmitFile);
		}
		break;

	default:
		logWarn("%s","this should not happen\n");
	}
	return 0;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new LogDownload(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
