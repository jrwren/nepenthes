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

#include "vuln-sub7.hpp"
#include "SUB7Dialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"

#include "Config.hpp"

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
 * creates a new SUB7Vuln Module, 
 * SUB7Vuln is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
SUB7Vuln::SUB7Vuln(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-sub7";
	m_ModuleDescription = "emulate the sub7 backdoor";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "SUB7DialogueFactory";
	m_DialogueFactoryDescription = "creates Sub7 Dialogues";

	g_Nepenthes = nepenthes;
}

SUB7Vuln::~SUB7Vuln()
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
bool SUB7Vuln::Init()
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
		sList = *m_Config->getValStringList("vuln-sub7.ports");
		timeout = m_Config->getValInt("vuln-sub7.accepttimeout");
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

bool SUB7Vuln::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new SUB7VulnDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *SUB7Vuln::createDialogue(Socket *socket)
{
	return new SUB7Dialogue(socket);
}



extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new SUB7Vuln(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

