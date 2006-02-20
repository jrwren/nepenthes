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



#include "sch_generic_winexec.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "DialogueFactory.hpp"
#include "Dialogue.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

GenericWinExec::GenericWinExec(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "GenericWinExec";
	m_ShellcodeHandlerDescription = "generic WinExec decoder";
	m_pcre = NULL;
}

GenericWinExec::~GenericWinExec()
{

}

bool GenericWinExec::Init()
{
	const char *createprocesspcre = "\\xE8\\x46\\x00\\x00\\x00\\x8B\\x45\\x3C\\x8B\\x7C\\x05\\x78\\x01\\xEF\\x8B\\x4F\\x18\\x8B\\x5F\\x20\\x01\\xEB\\xE3\\x2E\\x49\\x8B\\x34\\x8B\\x01\\xEE\\x31\\xC0\\x99\\xAC\\x84\\xC0\\x74\\x07\\xC1\\xCA\\x0D\\x01\\xC2\\xEB\\xF4\\x3B\\x54\\x24\\x04\\x75\\xE3\\x8B\\x5F\\x24\\x01\\xEB\\x66\\x8B\\x0C\\x4B\\x8B\\x5F\\x1C\\x01\\xEB\\x8B\\x1C\\x8B\\x01\\xEB\\x89\\x5C\\x24\\x04\\xC3\\x31\\xC0\\x64\\x8B\\x40\\x30\\x85\\xC0\\x78\\x0F\\x8B\\x40\\x0C\\x8B\\x70\\x1C\\xAD\\x8B\\x68\\x08\\xE9\\x0B\\x00\\x00\\x00\\x8B\\x40\\x34\\x05\\x7C\\x00\\x00\\x00\\x8B\\x68\\x3C\\x5F\\x31\\xF6\\x60\\x56\\xEB\\x0D\\x68\\xEF\\xCE\\xE0\\x60\\x68\\x98\\xFE\\x8A\\x0E\\x57\\xFF\\xE7\\xE8\\xEE\\xFF\\xFF\\xFF(.*\\x00)";

	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(createprocesspcre, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("GenericWinExec could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				createprocesspcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool GenericWinExec::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result GenericWinExec::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());

	bool bMatch=false;
	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getSize();
	int32_t piOutput[10 * 3];
	int32_t iResult=0;
	if((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
		const char * pRemoteCommand;

		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 1, &pRemoteCommand);

		logInfo("Detected generic WinExec Shellcode: \"%s\" \n", pRemoteCommand);

        // handle shell session here
		
		if (g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory") == NULL)
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}

		Dialogue *dia = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue((*msg)->getSocket());

		Message *nmsg = new Message((char *)pRemoteCommand, strlen(pRemoteCommand), (*msg)->getLocalPort(), (*msg)->getRemotePort(),
			   (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());
		dia->incomingData(nmsg);
		delete nmsg;
		delete dia;

		
		pcre_free_substring(pRemoteCommand);
		bMatch = true;
	}

	if(bMatch == true)
		return SCH_DONE;
	return SCH_NOTHING;
}


