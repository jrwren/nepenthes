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

#ifdef HAVE_LIBPRELUDE
#include <prelude.h>
#include <libprelude/prelude-log.h>
#include <idmef-message-print.h>
#include <prelude-io.h>
#include <libprelude/prelude-timer.h>
#endif

#include <arpa/inet.h>
#include "log-prelude.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SubmitEvent.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"

#include "Socket.hpp"
#include "SocketEvent.hpp"

#include "Message.hpp"
#include "Utilities.hpp"
#include "Config.hpp"
#include "ShellcodeHandler.hpp"


using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif

#define STDTAGS l_mod | l_ev | l_hlr
#define ANALYZER_MANUFACTURER "http://nepenthes.sf.net"
#define NEPENTHES_VERSION "$Rev$"



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

//	m_Timeout = time(NULL) + rand()%23;

	g_Nepenthes = nepenthes;

#ifdef HAVE_LIBPRELUDE
	m_PreludeClient = NULL;
#endif
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
LogPrelude::~LogPrelude()
{

}



/**
 * bool Module::Init()
 * setup Module specific values 
 * here:
 * - register as EventHandler
 * - set wanted events
 * 
 * @return returns true if everything was fine, else false
 *         returning false will showup errors in warning a module
 */
bool LogPrelude::Init()
{

#ifdef HAVE_LIBPRELUDE

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	string analyzerClass;
	string analyzerModel;
	string analyzerName;

	try
	{
		analyzerClass = (m_Config->getValString("log-prelude.analyzerClass"));
		analyzerModel = m_Config->getValString("log-prelude.analyzerModel");
		analyzerName = m_Config->getValString("log-prelude.analyzerName");

	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}
	
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	m_Events.set(EV_SOCK_TCP_ACCEPT);
	m_Events.set(EV_SOCK_TCP_CLOSE);
	m_Events.set(EV_DIALOGUE_ASSIGN_AND_DONE);
	m_Events.set(EV_SHELLCODE_DONE);

	m_Events.set(EV_DOWNLOAD);
	m_Events.set(EV_SUBMISSION);


    const char *profile, *config;

	config = NULL;
	profile = analyzerName.c_str();




	int32_t ret;
// Initialize Prelude Library
	ret = prelude_init(NULL, NULL);
	if ( ret < 0 )
		logCrit("%s: Unable to initialize the Prelude library: %s.\n",
				prelude_strsource(ret), 
				prelude_strerror(ret));

// generate a new Prelude client
	ret = prelude_client_new(&m_PreludeClient, profile);

	if ( ret < 0 )
		logCrit("%s: Unable to create a prelude client object: %s.\n",
				prelude_strsource(ret), 
				prelude_strerror(ret));


	// set options in the analyzer-part of the client
	prelude_string_t *string;

	ret = idmef_analyzer_new_model(prelude_client_get_analyzer(m_PreludeClient), &string);
	if ( ret < 0 )
	        return false;
	prelude_string_set_constant(string, analyzerModel.c_str());
	
	ret = idmef_analyzer_new_class(prelude_client_get_analyzer(m_PreludeClient), &string);
	if ( ret < 0 )
	        return false;
	prelude_string_set_constant(string, analyzerClass.c_str());
	
	ret = idmef_analyzer_new_manufacturer(prelude_client_get_analyzer(m_PreludeClient), &string);
	if ( ret < 0 )
	        return false;
	prelude_string_set_constant(string, ANALYZER_MANUFACTURER);
	
	ret = idmef_analyzer_new_version(prelude_client_get_analyzer(m_PreludeClient), &string);
	if ( ret < 0 )
	        return false;
	        
	prelude_string_set_constant(string, NEPENTHES_VERSION);

//  start the Prelude Client
	ret = prelude_client_start(m_PreludeClient);
	if ( ret < 0 )
	{
		if ( prelude_client_is_setup_needed(ret) )
			prelude_client_print_setup_error(m_PreludeClient);

		logCrit("%s: Unable to initialize prelude client: %s.\n",
				   prelude_strsource(ret), prelude_strerror(ret));
	}

// set async Prelude Flags for the client, makes the application multithreaded
	ret = prelude_client_set_flags(m_PreludeClient, (prelude_client_flags_t) (PRELUDE_CLIENT_FLAGS_CONNECT | PRELUDE_CLIENT_FLAGS_ASYNC_SEND | PRELUDE_CLIENT_FLAGS_ASYNC_TIMER));
	if ( ret < 0 )
		logCrit("%s: Unable to set asynchronous send and timer: %s.\n",
				prelude_strsource(ret), 
				prelude_strerror(ret));
				

	REG_EVENT_HANDLER(this);
	return true;
#else 
	logCrit("Module log-prelude is compiled without libprelude, this wont work, reconfigure the whole source and recompile");
	return false;
#endif

}


