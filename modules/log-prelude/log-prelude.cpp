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
#include <idmef-message-print.h>
#include <prelude-io.h>
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
#define DEFAULT_ANALYZER_NAME "markus.koetter"
#define VERSION "$Rev$"


static int32_t setup_analyzer(idmef_analyzer_t *analyzer)
{
        int32_t ret;
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

	m_Events.set(EV_SOCK_TCP_ACCEPT);
	m_Events.set(EV_SOCK_TCP_CLOSE);
	m_Events.set(EV_SOCK_TCP_RX);

	m_Events.set(EV_TIMEOUT);


	int32_t ret;
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

//	ret = prelude_client_set_flags(m_PreludeClient,
//								   (prelude_client_flags_t)
//								   (prelude_client_get_flags(m_PreludeClient) | PRELUDE_CLIENT_FLAGS_ASYNC_SEND|PRELUDE_CLIENT_FLAGS_ASYNC_TIMER));
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
	if( m_PreludeClient != NULL)
		prelude_client_destroy(m_PreludeClient, PRELUDE_CLIENT_EXIT_STATUS_SUCCESS);

//	UNREG_EVENT_HANDLER(this);
	return true;
}

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


int32_t add_idmef_object(idmef_message_t *message, const char *object, int32_t i)
{
	char value[20];
	memset(value,0,20);
	snprintf(value,19,"%i",i);
	return add_idmef_object(message,object,value);
}



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
	logPF();
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

	case EV_SOCK_TCP_RX:
		handleTCPrecv(event);
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


void LogPrelude::handleTCPaccept(Event *event)
{
    Socket *socket = ((SocketEvent *)event)->getSocket();

	SocketContext *ctx = new SocketContext(socket);
	logInfo("Adding Socket 0x%x to Contexts\n",socket);
	m_Contexts.push_back(ctx);

	idmef_message_t *idmef;

	int32_t ret = idmef_message_new(&idmef);
	if ( ret < 0 )
		return;

	add_idmef_object(idmef, "alert.classification.text"						,"nepenthes::TCPSocket::acceptConnection");
	add_idmef_object(idmef, "alert.classification.reference(0).origin"		,"vendor-specific" );


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
//	add_idmef_object(idmef, "alert.target(0).Node.Address(0).address"		,address.c_str());

	add_idmef_object(idmef, "alert.assessment.impact.description"			,"possible malicious connection established");
	add_idmef_object(idmef, "alert.assessment.impact.severity"				,"low");
    add_idmef_object(idmef, "alert.assessment.impact.completion"			,"succeeded");
    add_idmef_object(idmef, "alert.assessment.impact.type"					,"other");


	idmef_time_t *time;

	ret = idmef_time_new_from_gettimeofday(&time);
	idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								time);


	// analyzer id
	idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							 idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							 0);


	prelude_client_send_idmef(m_PreludeClient, idmef);

//	prelude_string_t *field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
//	logInfo("PreludeMessageID = %s \n",prelude_string_get_string(field));

	prelude_string_t *field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
	const char *msgid = prelude_string_get_string(field);
	logInfo("PreludeMessageID = %s \n",msgid);
	addIDtoSocketContext(socket,(char *)msgid);


	idmef_message_destroy(idmef);
}

void LogPrelude::handleTCPclose(Event *event)
{

	Socket *socket = ((SocketEvent *)event)->getSocket();

	if (! socket->isAccept())
	{
		return;
	}

	SocketContext *ctx = *findSocketContext(socket);
	if (ctx == NULL)
	{
		logCrit("ctx is %x\n",ctx);
	}


	idmef_message_t *idmef;

	int32_t ret = idmef_message_new(&idmef);
	if ( ret < 0 )
		return;


	add_idmef_object(idmef, "alert.classification.text"							,"nepenthes::TCPSocket::~TCPSocket");
	add_idmef_object(idmef, "alert.classification.reference(0).origin"			,"vendor-specific" );


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
							 0);


	prelude_client_send_idmef(m_PreludeClient, idmef);

	prelude_string_t *field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
	const char *msgid = prelude_string_get_string(field);
	logInfo("CloseMessageID = %s \n",msgid);
	addIDtoSocketContext(socket,(char *)msgid);


	idmef_message_destroy(idmef);

//	return;

	ret = idmef_message_new(&idmef);
	if ( ret < 0 )
		return;
	add_idmef_object(idmef, "alert.correlation_alert.name"						,"TCP Session");

	char path[128];

	list<string>::iterator it;
	uint32_t i=0;

	for (it=ctx->m_Collection.begin();it!=ctx->m_Collection.end();it++,i++)
	{
		memset(path,0,128);
		snprintf(path,127,"alert.correlation_alert.alertident(%i).alertident",i);
		add_idmef_object(idmef,path,it->c_str() );
	}
    

	ret = idmef_time_new_from_gettimeofday(&time);
	idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								time);


	// analyzer id
	idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							 idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							 0);


	prelude_client_send_idmef(m_PreludeClient, idmef);

	field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
	msgid = prelude_string_get_string(field);
	logInfo("CorrelationMsgID = %s \n",msgid);

	idmef_message_destroy(idmef);

}


