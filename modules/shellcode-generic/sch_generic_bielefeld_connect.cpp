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
#include "sch_generic_bielefeld_connect.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"

#include "Dialogue.hpp"
#include "DialogueFactory.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

BieleFeldConnect::BieleFeldConnect(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "BieleFeldConnect";
	m_ShellcodeHandlerDescription = "handles oc192 dcom bindshell";
	m_pcre = NULL;
}

BieleFeldConnect::~BieleFeldConnect()
{

}

bool BieleFeldConnect::Init()
{
	logPF();

	const char *pattern =
	"\\xc7\\x02\\x63\\x6d\\x64\\x00\\x52\\x50\\xff\\x57\\xe8"
	"\\xc7\\x07\\x02\\x00(..)\\xc7\\x47\\x04(....)\\x6a\\x10"
	"\\x57\\x53\\xff\\x57\\xf8\\x53\\xff\\x57\\xfc\\x50\\xff"
	"\\x57\\xec";

	logInfo("pcre is %s \n",pattern);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(pattern, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("BieleFeldConnect could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				pattern, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool BieleFeldConnect::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result BieleFeldConnect::handleShellcode(Message **msg)
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
        uint32_t host;


		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &match);
		port = *(uint16_t *) match;
		port = ntohs(port);
        pcre_free_substring(match);

		pcre_get_substring((char *) shellcode, piOutput, iResult, 2, &match);
		host = * ((uint32_t *) match);
		pcre_free_substring(match);

		logInfo("Detected Lsass HoD connectback shellcode, %s:%u  \n", inet_ntoa(*(in_addr *)&host), port);


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
	return SCH_NOTHING;
}