/**
 * unregister as EventHandler, destroy the Prelude Client
 * 
 * @return returns true if everything was fine
 */
bool LogPrelude::Exit()
{
#ifdef HAVE_LIBPRELUDE
	if( m_PreludeClient != NULL)
	{
		prelude_client_destroy(m_PreludeClient, (prelude_client_exit_status_t)(PRELUDE_CLIENT_EXIT_STATUS_SUCCESS));
		prelude_deinit();
	}
	// disabled by harald due to segfaults
        //UNREG_EVENT_HANDLER(this);
#endif
	return true;
}



/**
 * This function adds char * idmef values into an idmef message
 * 
 */
#ifdef HAVE_LIBPRELUDE
int32_t add_idmef_object(idmef_message_t *message, const char *object, const char *value)
{
	int32_t ret=0;
	idmef_value_t *val;
	idmef_path_t *path;

	ret = idmef_path_new(&path, object);
	if ( ret < 0 )
	{
		logWarn("imdef error #1 %s -> %s %i (%s) \n",object,value,ret, prelude_strerror(ret));
		return -1;
	}

	ret = idmef_value_new_from_path(&val, path, value);
	if ( ret < 0 )
	{
		idmef_path_destroy(path);
		logWarn("imdef error #2 %s -> %s %i (%s) \n",object,value,ret, prelude_strerror(ret));
		return -1;
	}

	ret = idmef_path_set(path, message, val);

	idmef_value_destroy(val);
	idmef_path_destroy(path);
	return ret;
}


/**
 * 
 * This function adds int32_t idmef values into an idmef message
 */
int32_t add_idmef_object(idmef_message_t *message, const char *object, int32_t i)
{
	char value[20];
	memset(value,0,20);
	snprintf(value,19,"%i",i);
	return add_idmef_object(message,object,value);
}

#endif


/**
 * the handleEvent method is called whenever an event occurs 
 * the EventHandler wanted to have.
 * 
 * @param event  the Event
 * 
 * @return return 0
 */
uint32_t LogPrelude::handleEvent(Event *event)
{
//	logPF();
//	logInfo("Event %i\n",event->getType());
	switch(event->getType())
	{

	case EV_SOCK_TCP_ACCEPT:
		handleTCPaccept(event);
		break;

	case EV_SOCK_TCP_CLOSE:
		handleTCPclose(event);
		break;

	case EV_SUBMISSION:
		handleSubmission(event);
		break;
		
	case EV_DIALOGUE_ASSIGN_AND_DONE:
		handleDialogueAssignAndDone(event);
		break;
		
	case EV_SHELLCODE_DONE:
		handleShellcodeDone(event);
		break;
		
		
	case EV_DOWNLOAD:
		handleDownload(event);
		break;

	default:
		logWarn("this should not happen\n");
	}
	return 0;
}


