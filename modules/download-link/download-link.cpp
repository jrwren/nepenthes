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


#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "download-link.hpp"
#include "LinkDialogue.hpp"

#include "FILESocket.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "SubmitManager.hpp"
#include "Config.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_hlr | l_mod

using namespace nepenthes;
Nepenthes *g_Nepenthes;
LinkDownloadHandler::LinkDownloadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "Link Download Module";
	m_ModuleDescription = "provides a downloadhandler for link protocol";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DownloadHandlerName ="link download handler";
	m_DownloadHandlerDescription = "download files via tcp";

	g_Nepenthes = nepenthes;
}

LinkDownloadHandler::~LinkDownloadHandler()
{
	logPF();
}

bool LinkDownloadHandler::Init()
{
	logPF();

	if (m_Config == NULL)
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	try{
		m_MaxFileSize 	 = m_Config->getValInt("download-link.max-filesize");
		m_ConnectTimeout = m_Config->getValInt("download-link.connect-timeout");
	}
	catch(...)
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_DOWNLOAD_HANDLER(this,"link");
	REG_DOWNLOAD_HANDLER(this,"blink");
	
	return true;
}

bool LinkDownloadHandler::Exit()
{

	return true;
}

bool LinkDownloadHandler::download(Download *down)
{
	logPF();

	if (down->getDownloadUrl()->getProtocol() == "link")
	{
    	int32_t Port = down->getDownloadUrl()->getPort();
		uint32_t Host = inet_addr(down->getDownloadUrl()->getHost().c_str());

		Socket *socket = m_Nepenthes->getSocketMgr()->connectTCPHost(down->getLocalHost(),Host,Port,m_ConnectTimeout);

		Dialogue *dia = new LinkDialogue(socket,down,m_MaxFileSize);
		socket->addDialogue(dia);
        return true;
	}else
	if (down->getDownloadUrl()->getProtocol() == "blink")
	{
		Socket *socket;
		int32_t port = down->getDownloadUrl()->getPort();
		if ( ( socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,30,30)) != NULL )
		{
			socket->addDialogueFactory(this);
			
		}else
		{
			logWarn("Could not bind port %i \n",port);
			return false;
		}

		LinkBindContext *ctx = new LinkBindContext;
		ctx->m_Challenge = down->getDownloadUrl()->getPath();
		ctx->m_LocalPort = down->getDownloadUrl()->getPort();
		ctx->m_Download = down;

		m_BindContexts.push_back(ctx);

//		uint32_t host = down->getAddress();
		return true;

	}
	return false;

}




Dialogue *LinkDownloadHandler::createDialogue(Socket *socket)
{
	logPF();
	list <LinkBindContext *>::iterator it;
	LinkBindContext *ctx = NULL;

	for (it=m_BindContexts.begin();it!=m_BindContexts.end();it++)
	{
		if ((*it)->m_LocalPort == socket->getLocalPort())
		{
			ctx = *it;
			break;
		}
	}

	if (ctx != NULL)
	{
		Download *down = ctx->m_Download;

		// remove ctx
		delete ctx;
		m_BindContexts.erase(it);

        return new LinkDialogue(socket,down , m_MaxFileSize);	
	}else
	{
		return NULL;
	}
	
}


void LinkDownloadHandler::socketClosed(Socket *socket)
{
	logPF();
	list <LinkBindContext *>::iterator it;
	LinkBindContext *ctx = NULL;

	for (it=m_BindContexts.begin();it!=m_BindContexts.end();it++)
	{
		if ((*it)->m_LocalPort == socket->getLocalPort())
		{
			ctx = *it;
			break;
		}
	}

	if (ctx != NULL)
	{
		// remove ctx
		delete ctx->m_Download;
		delete ctx;
		m_BindContexts.erase(it);
	}
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new LinkDownloadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
