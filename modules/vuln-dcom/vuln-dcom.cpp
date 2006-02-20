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

#include "vuln-dcom.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DCOMDialogue.hpp"
#include "Socket.hpp"
#include "LogManager.hpp"
#include "Config.hpp"


#include "sch_dcom_sol2k_bind.hpp"
#include "sch_dcom_sol2k_connect.hpp"
#include "sch_dcom_oc192_bind.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_dia | l_hlr

using namespace nepenthes;

Nepenthes *g_Nepenthes;

DCOMVuln::DCOMVuln(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-dcom";
	m_ModuleDescription = "emulate the dcom vuln";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "dcom vuln Factory";
	m_DialogueFactoryDescription = "creating dialogues waiting for dcom";
	g_Nepenthes = nepenthes;
}

DCOMVuln::~DCOMVuln()
{
	logPF();
	while (m_ShellcodeHandlers.size() > 0)
	{
		delete m_ShellcodeHandlers.front();
		m_ShellcodeHandlers.pop_front();
	}
}

bool DCOMVuln::Init()
{
	logPF();
	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	StringList sList;
	int32_t timeout;
	try
	{
		sList = *m_Config->getValStringList("vuln-dcom.ports");
		timeout = m_Config->getValInt("vuln-dcom.accepttimeout");
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

	m_ModuleManager = m_Nepenthes->getModuleMgr();

// removed as they were not seen during the last 2 month and need a new pcre
//	m_ShellcodeHandlers.push_back( new SOL2KBind	(m_Nepenthes->getShellcodeMgr())); 
//	m_ShellcodeHandlers.push_back( new SOL2KConnect	(m_Nepenthes->getShellcodeMgr()));

// replaced by adenau xor & Parthenstein Bind
//	m_ShellcodeHandlers.push_back( new OC192Bind	(m_Nepenthes->getShellcodeMgr()));


	list <ShellcodeHandler *>::iterator handler;
	for (handler = m_ShellcodeHandlers.begin(); handler != m_ShellcodeHandlers.end(); handler++)
	{
		if ((*handler)->Init() == false)
        {
			logCrit("ERROR %s\n",__PRETTY_FUNCTION__);
			return false;
		}
		REG_SHELLCODE_HANDLER((*handler));

	}
	return true;
}

bool DCOMVuln::Exit()
{
	list <ShellcodeHandler *>::iterator handler;
	for (handler = m_ShellcodeHandlers.begin(); handler != m_ShellcodeHandlers.end(); handler++)
	{
		if ((*handler)->Exit() == false)
		{
			logCrit("ERROR %s\n",__PRETTY_FUNCTION__);
			return false;
		}
		m_Nepenthes->getShellcodeMgr()->unregisterShellcodeHandler((*handler));
	}
	return true;
}


Dialogue *DCOMVuln::createDialogue(Socket *socket)
{
	return new DCOMDialogue(socket);
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new DCOMVuln(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
