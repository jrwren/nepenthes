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

#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sch_namespace_connectbackfiletransfer.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "Utilities.hpp"
#include "Socket.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "DownloadManager.hpp"


#include "parser.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

NamespaceConnectbackFiletransfer::NamespaceConnectbackFiletransfer(sc_shellcode *sc)
{
	m_ShellcodeHandlerName = sc_get_namespace_by_numeric(sc->nspace);
	m_ShellcodeHandlerName += "::";
	m_ShellcodeHandlerName += sc->name;

	m_Shellcode = sc;
}

NamespaceConnectbackFiletransfer::~NamespaceConnectbackFiletransfer()
{

}

bool NamespaceConnectbackFiletransfer::Init()
{
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if ( (m_Pcre = pcre_compile(m_Shellcode->pattern, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL )
	{
		logCrit("%s could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				m_ShellcodeHandlerName.c_str(), pcreEerror, pcreErrorPos);
		return false;
	} else
	{
		logInfo("%s loaded ...\n",m_ShellcodeHandlerName.c_str());
	}

//	printf("%s\n",m_Shellcode->pattern);
//	g_Nepenthes->getUtilities()->hexdump((byte *)m_Shellcode->pattern,m_Shellcode->pattern_size);
	return true;
}

bool NamespaceConnectbackFiletransfer::Exit()
{
	return true;
}

sch_result NamespaceConnectbackFiletransfer::handleShellcode(Message **msg)
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
		

	// key
	const char  *keyMatch	=  	NULL;


	if ( (matchCount = pcre_exec(m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0 )
	{
		if ( (matchCount = pcre_exec(m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0 )
		{
			logCrit("MATCH %s  matchCount %i map_items %i \n",m_ShellcodeHandlerName.c_str(), matchCount, m_Shellcode->map_items);
			int32_t i;
			for ( i=0; i < m_Shellcode->map_items; i++ )
			{
				if (m_Shellcode->map[i] == sc_none)
                		continue;
				

				logInfo(" i = %i map_items %i , map = %s\n",i,m_Shellcode->map_items, sc_get_mapping_by_numeric(m_Shellcode->map[i]));
				const char *match = NULL;
				pcre_get_substring((char *) shellcode, (int *)ovec, (int)matchCount, i, &match);

				switch ( m_Shellcode->map[i] )
				{

				case sc_host:
					hostMatch = match;
					host = (uint32_t)*((uint32_t *)hostMatch);
					break;

				case sc_port:
					portMatch = match;
					port = *((uint16_t *)portMatch);
					port = ntohs(port);
					break;

				case sc_key:
					 keyMatch = match;
					 break;

				default:
					logCrit("%s not used mapping %s\n",m_ShellcodeHandlerName.c_str(), sc_get_mapping_by_numeric(m_Shellcode->map[i]));
				}

			}
		}


		


		


		logInfo("%s -> %s:%u  \n",m_ShellcodeHandlerName.c_str(), inet_ntoa(*(in_addr *)&host), port);

		if (keyMatch != NULL)
		{
			unsigned char *authKey = (unsigned char *)keyMatch;
			
			logInfo("%s -> %s:%d, key 0x%02x%02x%02x%02x.\n",m_ShellcodeHandlerName.c_str(),
					inet_ntoa(*(in_addr *)&host), port, authKey[0], authKey[1], authKey[2], authKey[3]);


			char *url;
			unsigned char *base64Key = g_Nepenthes->getUtilities()->b64encode_alloc(authKey,4);

			asprintf(&url,"link://%s:%i/%s",inet_ntoa(*(in_addr *)&host),port,base64Key);
			g_Nepenthes->getDownloadMgr()->downloadUrl((*msg)->getLocalHost(),url,(*msg)->getRemoteHost(),url,0);
			free(url);
			free(base64Key);
		}else
		{
			logInfo("%s -> %s:%u  \n",m_ShellcodeHandlerName.c_str(), inet_ntoa(*(in_addr *)&host), port);

			char *url;
			asprintf(&url,"csend://%s:%d/%i",inet_ntoa(*(in_addr *)&host), port, 0);
			g_Nepenthes->getDownloadMgr()->downloadUrl((*msg)->getLocalHost(),url, (*msg)->getRemoteHost(), url,0);
			free(url);

		}
		


		pcre_free_substring(hostMatch);
		pcre_free_substring(portMatch);
		pcre_free_substring(keyMatch);

		return SCH_DONE;
	}
	return SCH_NOTHING;
}




