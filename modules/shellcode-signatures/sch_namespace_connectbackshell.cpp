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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sch_namespace_connectbackshell.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "Utilities.hpp"
#include "Socket.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "DialogueFactoryManager.hpp"
#include "DialogueFactory.hpp"

#include "parser.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

NamespaceConnectbackShell::NamespaceConnectbackShell(sc_shellcode *sc) : NamespaceShellcodeHandler(sc)
{
}

NamespaceConnectbackShell::~NamespaceConnectbackShell()
{

}

sch_result NamespaceConnectbackShell::handleShellcode(Message **msg)
{
	logSpam("%s checking ...\n",m_ShellcodeHandlerName.c_str());

	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 

	// host
	const char  *hostMatch	=	NULL;
	uint32_t 	host 		= 	0;

	// port
	const char  *portMatch	=  	NULL;
	uint16_t 	port		= 	0;
		

	const char  *hkeyMatch	=	NULL;
	uint32_t 	hostKey 		= 	0;

	// port
	const char  *pkeyMatch	=  	NULL;
	uint16_t 	portKey		= 	0;


	if ( (matchCount = pcre_exec(m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0 )
	{
		if ( (matchCount = pcre_exec(m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0 )
		{
			logSpam("MATCH %s  matchCount %i map_items %i \n",m_ShellcodeHandlerName.c_str(), matchCount, m_MapItems);
			int32_t i;
			for ( i=0; i < m_MapItems; i++ )
			{
				if (m_Map[i] == sc_none)
                		continue;
				

				logSpam(" i = %i map_items %i , map = %s\n",i,m_MapItems, sc_get_mapping_by_numeric(m_Map[i]));
				const char *match = NULL;
				pcre_get_substring((char *) shellcode, (int *)ovec, (int)matchCount, i, &match);

				switch ( m_Map[i] )
				{

				case sc_host:
					hostMatch = match;
					break;

				case sc_hostkey:
					hkeyMatch = match;
					break;

				case sc_portkey:
					pkeyMatch = match;
					break;

				case sc_port:
					portMatch = match;
					break;

				default:
					logCrit("%s not used mapping %s\n",m_ShellcodeHandlerName.c_str(), sc_get_mapping_by_numeric(m_Map[i]));
				}

			}
		}

		port = *((uint16_t *)portMatch);
		port = ntohs(port);

		host = (uint32_t)*((uint32_t *)hostMatch);

		if (hkeyMatch != NULL)
		{
                hostKey = *((uint32_t *)hkeyMatch);
				host = host ^ hostKey;
				pcre_free_substring(hkeyMatch);
		}

		if (pkeyMatch != NULL)
		{
				portKey = *((uint16_t *)pkeyMatch);
				port = port ^ portKey;
				pcre_free_substring(pkeyMatch);
		}


		
		pcre_free_substring(hostMatch);
		pcre_free_substring(portMatch);


		logInfo("%s -> %s:%u  \n",m_ShellcodeHandlerName.c_str(), inet_ntoa(*(in_addr *)&host), port);


		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost((*msg)->getLocalHost(),host,port,30);
		DialogueFactory *diaf;
		if ((diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL)
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}
		sock->addDialogue(diaf->createDialogue(sock));
		return SCH_DONE;


		return SCH_DONE;
	}
	return SCH_NOTHING;
}




