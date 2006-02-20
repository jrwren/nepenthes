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

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_generic_mainz_bind.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

MainzBind::MainzBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "MainzBind";
	m_ShellcodeHandlerDescription = "handles oc192 dcom bindshell";
	m_pcre = NULL;
}

MainzBind::~MainzBind()
{

}

bool MainzBind::Init()
{
	logPF();
	const char *pattern =
	"\\x50\\x50\\x50\\x50\\x6A\\x01\\x6A\\x02\\xFF\\x57\\xEC"
	"\\x8B\\xD8\\xC7\\x07\\x02\\x00(..)\\x33\\xC0\\x89\\x47"
	"\\x04\\x6A\\x10\\x57\\x53\\xFF\\x57\\xF0\\x6A\\x01\\x53"
	"\\xFF\\x57\\xF4\\x50\\x50\\x53\\xFF\\x57\\xF8";

//	logInfo("pcre is %s \n",pattern);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(pattern, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("MainzBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				pattern, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool MainzBind::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result MainzBind::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

//	(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);




	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
//		g_Nepenthes->getUtilities()->hexdump((unsigned char *)shellcode,len);
		const char * match;
		uint16_t port;
        
		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &match);

        port = ntohs(*(uint32_t *) match);
        logInfo("Detected Lsass Mainz listenshell shellcode, :%u \n", port);
		pcre_free_substring(match);

		Socket *socket;
		if ((socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,60,30)) == NULL)
		{
			logCrit("%s","Could not bind socket %u \n",port);
			return SCH_DONE;
		}
		
		DialogueFactory *diaf;
		if ((diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL)
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}

		socket->addDialogueFactory(diaf);
        return SCH_DONE;
	}
	return SCH_NOTHING;
}