void LogPrelude::handleTCPaccept(Event *event)
{
    

	logInfo("LogPrelude EVENT EV_SOCK_TCP_ACCEPT\n");

#ifdef HAVE_LIBPRELUDE
	Socket *socket = ((SocketEvent *)event)->getSocket();
	
	idmef_message_t *idmef;

	int32_t ret = idmef_message_new(&idmef);
	if ( ret < 0 )
		return;

	add_idmef_object(idmef, "alert.classification.text"						,"TCP Connection established");
	add_idmef_object(idmef, "alert.classification.ident", EV_SOCK_TCP_ACCEPT);
//	add_idmef_object(idmef, "alert.classification.reference(0).origin"		,"vendor-specific" );


	add_idmef_object(idmef, "alert.source(0).Spoofed"						,"no");
	add_idmef_object(idmef, "alert.source(0).Service.protocol"				,"TCP");
	add_idmef_object(idmef, "alert.source(0).Service.port"					,socket->getRemotePort());

	uint32_t addr = socket->getRemoteHost();
	string address = inet_ntoa(*(in_addr *)&addr);
	add_idmef_object(idmef, "alert.source(0).Node.Address(0).address"		,address.c_str());


	add_idmef_object(idmef, "alert.target(0).Decoy"							,"yes");
	add_idmef_object(idmef, "alert.target(0).Service.protocol"				,"TCP");
	add_idmef_object(idmef, "alert.target(0).Service.port"					,socket->getLocalPort());

	addr = socket->getLocalHost();
	address = inet_ntoa(*(in_addr *)&addr);
	add_idmef_object(idmef, "alert.target(0).Node.Address(0).address"		,address.c_str());




	idmef_time_t *time;

	ret = idmef_time_new_from_gettimeofday(&time);
	idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								time);


	// analyzer id
	idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							 idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)),							 
							 IDMEF_LIST_PREPEND);


	prelude_client_send_idmef(m_PreludeClient, idmef);

	//prelude_string_t *field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
	//const char *msgid = prelude_string_get_string(field);
	
	//logInfo("PreludeMessageID = %s \n",msgid);

	idmef_message_destroy(idmef);
#endif
}




void LogPrelude::handleTCPclose(Event *event)
{

	Socket *socket = ((SocketEvent *)event)->getSocket();

	if (! socket->isAccept())
	{
		return;
	}

	logInfo("LogPrelude EVENT EV_SOCK_TCP_CLOSE\n");

#ifdef HAVE_LIBPRELUDE
	
	idmef_message_t *idmef;

	int32_t ret = idmef_message_new(&idmef);
	if ( ret < 0 )
		return;


	add_idmef_object(idmef, "alert.classification.text"							,"TCP Connection closed");
	add_idmef_object(idmef, "alert.classification.ident", EV_SOCK_TCP_CLOSE);
//	add_idmef_object(idmef, "alert.classification.reference(0).origin"			,"vendor-specific" );


	add_idmef_object(idmef, "alert.source(0).Service.protocol"					,"TCP");
	add_idmef_object(idmef, "alert.source(0).Service.port"						,socket->getRemotePort());

	uint32_t addr = socket->getRemoteHost();
	string address = inet_ntoa(*(in_addr *)&addr);
	add_idmef_object(idmef, "alert.source(0).Node.Address(0).address"			,address.c_str());

	add_idmef_object(idmef, "alert.target(0).Service.protocol"					,"TCP");
	add_idmef_object(idmef, "alert.target(0).Service.port"						,socket->getLocalPort());

	addr = socket->getLocalHost();
	address = inet_ntoa(*(in_addr *)&addr);
	add_idmef_object(idmef, "alert.target(0).Node.Address(0).address",address.c_str());

	idmef_time_t *time;

	ret = idmef_time_new_from_gettimeofday(&time);
	idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								time);


	// analyzer id
	idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							 idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							 IDMEF_LIST_PREPEND);


	prelude_client_send_idmef(m_PreludeClient, idmef);

//	prelude_string_t *field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
//	const char *msgid = prelude_string_get_string(field);
	
//	logInfo("CloseMessageID = %s \n",msgid);

	idmef_message_destroy(idmef);
	
#endif
}


/**
 * Send idmef message when finished with the Shellcode
 * 
 */
