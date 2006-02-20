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
#include "sch_dcom_sol2k_connect.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"
#include "DialogueFactory.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

SOL2KConnect::SOL2KConnect(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "SOL2KConnect";
	m_ShellcodeHandlerDescription = "handles sol2k dcom connectback";
	m_pcre = NULL;
}

SOL2KConnect::~SOL2KConnect()
{

}

bool SOL2KConnect::Init()
{

	const char *sol2kconnectpcre = "^.*(\\x42\\x65\\x61\\x76\\x75\\x68\\x3a\\x20\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90.*\\xf0\\xed\\xf0\\x95\\x0d\\x0a).*";
	
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(sol2kconnectpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("SOL2KConnect could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				sol2kconnectpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool SOL2KConnect::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result SOL2KConnect::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());

	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
//		(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);
		const char * pCode;
		uint16_t usPort;
		uint32_t ulHost;

		
		uint32_t foo = pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &pCode);

		(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)pCode,foo);
		usPort = htons(* ((uint16_t *) &pCode[441 - 28]) ^0x9595);
		ulHost = htonl(* ((uint32_t *) &pCode[446 - 28]) ^ 0x95959595);
        logInfo("Detected sol2k connectshell shellcode, %s:%u .\n",inet_ntoa(*(in_addr *)&ulHost) , usPort);
		// fixme spawn a shell
		pcre_free_substring(pCode);

		Socket *socket;
		if ((socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,ulHost, usPort,60)) == NULL)
		{
			logCrit("Could not gain socket to connect %s:%i bind socket %u \n",inet_ntoa(*(in_addr *)&ulHost),usPort);
			return SCH_DONE;
		}
		
		DialogueFactory *diaf;
		if ((diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL)
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}

		socket->addDialogue(diaf->createDialogue(socket));

		return SCH_DONE;
	}
	return SCH_NOTHING;
}
