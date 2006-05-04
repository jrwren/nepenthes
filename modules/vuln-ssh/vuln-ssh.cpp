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
#include <signal.h>
#include <sys/types.h>

#include "config.h"
#include "vuln-ssh.hpp"
#include "SSHDialogue.hpp"
#include "SSHSocket.hpp"

#include "SocketManager.hpp"

#include "SSHSocket.hpp"
#include "Socket.cpp"

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
 * creates a new SSHVuln Module, 
 * SSHVuln is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
SSHVuln::SSHVuln(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-ssh";
	m_ModuleDescription = "log ssh bruteforces";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;
}

SSHVuln::~SSHVuln()
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
bool SSHVuln::Init()
{
/*	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	StringList sList;
	int32_t timeout;
	try
	{
		sList = *m_Config->getValStringList("x-2.ports");
		timeout = m_Config->getValInt("x-2.accepttimeout");
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
*/	

#ifdef HAVE_LIBSSH
	signal(SIGPIPE,  SIG_IGN);	//      13       Term    Broken pipe: write to pipe with no readers
	SSH_OPTIONS *options=ssh_options_new();
	ssh_set_verbosity(10);
    ssh_options_set_dsa_server_key(options,"/etc/ssh/ssh_host_dsa_key");
    ssh_options_set_rsa_server_key(options,"/etc/ssh/ssh_host_rsa_key");
	ssh_options_set_bind(options,"0.0.0.0",22);

	SSHSocket *socket = new SSHSocket(options);
	socket->Init();


	g_Nepenthes->getSocketMgr()->addPOLLSocket(socket);
#endif /* HAVE_LIBSSH */
	return true;
}

bool SSHVuln::Exit()
{
	return true;
}



extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new SSHVuln(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
