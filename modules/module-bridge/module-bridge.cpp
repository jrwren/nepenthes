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

#include "module-bridge.hpp"
#include "BridgeDialogueAccept.hpp"
#include "BridgeDialogueConnect.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "Config.hpp"

#include "Download.hpp"

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
 * creates a new BridgeModule Module, 
 * BridgeModule is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
BridgeModule::BridgeModule(Nepenthes *nepenthes)
{
	m_ModuleName        = "module-bridge";
	m_ModuleDescription = "bridge bad traffic to real hosts";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "bridge Factory";
	m_DialogueFactoryDescription = "creates bridge dialogues";

	g_Nepenthes = nepenthes;
}

BridgeModule::~BridgeModule()
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
bool BridgeModule::Init()
{
	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	StringList sList;
	int32_t timeout;

	m_BridgeHost = inet_addr("192.168.53.204");

	try
	{
		sList = *m_Config->getValStringList("module-bridge.ports");
		timeout = m_Config->getValInt("module-bridge.accepttimeout");
	} catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	uint32_t i = 0;
	while (i < sList.size())
	{
		m_Nepenthes->getSocketMgr()->bindTCPSocket(0,atoi(sList[i]),0,timeout,this);
		i++;
	}
	return true;
}

bool BridgeModule::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new BridgeModuleDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *BridgeModule::createDialogue(Socket *socket)
{

	Socket *bridgesocket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,m_BridgeHost,socket->getLocalPort(),30);

	BridgeDialogueAccept *adia = new BridgeDialogueAccept(socket,bridgesocket);
	BridgeDialogueConnect *cdia = new BridgeDialogueConnect(bridgesocket,socket);

	adia->setBridge(cdia);
	cdia->setBridge(adia);

	bridgesocket->addDialogue(cdia);

	return adia;
}









#ifdef WIN32
extern "C" int32_t __declspec(dllexport)  module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#else
extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#endif

{
	if (version == MODULE_IFACE_VERSION) {
        *module = new BridgeModule(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