void LogPrelude::handleShellcodeDone(Event *event)
{
	logInfo("LogPrelude EVENT EV_SHELLCODE_DONE\n");

#ifdef HAVE_LIBPRELUDE

	ShellcodeHandler *handler = ((ShellcodeEvent *)event)->getShellcodeHandler();
	Socket *socket = ((ShellcodeEvent *)event)->getSocket();

	idmef_message_t *idmef;

	int32_t ret = idmef_message_new(&idmef);
	if ( ret < 0 )
		return;
	string shellcodeText = "Shellcode detected: " + handler->getShellcodeHandlerName();
	add_idmef_object(idmef, "alert.classification.text", shellcodeText.c_str());
	// hl: added ident
	add_idmef_object(idmef, "alert.classification.ident", EV_SHELLCODE_DONE);

	//	add_idmef_object(idmef, "alert.classification.reference(0).origin"		,"vendor-specific" );


	add_idmef_object(idmef, "alert.source(0).Spoofed"					,"no");
	add_idmef_object(idmef, "alert.source(0).Service.protocol"				,"TCP");
	add_idmef_object(idmef, "alert.source(0).Service.port"					,socket->getRemotePort());

	uint32_t addr = socket->getRemoteHost();
	string address = inet_ntoa(*(in_addr *)&addr);
	add_idmef_object(idmef, "alert.source(0).Node.Address(0).address"		,address.c_str());


	add_idmef_object(idmef, "alert.target(0).Decoy"							,"yes");
	add_idmef_object(idmef, "alert.target(0).Service.protocol"				,"TCP");
	add_idmef_object(idmef, "alert.target(0).Service.port"					,socket->getLocalPort());

	addr = socket->getLocalHost();
	address = inet_ntoa(*(in_addr *)&addr);
	add_idmef_object(idmef, "alert.target(0).Node.Address(0).address"		,address.c_str());


	add_idmef_object(idmef, "alert.assessment.impact.description"			,"possible Shellcode has been detected.");
	add_idmef_object(idmef, "alert.assessment.impact.severity"			,"medium");
//    add_idmef_object(idmef, "alert.assessment.impact.completion"			,"succeeded");
	add_idmef_object(idmef, "alert.assessment.impact.type"				,"other");


	// hl: added for additional information
        add_idmef_object(idmef, "alert.additional_data(0).type", "string");
        add_idmef_object(idmef, "alert.additional_data(0).meaning", "Shellcode");
        add_idmef_object(idmef, "alert.additional_data(0).data", handler->getShellcodeHandlerName().c_str());

 
	 idmef_time_t *time;

	ret = idmef_time_new_from_gettimeofday(&time);
	idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								time);


	// analyzer id
	idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							 idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							 IDMEF_LIST_PREPEND);


	prelude_client_send_idmef(m_PreludeClient, idmef);

//	prelude_string_t *field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
//	const char *msgid = prelude_string_get_string(field);
//	logInfo("RecvMessageID = %s \n",msgid);

	idmef_message_destroy(idmef);
#endif
}


/**
 * 
 * handle submitted files
 */