void LogPrelude::handleTCPrecv(Event *event)
{
	Message *msg = ((MessageEvent *)event)->getMessage();
	Socket *socket = ((MessageEvent *)event)->getMessage()->getSocket();

	SocketContext *ctx = new SocketContext(socket);
	logInfo("Adding Socket 0x%x to Contexts\n",socket);
	m_Contexts.push_back(ctx);

	idmef_message_t *idmef;

	int32_t ret = idmef_message_new(&idmef);
	if ( ret < 0 )
		return;

	add_idmef_object(idmef, "alert.classification.text"						,"nepenthes::TCPSocket::doRecv");
	add_idmef_object(idmef, "alert.classification.reference(0).origin"		,"vendor-specific" );


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

	add_idmef_object(idmef, "alert.additional_data(0).type"					,"byte-string");


	unsigned char *payload = g_Nepenthes->getUtilities()->b64encode_alloc((unsigned char *)msg->getMsg(),msg->getSize());
	add_idmef_object(idmef, "alert.additional_data(0).data"					,(char *)payload);
	free(payload);
	
	add_idmef_object(idmef, "alert.additional_data(0).meaning"					,"the payload");

	idmef_time_t *time;

	ret = idmef_time_new_from_gettimeofday(&time);
	idmef_alert_set_create_time(idmef_message_get_alert(idmef), 
								time);


	// analyzer id
	idmef_alert_set_analyzer(idmef_message_get_alert(idmef), 
							 idmef_analyzer_ref(prelude_client_get_analyzer(m_PreludeClient)), 
							 0);


	prelude_client_send_idmef(m_PreludeClient, idmef);

	prelude_string_t *field = idmef_alert_get_messageid(idmef_message_get_alert(idmef));
	const char *msgid = prelude_string_get_string(field);
	logInfo("RecvMessageID = %s \n",msgid);
	addIDtoSocketContext(socket,(char *)msgid);

	idmef_message_destroy(idmef);
}

void LogPrelude::handleSubmission(Event *event)
{
	SubmitEvent *se = (SubmitEvent *)event;
	 Download *down = se->getDownload();

	 se->getType();
	 logInfo("LogPrelude EVENT EV_SUBMISSION %s %s %i \n",down->getUrl().c_str(), 
			 down->getMD5Sum().c_str(), 
			 down->getDownloadBuffer()->getSize());

	 idmef_message_t *idmef;

	 int32_t ret = idmef_message_new(&idmef);
	 if ( ret < 0 )
		 return;

	 // generic information
	 add_idmef_object(idmef, "alert.classification.text"						,"nepenthes::SubmitManager::Submit");

	 string url = "http://nepenthes.sf.net/wiki/submission/" + down->getMD5Sum();
	 add_idmef_object(idmef, "alert.classification.reference(0).origin"			,"vendor-specific" );
	 add_idmef_object(idmef, "alert.classification.reference(0).url"			,url.c_str() );


	 // file name and info
	 add_idmef_object(idmef, "alert.target(0).file(0).name"						,down->getDownloadUrl()->getFile().c_str());
	 add_idmef_object(idmef, "alert.target(0).file(0).data_size"				,down->getDownloadBuffer()->getSize());
	 add_idmef_object(idmef, "alert.target(0).file(0).Checksum(0).algorithm"	,"MD5");
	 add_idmef_object(idmef, "alert.target(0).file(0).Checksum(0).value"		,down->getMD5Sum().c_str());
//			add_idmef_object(idmef, "alert.target(0).file(0).Checksum(0).category","current");
	 add_idmef_object(idmef, "alert.target(0).file(0).Checksum(1).algorithm"	,"SHA2-512");
	 add_idmef_object(idmef, "alert.target(0).file(0).Checksum(1).value"		,down->getSHA512Sum().c_str());


	 // infection host
	 uint32_t addr = down->getAddress();
	 string address = inet_ntoa(*(in_addr *)&addr);
	 add_idmef_object(idmef, "alert.source(1).Node.Address(0).address"			,address.c_str());


	 // download source
	 add_idmef_object(idmef, "alert.source(0).Service.port"						,down->getDownloadUrl()->getPort());

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





list<SocketContext *>::iterator LogPrelude::findSocketContext(Socket *socket)
{
	list<SocketContext *>::iterator it;
	for (it=m_Contexts.begin();it!=m_Contexts.end();it++)
	{
		if ((*it)->getSocket() == socket)
		{
			return it;
		}
	}
	return NULL;
}

bool LogPrelude::addIDtoSocketContext(Socket *s,char *msgid)
{
	list<SocketContext *>::iterator ctx;
	if (( ctx = findSocketContext(s)) == NULL )
	{
		return false;
	}

    (*ctx)->m_Collection.push_back(msgid);
//	printf("Context %x\n\t %s \n",(uint32_t)(*ctx),(*ctx)->m_Collection.c_str());
	return true;
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

