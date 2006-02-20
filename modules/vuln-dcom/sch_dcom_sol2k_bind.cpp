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
#include "sch_dcom_sol2k_bind.hpp"
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

SOL2KBind::SOL2KBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "SOL2KBind";
	m_ShellcodeHandlerDescription = "handles sol2k dcom bindshell";
	m_pcre = NULL;
}

SOL2KBind::~SOL2KBind()
{

}

bool SOL2KBind::Init()
{
	logPF();
	const char *sol2kshellpcre = ".*(\\x42\\x65\\x61\\x76\\x75\\x68\\x3a\\x20\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90.*\\x40\\x64\\xb4\\xd7\\xec\\xcd\\xc2.*\\xe8\\x63\\xc7\\x7f\\xe9\\x1a\\x1f\\x50).*";
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(sol2kshellpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("SOL2KBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				sol2kshellpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool SOL2KBind::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result SOL2KBind::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());

	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
//		(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);
		const char * pCode;
		uint16_t usPort;
		uint32_t ulPort;

		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &pCode);

		ulPort = * ((uint32_t *) &pCode[279]) ^ 0x9432BF80;

		memcpy(&usPort,(char *)&ulPort+1,2);
		usPort = ntohs(usPort);
        logInfo("Detected sol2k listenshell shellcode, :%u \n", usPort);
		// fixme spawn a shell
		pcre_free_substring(pCode);

		Socket *socket;
		if ((socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,usPort,60,30)) == NULL)
		{
			logCrit("%s","Could not bind socket %u \n",usPort);
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

