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

#include "sch_namespace_execute.hpp"

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

NamespaceExecute::NamespaceExecute(sc_shellcode *sc)
{
	m_ShellcodeHandlerName = sc_get_namespace_by_numeric(sc->nspace);
	m_ShellcodeHandlerName += "::";
	m_ShellcodeHandlerName += sc->name;

	m_Shellcode = sc;
}

NamespaceExecute::~NamespaceExecute()
{

}

bool NamespaceExecute::Init()
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

	printf("%s\n",m_Shellcode->pattern);
//	g_Nepenthes->getUtilities()->hexdump((byte *)m_Shellcode->pattern,m_Shellcode->pattern_size);
	return true;
}

bool NamespaceExecute::Exit()
{
	return true;
}

sch_result NamespaceExecute::handleShellcode(Message **msg)
{
	logSpam("%s checking ...\n",m_ShellcodeHandlerName.c_str());

	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 


	if ((matchCount = pcre_exec(m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0)
	{
		 const char * match;


		 pcre_get_substring((char *) shellcode, (int *)ovec, (int)matchCount, 1, &match);

		 logInfo("%s \"%s\" \n",m_ShellcodeHandlerName.c_str(), match);

		 if (g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory") == NULL)
		 {
			 logCrit("%s","No WinNTShell DialogueFactory availible \n");
			 return SCH_DONE;
		 }

		 Dialogue *dia = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue((*msg)->getSocket());

		 Message *nmsg = new Message((char *)match, strlen(match), (*msg)->getLocalPort(), (*msg)->getRemotePort(),
				(*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());
		 dia->incomingData(nmsg);
		 delete nmsg;
		 delete dia;


		 pcre_free_substring(match);

		 

		 return SCH_DONE;
	 }


	return SCH_NOTHING;
}




