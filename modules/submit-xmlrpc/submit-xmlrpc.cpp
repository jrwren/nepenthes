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

#include "DNSHandler.cpp"
#include "EventHandler.cpp"


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

	m_EventHandlerName = "submit-xmlrpc";
	m_EventHandlerDescription = "timeout handler for submit-xmlrpc";


	g_Nepenthes = nepenthes;
	g_SubmitXMLRPC = this;

	m_Timeout = time(NULL);
	m_HTTPPipeline = false;

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
//		m_HTTPPipeline = m_Config->getValString("submit-xmlrpc.pipeline")!=0;
		string url = m_Config->getValString("submit-xmlrpc.server");

		DownloadUrl durl((char *)url.c_str());

		m_XMLRPCServerAddress 	= durl.getHost();
		m_XMLRPCServerPath    	= durl.getPath();
		m_XMLRPCServerPort 		= durl.getPort();
    } catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();


	REG_SUBMIT_HANDLER(this);
//	REG_EVENT_HANDLER(this);

	m_Events.set(EV_TIMEOUT);
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
							down->getDownloadBuffer()->getLength(), 
							down->getAddress(), 
							CS_INIT_SESSION);

	g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_XMLRPCServerAddress.c_str(),ctx);
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


bool SubmitXMLRPC::dnsResolved(DNSResult *result)
{
	logDebug("url %s resolved %i for %x\n",result->getDNS().c_str(), result->getIP4List().size(), (uint32_t) result->getObject());

	list <uint32_t> resolved = result->getIP4List();
	uint32_t host = resolved.front();
	XMLRPCContext *ctx = (XMLRPCContext *)result->getObject();

	Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,host,m_XMLRPCServerPort,30);
	socket->addDialogue(new XMLRPCDialogue(socket,ctx, m_HTTPPipeline));
	return true;
}

bool SubmitXMLRPC::dnsFailure(DNSResult *result)
{
	// FIXME HARD
	return true;
}




uint32_t SubmitXMLRPC::handleEvent(Event *event)
{
	logPF();
	if ( event->getType() != EV_TIMEOUT )
	{
		logCrit("Unwanted event %i\n",event->getType());
		return 1;
	}


//  dasmit wir nicht dauernd was downloaden mussen, einfach jeden 10ten timeout nen event fahren
	if ( rand()%100 > 20 )
		Submit(NULL);
	m_Timeout =time(NULL)+1;
	return true;
}

string SubmitXMLRPC::getXMLRPCHost()
{
	return m_XMLRPCServerAddress;
}

string SubmitXMLRPC::getXMLRPCPath()
{
	return m_XMLRPCServerPath;
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




