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

#include <netinet/in.h>
#include <arpa/inet.h>

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_generic_connect.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"
#include "DialogueFactory.hpp"

#include "Config.hpp"
#include "shellcode-generic.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

GenericConnect::GenericConnect(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "GenericConnect";
	m_ShellcodeHandlerDescription = "various bindshells";
	
}

GenericConnect::~GenericConnect()
{

}

bool GenericConnect::Init()
{
	logPF();

	StringList sList;
	try
	{
		sList = *g_GenericShellcodeHandler->getConfig()->getValStringList("shellcode-generic.generic_connect");
	} catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	uint32_t i = 0;
	while (i < sList.size())
	{
		const char *name = sList[i];
		i++;

		const char *pattern = sList[i];
		logInfo("pcre is %s \n",pattern);
		const char * pcreEerror;
		int32_t pcreErrorPos;
		pcre *mypcre=NULL;
		if((mypcre = pcre_compile(pattern, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
		{
			logCrit("GenericConnect could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
					pattern, pcreEerror, pcreErrorPos);
			return false;
		}else
		{
			logInfo("Adding %s \n",name);
			PcreContext *ctx = new PcreContext;
			ctx->m_Name = name;
			ctx->m_Pcre = mypcre;
			m_Pcres.push_back(ctx);
		}


		i++;
	}


	return true;
}

bool GenericConnect::Exit()
{
	return true;
}

sch_result GenericConnect::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();

	int32_t output[10 * 3];


	list <PcreContext *>::iterator it;
	uint32_t i;
	for ( it=m_Pcres.begin(), i=0; it != m_Pcres.end();it++,i++ )
	{
		int32_t result=0;
		const char *match;
		if ( (result = pcre_exec((*it)->m_Pcre, 0, (char *) shellcode, len, 0, 0, output, sizeof(output)/sizeof(int32_t))) > 0 )
		{
			uint32_t host = 0, codesizeLen;
			uint16_t port=0;

			codesizeLen = pcre_get_substring((char *) shellcode, output, result, 1, &match);
			if( codesizeLen == 2 )
			{
            	port = (uint32_t)*((uint16_t *)match);
				port = ntohs(port);
			}else
			if( codesizeLen == 4 )
			{
				host = (uint32_t)*((uint32_t *)match);
			}

			pcre_free_substring(match);

			codesizeLen = pcre_get_substring((char *) shellcode, output, result, 2, &match);
			if( codesizeLen == 2 )
			{
				port = (uint32_t)*((uint16_t *)match);
				port = ntohs(port);
			}else
			if( codesizeLen == 4 )
			{
				host = (uint32_t)*((uint32_t *)match);
			}
			pcre_free_substring(match);


			logInfo("Detected connectback shellcode %s, %s:%u  \n",(*it)->m_Name.c_str(), inet_ntoa(*(in_addr *)&host), port);


			Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost(0,host,port,30);
			DialogueFactory *diaf;
			if ((diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL)
			{
				logCrit("%s","No WinNTShell DialogueFactory availible \n");
				return SCH_DONE;
			}
			sock->addDialogue(diaf->createDialogue(sock));
			return SCH_DONE;

		}
	}
	return SCH_NOTHING;
}