void LogPrelude::handleSubmission(Event *event)
{
	 SubmitEvent *se = (SubmitEvent *)event;
	 Download *down = se->getDownload();

	 logInfo("LogPrelude EVENT EV_SUBMISSION %s %s %i \n",down->getUrl().c_str(), 
			 down->getMD5Sum().c_str(), 
			 down->getDownloadBuffer()->getSize());

#ifdef HAVE_LIBPRELUDE
	 idmef_message_t *idmef;

	 int32_t ret = idmef_message_new(&idmef);
	 if ( ret < 0 )
		 return;

	 // generic information
	 // hl: changed submited to submitted, added ident
	 add_idmef_object(idmef, "alert.classification.text"						,"Malware submitted");
	 add_idmef_object(idmef, "alert.classification.ident", EV_SUBMISSION);

	 string url = "http://nepenthes.sf.net/wiki/submission/" + down->getMD5Sum();
	 add_idmef_object(idmef, "alert.classification.reference(0).origin"			,"vendor-specific" );
	 add_idmef_object(idmef, "alert.classification.reference(0).url"			,url.c_str() );


	 // file name and info
	 // hl: changed file tags because of DTD violation
	 add_idmef_object(idmef, "alert.target(0).file(0).name"				,down->getDownloadUrl()->getFile().c_str());
	 add_idmef_object(idmef, "alert.target(0).file(0).path"				,down->getUrl().c_str());
	 add_idmef_object(idmef, "alert.target(0).file(0).category"			,"current");
	 add_idmef_object(idmef, "alert.target(0).file(0).ident"			,down->getMD5Sum().c_str());
	 add_idmef_object(idmef, "alert.target(0).file(0).data_size"			,down->getDownloadBuffer()->getSize());
	 
         //hl: some debug stuff, prelude-manager doesnt write the checksums into xml 
	 ret = add_idmef_object(idmef, "alert.target(0).file(0).checksum(0).algorithm"	,"MD5");
	 //logInfo("LogPrelude DEBUG MD5 %i\n", ret);
	 ret = add_idmef_object(idmef, "alert.target(0).file(0).checksum(0).value"		,down->getMD5Sum().c_str());
         //logInfo("LogPrelude DEBUG Hash %i\n", ret);
	 ret = add_idmef_object(idmef, "alert.target(0).file(0).checksum(1).algorithm"	,"SHA2-512");
         //logInfo("LogPrelude DEBUG SHA %i\n", ret);
	 ret = add_idmef_object(idmef, "alert.target(0).file(0).checksum(1).value"		,down->getSHA512Sum().c_str());
         //logInfo("LogPrelude DEBUG Hash %i\n", ret);

	 uint32_t addr = down->getLocalHost();
	 string address = inet_ntoa(*(in_addr *)&addr);
	 add_idmef_object(idmef, "alert.target(0).Node.Address(0).address"		,address.c_str());



	 // infection host
	 addr = down->getRemoteHost();
	 address = inet_ntoa(*(in_addr *)&addr);
	 add_idmef_object(idmef, "alert.source(0).Node.Address(0).address"			,address.c_str());


	 // download source
	 add_idmef_object(idmef, "alert.source(0).Service.port", down->getDownloadUrl()->getPort());
	 
	 /* hl: previous dirty workaround -> commented
	 string protocol;
	 if (down->getDownloadUrl()->getProtocol() == "tftp" )
		 protocol = "UDP";
	 else
		 protocol = "TCP";

	 add_idmef_object(idmef, "alert.source(0).Service.protocol"					,protocol.c_str());
	 */
	 
	 add_idmef_object(idmef, "alert.source(0).Service.web_service.url"			,down->getUrl().c_str());
	 // hl: not needed
	 //add_idmef_object(idmef, "alert.source(0).Service.web_service.http_method"	,"get");

	 add_idmef_object(idmef, "alert.assessment.impact.description"			,"possible Malware stored for further analysis");
	 add_idmef_object(idmef, "alert.assessment.impact.severity"				,"high");
//     add_idmef_object(idmef, "alert.assessment.impact.completion"			,"succeeded");
         add_idmef_object(idmef, "alert.assessment.impact.type"					,"other");

	 // time
	 idmef_time_t *time;
	 ret = idmef_time_new_from_gettimeofday(&time);
	 idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								 time);


	 // analyzer id
	 idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							  idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							  IDMEF_LIST_PREPEND);


	 prelude_client_send_idmef(m_PreludeClient, idmef);
	 idmef_message_destroy(idmef);
	 
#endif
}



/**
 * 
 * 
 * 
 */
void LogPrelude::handleDialogueAssignAndDone(Event *event)
{
	 logInfo("LogPrelude EVENT EV_ASSIGN_AND_DONE\n");

#ifdef HAVE_LIBPRELUDE

	 Dialogue *dia = ((DialogueEvent *)event)->getDialogue();
	 Socket *socket = ((DialogueEvent *)event)->getSocket();
	 idmef_message_t *idmef;

	 int32_t ret = idmef_message_new(&idmef);
	 if ( ret < 0 )
		 return;

	 string attack = "Exploit attempt: " + dia->getDialogueName();
	
	 // generic information
	 add_idmef_object(idmef, "alert.classification.text", attack.c_str());
	 // hl: added ident field
	 add_idmef_object(idmef, "alert.classification.ident", EV_DIALOGUE_ASSIGN_AND_DONE);

//	 add_idmef_object(idmef, "alert.classification.reference(0).origin"			,"vendor-specific" );


	 // attacker
	 uint32_t addr = socket->getRemoteHost();
	 string address = inet_ntoa(*(in_addr *)&addr);
	 add_idmef_object(idmef, "alert.source(0).Node.Address(0).address", address.c_str());

	 // target
	 addr = socket->getLocalHost();
	 address = inet_ntoa(*(in_addr *)&addr);
	 add_idmef_object(idmef, "alert.target(0).Node.Address(0).address", address.c_str());

//	 string protocol;
//	 if (down->getDownloadUrl()->getProtocol() == "tftp" )
//		 protocol = "UDP";
//	 else
//		 protocol = "TCP";
//
//	 add_idmef_object(idmef, "alert.source(0).Service.protocol"					,protocol.c_str());
//	 add_idmef_object(idmef, "alert.source(0).Service.web_service.url"			,down->getUrl().c_str());
//	 add_idmef_object(idmef, "alert.source(0).Service.web_service.http_method"	,"get");

	 add_idmef_object(idmef, "alert.assessment.impact.description"			,"An exploit attempt is getting handled.");
	 add_idmef_object(idmef, "alert.assessment.impact.severity"				,"low");
//       add_idmef_object(idmef, "alert.assessment.impact.completion"			,"succeeded");
         add_idmef_object(idmef, "alert.assessment.impact.type"					,"other");


	 // hl: added
         add_idmef_object(idmef, "alert.additional_data(0).type", "string");
         add_idmef_object(idmef, "alert.additional_data(0).meaning", "Dialogue");
         add_idmef_object(idmef, "alert.additional_data(0).data", dia->getDialogueName().c_str());

	 // time
	 idmef_time_t *time;
	 ret = idmef_time_new_from_gettimeofday(&time);
	 idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								 time);


	 // analyzer id
	 idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							  idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							  IDMEF_LIST_PREPEND);


	 prelude_client_send_idmef(m_PreludeClient, idmef);


	 idmef_message_destroy(idmef);

