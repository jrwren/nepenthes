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


/* Additional notes:
 *
 * The pcre's and processing logic in this module is derived from mwcollect written by Georg Wicherski
 *
 * if you got any idea what has to be done to relicense bsd code on a gpl license mail us
 * wikipedia states bsd code can be relicensed on to gpl, but we got no information what has to be done
 * 
 *
 */

/* $Id$ */



#include "sch_generic_createprocess.hpp"

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

GenericCreateProcess::GenericCreateProcess(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "GenericCreateProcess";
	m_ShellcodeHandlerDescription = "generic CreateProcess decoder";
	m_pcre = NULL;

	fprintf(stderr,"\n");
	fprintf(stderr,"The generic createprocess shellcodehandler is based on \n");
	fprintf(stderr,"mwcollects generic createprocess shellcodehandler \n");
	fprintf(stderr,"mwcollect is\n"); 
	fprintf(stderr,"Copyright (c) 2005, Honeynet Project\n");
    fprintf(stderr,"All rights reserved.\n");
	fprintf(stderr,"published on a bsd license\n");
	fprintf(stderr,"and written by Georg Wicherski\n");
	fprintf(stderr,"http://www.mwcollect.org for more information about mwcollect\n");
	fprintf(stderr,"\n");		

}

GenericCreateProcess::~GenericCreateProcess()
{

}

bool GenericCreateProcess::Init()
{
	const char *createprocesspcre = "^.*\\x0A\\x65\\x73\\x73.*\\x57\\xE8....(.*)\\x6A.\\xE8....+$";
	const char * pcreEerror;
	int pcreErrorPos;
	if((m_pcre = pcre_compile(createprocesspcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("GenericCreateProcess could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				createprocesspcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool GenericCreateProcess::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result GenericCreateProcess::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());

	bool bMatch=false;
	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	unsigned int len = (*msg)->getMsgLen();
	int piOutput[10 * 3];
	int iResult=0;
	if((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int))) > 0)
	{
		const char * pRemoteCommand;

		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &pRemoteCommand);

		logInfo("Detected generic CreateProcess Shellcode: \"%s\" \n", pRemoteCommand);

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


