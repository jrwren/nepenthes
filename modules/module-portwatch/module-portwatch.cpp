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

#include "module-portwatch.hpp"
#include "WatchDialogue.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"



#include "Config.hpp"
#include "ShellcodeManager.hpp"


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
 * creates a new WatchModule Module, 
 * WatchModule is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
WatchModule::WatchModule(Nepenthes *nepenthes)
{
	m_ModuleName        = "module-portwatch";
	m_ModuleDescription = "look for activity on new ports";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "Watch Factory";
	m_DialogueFactoryDescription = "create Watch Dialogues";

	g_Nepenthes = nepenthes;
}

WatchModule::~WatchModule()
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
bool WatchModule::Init()
{
	logPF();
	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	StringList sList;
	int32_t timeout;
	try
	{
		sList = *m_Config->getValStringList("module-portwatch.ports");
		timeout = m_Config->getValInt("module-portwatch.accepttimeout");
	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	uint32_t i = 0;
	while (i < sList.size())
	{
		m_Nepenthes->getSocketMgr()->bindTCPSocket(0,atoi(sList[i]),0,timeout,this);
		i++;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();
	return true;
}

bool WatchModule::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new WatchModuleDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *WatchModule::createDialogue(Socket *socket)
{
	return new WatchDialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new WatchModule(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
