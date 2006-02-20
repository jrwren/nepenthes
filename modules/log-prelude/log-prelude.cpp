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

#include <prelude.h>
#include <arpa/inet.h>

#include "log-prelude.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SubmitEvent.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"

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
 * creates a new LogPrelude Module, where x% is public Module, public EventHandler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the EventHandlerName
 * - sets the EventHandlerDescription
 * - sets the EventHandlers Timeout
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
LogPrelude::LogPrelude(Nepenthes *nepenthes)
{
	m_ModuleName        = "log-prelude";
	m_ModuleDescription = "event based prelude logger";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = "LogPreludeEventHandler";
	m_EventHandlerDescription = "log events to a prelude database";

	m_Timeout = time(NULL) + rand()%23;

	g_Nepenthes = nepenthes;

	m_PreludeClient = NULL;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
LogPrelude::~LogPrelude()
{

}

#define ANALYZER_CLASS "NIDS"
#define ANALYZER_MODEL "Nepenthes"
#define ANALYZER_MANUFACTURER "http://nepenthes.sf.net"
#define DEFAULT_ANALYZER_NAME "nepenthes"
#define VERSION "$Rev$"


static int setup_analyzer(idmef_analyzer_t *analyzer)
{
        int ret;
        prelude_string_t *string;

        ret = idmef_analyzer_new_model(analyzer, &string);
        if ( ret < 0 )
                return ret;
        prelude_string_set_constant(string, ANALYZER_MODEL);

        ret = idmef_analyzer_new_class(analyzer, &string);
        if ( ret < 0 )
                return ret;
        prelude_string_set_constant(string, ANALYZER_CLASS);

        ret = idmef_analyzer_new_manufacturer(analyzer, &string);
        if ( ret < 0 )
                return ret;
        prelude_string_set_constant(string, ANALYZER_MANUFACTURER);

        ret = idmef_analyzer_new_version(analyzer, &string);
        if ( ret < 0 )
                return ret;
        prelude_string_set_constant(string, VERSION);

        return 0;
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
bool LogPrelude::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
    m_Events.set(EV_SUBMISSION);
	m_Events.set(EV_TIMEOUT);


	int ret;
    const char *profile, *config;

	config = NULL;
	profile = DEFAULT_ANALYZER_NAME;

//	parse_args(args, &profile, &config);

	ret = prelude_init(NULL, NULL);
	if ( ret < 0 )
		logCrit("%s: Unable to initialize the Prelude library: %s.\n",
				prelude_strsource(ret), 
				prelude_strerror(ret));

	ret = prelude_client_new(&m_PreludeClient, profile);

	if ( ret < 0 )
		logCrit("%s: Unable to create a prelude client object: %s.\n",
				prelude_strsource(ret), 
				prelude_strerror(ret));

//	ret = prelude_client_set_flags(m_PreludeClient,prelude_client_get_flags(m_PreludeClient) | PRELUDE_CLIENT_FLAGS_ASYNC_SEND|PRELUDE_CLIENT_FLAGS_ASYNC_TIMER);
	if ( ret < 0 )
		logCrit("%s: Unable to set asynchronous send and timer: %s.\n",
				prelude_strsource(ret), 
				prelude_strerror(ret));

	setup_analyzer(prelude_client_get_analyzer(m_PreludeClient));

	ret = prelude_client_start(m_PreludeClient);
	if ( ret < 0 )
	{
		if ( prelude_client_is_setup_needed(ret) )
			prelude_client_print_setup_error(m_PreludeClient);

		logCrit("%s: Unable to initialize prelude client: %s.\n",
				   prelude_strsource(ret), prelude_strerror(ret));
	}


	REG_EVENT_HANDLER(this);
	return true;
}


/**
 * unregister as EventHandler
 * 
 * @return returns true if everything was fine
 */
bool LogPrelude::Exit()
{
//	if( m_PreludeClient != NULL)
//		prelude_client_destroy(m_PreludeClient, PRELUDE_CLIENT_STATUS_EXIT_SUCCESS);

	return true;
}

int add_idmef_object(idmef_message_t *message, const char *object, const char *value)
{
        int ret=0;
        idmef_value_t *val;
        idmef_path_t *path;
        
        ret = idmef_path_new(&path, object);
        if ( ret < 0 )
		{
			logSpam("imdef ranz #1 %i (%s) \n",ret, prelude_strerror(ret));
            return -1;
		}

        ret = idmef_value_new_from_path(&val, path, value);
        if ( ret < 0 ) {
                idmef_path_destroy(path);
				logSpam("imdef ranz #2 %i (%s)\n",ret,prelude_strerror(ret));
                return -1;
        }

        ret = idmef_path_set(path, message, val);

        idmef_value_destroy(val);
        idmef_path_destroy(path);
        logSpam("imdef ranz #3 %i %s\n",ret,prelude_strerror(ret));
        return ret;
}


/**
 * the handleEvent method is called whenever an event occurs 
 * the EventHandler wanted to have.
 * 
 * @param event  the Event
 * 
 * @return return 0
 */
unsigned int LogPrelude::handleEvent(Event *event)
{
	logPF();
//	logInfo("Event %i\n",event->getType());
	switch(event->getType())
	{
	case EV_SUBMISSION:
		{
			SubmitEvent *se = (SubmitEvent *)event;
			Download *down = se->getDownload();

			se->getType();
			logInfo("LogPrelude EVENT EV_SUBMISSION %s %s %i \n",down->getUrl().c_str(), 
					down->getMD5Sum().c_str(), 
					down->getDownloadBuffer()->getLength());

			idmef_message_t *idmef;

			int ret = idmef_message_new(&idmef);
			if ( ret < 0 )
				return 1;

			// generic information
			add_idmef_object(idmef, "alert.classification.text", "Nepenthes Submit File");
			string url = "http://nepenthes.sf.net/wiki/submission/" + down->getMD5Sum();
			add_idmef_object(idmef, "alert.classification.reference(0).origin", "vendor-specific" );
			add_idmef_object(idmef, "alert.classification.reference(0).url", url.c_str() );


			// file name and info
			add_idmef_object(idmef, "alert.target(0).file(0).name", down->getDownloadUrl()->getFile().c_str());
			char csize[9];
			memset(csize,0,9);
			snprintf(csize,9,"%i",down->getDownloadBuffer()->getLength());
			add_idmef_object(idmef, "alert.target(0).file(0).data_size", csize);
			add_idmef_object(idmef, "alert.target(0).file(0).Checksum(0).algorithm","MD5");
			add_idmef_object(idmef, "alert.target(0).file(0).Checksum(0).value",down->getMD5Sum().c_str());
			add_idmef_object(idmef, "alert.target(0).file(0).Checksum(0).category","current");

			// infection host
			unsigned long addr = down->getAddress();
			string address = inet_ntoa(*(in_addr *)&addr);
			add_idmef_object(idmef, "alert.target(1).Node.Address(0).address",address.c_str());


			// download source
			char port[5];
			memset(port,0,5);
            memset(port,0,5);
			snprintf(port,9,"%i",down->getDownloadUrl()->getPort());
			add_idmef_object(idmef, "alert.source(0).Service.port"		,port);
			string protocol;
			if (down->getDownloadUrl()->getProtocol() == "tftp" )
            	protocol = "UDP";
			else
				protocol = "TCP";
			add_idmef_object(idmef, "alert.source(0).Service.protocol"					,protocol.c_str());
			add_idmef_object(idmef, "alert.source(0).Service.web_service.url"			,down->getUrl().c_str());
			add_idmef_object(idmef, "alert.source(0).Service.web_service.http_method"	,"get");


			// time
			idmef_time_t *time;
			ret = idmef_time_new_from_gettimeofday(&time);
            idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
										time);


			// analyzer id
			idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
									 idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
									 0);


			prelude_client_send_idmef(m_PreludeClient, idmef);
			idmef_message_destroy(idmef);
		}
		break;

	case EV_TIMEOUT:
		m_Timeout = time(NULL) + 1;
//		prelude_timer_wakeup();
		break;

	default:
		logWarn("%s","this should not happen\n");
	}
	return 0;
}

extern "C" int module_init(int version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new LogPrelude(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
