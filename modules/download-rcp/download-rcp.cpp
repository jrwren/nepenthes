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


#include "download-rcp.hpp"
#include "RCPDialogue.hpp"
#include "SocketManager.hpp"
#include "Socket.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "SubmitManager.hpp"
#include "Config.hpp"

#include "Buffer.cpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_hlr | l_mod

using namespace nepenthes;
Nepenthes *g_Nepenthes;
RCPDownloadHandler::RCPDownloadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "rcp Download Module";
	m_ModuleDescription = "provides a downloadhandler for rcp";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DownloadHandlerName ="rcp download handler";
	m_DownloadHandlerDescription = "download files via rcp";

	g_Nepenthes = nepenthes;
}

RCPDownloadHandler::~RCPDownloadHandler()
{
	logPF();
}

bool RCPDownloadHandler::Init()
{
	logPF();
/*
	if (m_Config == NULL)
	{
		logCrit("I need a config\n");
		return false;
	}

	try{
		m_MaxFileSize 	 = m_Config->getValInt("download-rcp.max-filesize");
		m_ConnectTimeout = m_Config->getValInt("download-rcp.connect-timeout");
	}
	catch(...)
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}
*/
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_DOWNLOAD_HANDLER(this,"rcp");

	
	return true;
}

bool RCPDownloadHandler::Exit()
{

	return true;
}

bool RCPDownloadHandler::download(Download *down)
{
	logPF();
	uint32_t Host = inet_addr(down->getDownloadUrl()->getHost().c_str());
//	Socket *socket = m_Nepenthes->getSocketMgr()connectTCPHost(,Host,1022,10);

	Socket *socket =NULL;
	int i=1000;
	while ((socket = g_Nepenthes->getSocketMgr()->connectTCPHost(down->getLocalHost(),Host,i,514,30)) == NULL && i <= 1023)
	{
		
		i+=2; // my rcpd does not like % 2 = 1 ports
	}
	if (socket == NULL)
	{
		logCrit("Could not bind to dedicated port %i\n",i);
		return false;
	}



	socket->addDialogue(new RCPDialogue(socket,down));

/* 
	int32_t Port = down->getDownloadUrl()->getPort();
	uint32_t Host = inet_addr(down->getDownloadUrl()->getHost().c_str());

	Socket *socket = m_Nepenthes->getSocketMgr()->connectTCPHost(down->getLocalHost(),Host,Port,m_ConnectTimeout);

    ((RCPDialogue*)dia)->setDownload(down);

	((RCPDialogue*)dia)->setMaxFileSize(m_MaxFileSize);	// FIXME CONFIG 4mb static value

	socket->addDialogue(dia);
*/	
	return true;
}



extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new RCPDownloadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
