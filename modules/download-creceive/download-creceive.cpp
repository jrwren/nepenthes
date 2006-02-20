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

#include <ctype.h>

#include "download-creceive.hpp"
#include "CReceiveDialogue.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "DialogueFactoryManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * The Constructor
 * creates a new CReceiveDownloadHandler Module, 
 * CReceiveDownloadHandler is an example for binding a socket & setting up the Dialogue & DialogueFactory
 * 
 * 
 * it can be used as a shell emu to allow trigger commands 
 * 
 * 
 * sets the following values:
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
CReceiveDownloadHandler::CReceiveDownloadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "download-creceive";
	m_ModuleDescription = "downloads file bei rx";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "download-creceive";
	m_DialogueFactoryDescription = "FIXME";

	m_DownloadHandlerName ="creceive download handler";
	m_DownloadHandlerDescription = "receive files via tcp";

	g_Nepenthes = nepenthes;
}

CReceiveDownloadHandler::~CReceiveDownloadHandler()
{

}


/**
 * Module::Init()
 * 
 * binds the port, adds the DialogueFactory to the Socket
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool CReceiveDownloadHandler::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
    REG_DOWNLOAD_HANDLER(this,"creceive");
    return true;
}

bool CReceiveDownloadHandler::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new CReceiveDownloadHandlerDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *CReceiveDownloadHandler::createDialogue(Socket *socket)
{
	return new CReceiveDialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}

/**
 * DownloadHandler::download(Download *down)
 * 
 * whenever a file has to be downloaded using our protocol handle, 
 * this method will get the file.
 * -104
 * creates a Socket, and adds the X3Dialogue to this Socket
 * 
 * @param down   the Download info for the file we got to pull
 * 
 * @return returns true if everything was fine, else false
 */
bool CReceiveDownloadHandler::download(Download *down)
{
//	unsigned long host = inet_addr(down->getDownloadUrl()->getHost().c_str());
	unsigned short port = down->getDownloadUrl()->getPort();

	Socket *sock ;
	if ((sock= g_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,30,30)) != NULL)
	{
    	sock->addDialogueFactory(this);
	}else
	{
		logCrit("Could not bind socket to port %i\n",port);
	}

	delete down;
    return true;
}

extern "C" int module_init(int version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new CReceiveDownloadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
