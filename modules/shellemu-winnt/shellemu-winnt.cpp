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

#include "shellemu-winnt.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "WinNTShellDialogue.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

Nepenthes *g_Nepenthes;


WinNTShell::WinNTShell(Nepenthes *nepenthes)
{
	m_ModuleName        = "shellemu module";
	m_ModuleDescription = "privdes a factory for winnt shell dialogues";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "WinNTShell DialogueFactory";
	m_DialogueFactoryDescription = "creates winnt shell dialogues";

	g_Nepenthes = nepenthes;
}

WinNTShell::~WinNTShell()
{

}

bool WinNTShell::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_DIALOGUEFACTORY(this);
   	return true;
}

bool WinNTShell::Exit()
{
	return true;
}


Dialogue *WinNTShell::createDialogue(Socket *socket)
{
	return new WinNTShellDialogue(socket);
}












extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new WinNTShell(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
