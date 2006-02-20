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


#include "submit-xmlrpc.hpp"
#include "XMLRPCDialogue.hpp"
#include "XMLRPCContext.hpp"
#include "XMLRPCParser.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadUrl.cpp"

#include "DownloadBuffer.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"

#include "EventManager.hpp"

#include "Config.hpp"

#include "DNSManager.hpp"

#include "DNSCallback.cpp"
#include "EventHandler.cpp"

#include "GeoLocationManager.hpp"
#include "GeoLocationResult.hpp"

#include "UploadManager.hpp"
#include "UploadResult.hpp"

using namespace nepenthes;


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;
SubmitXMLRPC *g_SubmitXMLRPC;

/**
 * Constructor
 * creates a new SubmitXMLRPC Module, where SubmitXMLRPC is public Module, public SubmitHanvler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the SubmitHandlerName
 * - sets the SubmitHandlerDescription
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
SubmitXMLRPC::SubmitXMLRPC(Nepenthes *nepenthes)
{
	m_ModuleName        = "submit-xmlrpc";
	m_ModuleDescription = "submit files to xmlrpc";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_SubmitterName = "submit-xmlrpc";
	m_SubmitterDescription = "submit files to xmlrpc";

	g_Nepenthes = nepenthes;
	g_SubmitXMLRPC = this;

}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
SubmitXMLRPC::~SubmitXMLRPC()
{

}

/**
 * Module::Init()
 * register the submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a module loading error
 */
bool SubmitXMLRPC::Init()
{
	logPF();

	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	try
	{
		m_XMLRPCServer = m_Config->getValString("submit-xmlrpc.server");
    } catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();


	REG_SUBMIT_HANDLER(this);
//	REG_EVENT_HANDLER(this);

//	m_Events.set(EV_TIMEOUT);
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
bool SubmitXMLRPC::Exit()
{
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
void SubmitXMLRPC::Submit(Download *down)
{
	logPF();
	XMLRPCContext *ctx;
	ctx = new XMLRPCContext(down->getMD5Sum(), 
							down->getUrl(), 
							(unsigned char *)down->getDownloadBuffer()->getData(), 
							down->getDownloadBuffer()->getSize(), 
							down->getRemoteHost(), 
							CS_INIT_SESSION);
#ifdef HAVE_GEOLOCATION
	g_Nepenthes->getGeoMgr()->addGeoLocation(this,down->getRemoteHost(),ctx);
#else
	string request = ctx->getRequest();
	g_Nepenthes->getUploadMgr()->uploadUrl((char *)m_XMLRPCServer.c_str(),(char *)request.c_str(),request.size(),this,ctx);
#endif	
}



/**
 * SubmitHandler::Hitt(Download *down)
 * 
 * get and submit a file.
 * 
 * 
 * @param down   the download to hexdump
 */
void SubmitXMLRPC::Hit(Download *down)
{
	Submit(down);
	return;
}


#ifdef HAVE_GEOLOCATION
void SubmitXMLRPC::locationSuccess(GeoLocationResult *result)
{
	XMLRPCContext *ctx = (XMLRPCContext *)result->getObject();
	ctx->setLocation(result);
	string request = ctx->getRequest();
	g_Nepenthes->getUploadMgr()->uploadUrl((char *)m_XMLRPCServer.c_str(),(char *)request.c_str(),request.size(),this,ctx);
}


void SubmitXMLRPC::locationFailure(GeoLocationResult *result)
{
	XMLRPCContext *ctx = (XMLRPCContext *)result->getObject();
	ctx->setLocation(NULL);
	string request = ctx->getRequest();
	g_Nepenthes->getUploadMgr()->uploadUrl((char *)m_XMLRPCServer.c_str(),(char *)request.c_str(),request.size(),this,ctx);
}
#endif


void SubmitXMLRPC::uploadSuccess(UploadResult *up)
{
	logPF();

	XMLRPCContext *ctx  = (XMLRPCContext *)up->getObject();

	switch ( ctx->getState() )
	{
	case CS_INIT_SESSION:
		logSpam("CS_INIT_SESSION (%i bytes)\n%.*s\n",up->getSize(),up->getSize(),up->getData());
		break;

	case CS_OFFER_MALWARE:
		logSpam("CS_OFFER_MALWARE (%i bytes)\n%.*s\n",up->getSize(),up->getSize(),up->getData());
		break;

	case CS_SEND_MALWARE:
		logSpam("CS_SEND_MALWARE (%i bytes)\n%.*s\n",up->getSize(),up->getSize(),up->getData());
		break;
	}



	string s(up->getData(),up->getSize());
    XMLRPCParser p((char *)s.c_str());

	string request;

	const char *value;
	switch ( ctx->getState() )
	{
	case CS_INIT_SESSION:
//		logSpam("CS_INIT_SESSION (%i bytes)\n%.*s\n",up->getSize(),up->getSize(),up->getData());
		value = p.getValue("methodResponse.params.param.value.array.data.value.string");
		ctx->setSessionID((char *)value);
		ctx->setState(CS_OFFER_MALWARE);
		request = ctx->getRequest();
		g_Nepenthes->getUploadMgr()->uploadUrl((char *)m_XMLRPCServer.c_str(),(char *)request.c_str(),request.size(),this,ctx);
		break;

	case CS_OFFER_MALWARE:
//		logSpam("CS_OFFER_MALWARE (%i bytes)\n%.*s\n",up->getSize(),up->getSize(),up->getData());
		
		value = p.getValue("methodResponse.params.param.value.boolean");
		if ( memcmp(value,"1",1) != 0 )
		{
			logInfo("Central server already knows file %s\n",value);
			delete ctx;
			return;
		}
		ctx->setState(CS_SEND_MALWARE);
		request = ctx->getRequest();
		g_Nepenthes->getUploadMgr()->uploadUrl((char *)m_XMLRPCServer.c_str(),(char *)request.c_str(),request.size(),this,ctx);
		break;

	case CS_SEND_MALWARE:
//		logSpam("CS_SEND_MALWARE (%i bytes)\n%.*s\n",up->getSize(),up->getSize(),up->getData());
		value = p.getValue("methodResponse.params.param.value.string");
		logDebug("Submit-XMLRPC was %s\n",value);
		delete ctx;
		break;
	}
}

void SubmitXMLRPC::uploadFailure(UploadResult *up)
{
	logCrit("UPLOAD FAILED %x\n",(uint32_t )((intptr_t)up));
	XMLRPCContext *ctx  = (XMLRPCContext *)up->getObject();
	delete ctx;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if(version == MODULE_IFACE_VERSION)
	{
		*module = new SubmitXMLRPC(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}