#endif
}






/**
 * 
 * 
 */
void LogPrelude::handleDownload(Event *event)
{
	 SubmitEvent *se = (SubmitEvent *)event;
	 Download *down = se->getDownload();
	 string url = se->getDownload()->getUrl();

	 se->getType();
	 logInfo("LogPrelude EVENT EV_DOWNLOAD %s %s %i \n",down->getUrl().c_str(), 
			 down->getMD5Sum().c_str(), 
			 down->getDownloadBuffer()->getSize());

#ifdef HAVE_LIBPRELUDE

	 idmef_message_t *idmef;

	 int32_t ret = idmef_message_new(&idmef);
	 if ( ret < 0 )
		 return;

	 // generic information
	 // hl: changed message
	 string message = "possible Malware offered: " + down->getUrl();
	 
	 add_idmef_object(idmef, "alert.classification.text", message.c_str());
         // hl: changed to ident number
	 add_idmef_object(idmef, "alert.classification.ident", EV_DOWNLOAD);

//	 add_idmef_object(idmef, "alert.classification.reference(0).origin"			,"vendor-specific" );


	 // infection host
	 uint32_t addr = down->getRemoteHost();
	 string address = inet_ntoa(*(in_addr *)&addr);
	 add_idmef_object(idmef, "alert.source(0).Node.Address(0).address"			,address.c_str());
	 //target host
	 addr = down->getLocalHost();
	 address = inet_ntoa(*(in_addr *)&addr);
	 add_idmef_object(idmef, "alert.target(0).Node.Address(0).address"			,address.c_str());


	 // download source
	 // hl: removed protocol, added url
	 /* 
	 string protocol;
	 if (down->getDownloadUrl()->getProtocol() == "tftp" )
		 protocol = "UDP";
	 else
		 protocol = "TCP";
	 */	 
	 add_idmef_object(idmef, "alert.source(0).Service.port"						,down->getDownloadUrl()->getPort());
	 //add_idmef_object(idmef, "alert.source(0).Service.protocol"					,protocol.c_str());
	 add_idmef_object(idmef, "alert.source(0).Service.web_service.url"			,down->getUrl().c_str());
//	 add_idmef_object(idmef, "alert.source(0).Service.web_service.http_method"	,"get");
	 add_idmef_object(idmef, "alert.assessment.impact.description"			,"Parsing the Shellcode has unrevealed a URL.");
	 add_idmef_object(idmef, "alert.assessment.impact.severity"				,"medium");
//     add_idmef_object(idmef, "alert.assessment.impact.completion"			,"succeeded");
     add_idmef_object(idmef, "alert.assessment.impact.type"					,"other");

	 // time
	 idmef_time_t *time;
	 ret = idmef_time_new_from_gettimeofday(&time);
	 idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								 time);


	 // analyzer id
	 idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							  idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							  IDMEF_LIST_PREPEND);


	 prelude_client_send_idmef(m_PreludeClient, idmef);


	 idmef_message_destroy(idmef);
#endif

}



extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new LogPrelude(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
