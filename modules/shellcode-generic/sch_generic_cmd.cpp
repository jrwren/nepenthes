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



#include "sch_generic_cmd.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "DialogueFactory.hpp"
#include "Dialogue.hpp"

#include "Utilities.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

GenericCMD::GenericCMD(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "GenericCMD";
	m_ShellcodeHandlerDescription = "generic CMD decoder";
	m_pcre = NULL;
}

GenericCMD::~GenericCMD()
{

}

bool GenericCMD::Init()
{
	const char *createprocesspcre = ".*(cmd.* /.*\\x00).*";
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(createprocesspcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("GenericCMD could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				createprocesspcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool GenericCMD::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result GenericCMD::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());

	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getSize();
	int32_t piOutput[10 * 3];
	int32_t iResult=0;
	if((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
//		logDebug("GenricCMD (improve pcre debug) (%i bytes)\n",(*msg)->getSize());
//		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *)(*msg)->getMsg(),(*msg)->getSize());
 
		const char * pRemoteCommand;

		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &pRemoteCommand);

		logInfo("Detected generic CMD Shellcode: \"%s\" \n", pRemoteCommand);

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
		
		return SCH_DONE;
	}
	return SCH_NOTHING;
}


