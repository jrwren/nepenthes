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

#include "vuln-wins.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "WINSDialogue.hpp"
#include "Socket.hpp"
#include "LogManager.hpp"
#include "ShellcodeManager.hpp"
#include "ShellcodeHandler.hpp"

#include "Config.hpp"

#include "sch_wins_hs_connect.hpp"
#include "sch_wins_hs_bind.hpp"
#include "sch_wins_zuc_connect.hpp"

using namespace nepenthes;


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_dia | l_hlr

Nepenthes *g_Nepenthes;

WINSVuln::WINSVuln(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-wins";
	m_ModuleDescription = "emulate the wins vuln";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "wins vuln Factory";
	m_DialogueFactoryDescription = "creating dialogues wins";

	g_Nepenthes = nepenthes;
}

WINSVuln::~WINSVuln()
{
	logPF();
	while (m_ShellcodeHandlers.size() > 0)
	{
		delete m_ShellcodeHandlers.front();
		m_ShellcodeHandlers.pop_front();
	}
}

bool WINSVuln::Init()
{

	logPF();
	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	StringList sList;
	int timeout;
	try
	{
		sList = *m_Config->getValStringList("vuln-wins.ports");
		timeout = m_Config->getValInt("vuln-wins.accepttimeout");
	} catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	unsigned int i = 0;
	while (i < sList.size())
	{
		m_Nepenthes->getSocketMgr()->bindTCPSocket(0,atoi(sList[i]),0,timeout,this);
		i++;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();
	

	m_ShellcodeHandlers.push_back(new HATSQUADConnect(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new HATSQUADBind(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new ZUCConnect(m_Nepenthes->getShellcodeMgr()));

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

bool WINSVuln::Exit()
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


Dialogue *WINSVuln::createDialogue(Socket *socket)
{
	return new WINSDialogue(socket);
}

extern "C" int module_init(int version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new WINSVuln(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
