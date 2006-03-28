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

#include <sys/types.h>
#include <netinet/in.h>

#include "sch_namespace_bindshell.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "Utilities.hpp"
#include "Socket.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "DialogueFactoryManager.hpp"


#include "parser.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

NamespaceBindShell::NamespaceBindShell(sc_shellcode *sc) : NamespaceShellcodeHandler(sc)
{
}

NamespaceBindShell::~NamespaceBindShell()
{

}

sch_result NamespaceBindShell::handleShellcode(Message **msg)
{
	logSpam("%s checking %i...\n",m_ShellcodeHandlerName.c_str(), (*msg)->getSize());

	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 


	if ( (matchCount = pcre_exec(m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0 )
	{
		 const char * match;

// the bind port
//		 const char *portMatch;
		 uint16_t port=0;

		for ( int i=0; i < m_MapItems; i++ )
		{
			if ( m_Map[i] == sc_port )
			{
				pcre_get_substring((char *) shellcode, (int *)ovec, (int)matchCount, 1, &match);
				port = ntohs(*(uint16_t *) match);
				pcre_free_substring(match);
			}
		}

		logInfo("%s :%u \n",m_ShellcodeHandlerName.c_str(), port);

		Socket *socket;
		if ( (socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,60,30)) == NULL )
		{
			logCrit("%s","Could not bind socket %u \n",port);
			return SCH_DONE;
		}

		DialogueFactory *diaf;
		if ( (diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL )
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}

		socket->addDialogueFactory(diaf);
		return SCH_DONE;
	 }


	return SCH_NOTHING;
}




