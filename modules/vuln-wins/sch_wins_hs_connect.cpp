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
 
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "vuln-wins.hpp"
#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_wins_hs_connect.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "DialogueFactory.hpp"
#include "SocketManager.hpp"


using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr


HATSQUADConnect::HATSQUADConnect(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "HATSQUADConnect";
	m_ShellcodeHandlerDescription = "handles hat-squad wins connect";
	m_pcre = NULL;
}

HATSQUADConnect::~HATSQUADConnect()
{

}

bool HATSQUADConnect::Init()
{
	logPF();
	const char *hatsquadbindpcre = // ".*(\\xFC\\x6A\\xEB\\x52\\xE8\\xF9\\xFF\\xFF\\xFF\\x60\\x8B\\x6C\\x24\\x24\\x8B\\x45\\x3C\\x8B\\x7C\\x05\\x78\\x01\\xEF.*)";
	"\\xFC\\x6A\\xEB\\x52\\xE8\\xF9\\xFF\\xFF\\xFF\\x60\\x8B\\x6C\\x24\\x24\\x8B\\x45"
	"\\x3C\\x8B\\x7C\\x05\\x78\\x01\\xEF\\x83\\xC7\\x01\\x8B\\x4F\\x17\\x8B\\x5F\\x1F"
	"\\x01\\xEB\\xE3\\x30\\x49\\x8B\\x34\\x8B\\x01\\xEE\\x31\\xC0\\x99\\xAC\\x84\\xC0"
	"\\x74\\x07\\xC1\\xCA\\x0D\\x01\\xC2\\xEB\\xF4\\x3B\\x54\\x24\\x28\\x75\\xE3\\x8B"
	"\\x5F\\x23\\x01\\xEB\\x66\\x8B\\x0C\\x4B\\x8B\\x5F\\x1B\\x01\\xEB\\x03\\x2C\\x8B"
	"\\x89\\x6C\\x24\\x1C\\x61\\xC3\\x31\\xC0\\x64\\x8B\\x40\\x30\\x8B\\x40\\x0C\\x8B"
	"\\x70\\x1C\\xAD\\x8B\\x40\\x08\\x5E\\x68\\x8E\\x4E\\x0E\\xEC\\x50\\xFF\\xD6\\x31"
	"\\xDB\\x66\\x53\\x66\\x68\\x33\\x32\\x68\\x77\\x73\\x32\\x5F\\x54\\xFF\\xD0\\x68"
	"\\xCB\\xED\\xFC\\x3B\\x50\\xFF\\xD6\\x5F\\x89\\xE5\\x66\\x81\\xED\\x08\\x02\\x55"
	"\\x6A\\x02\\xFF\\xD0\\x68\\xD9\\x09\\xF5\\xAD\\x57\\xFF\\xD6\\x53\\x53\\x53\\x53"
	"\\x43\\x53\\x43\\x53\\xFF\\xD0\\x68(....)\\x66\\x68(..)\\x66"
	"\\x53\\x89\\xE1\\x95\\x68\\xEC\\xF9\\xAA\\x60\\x57\\xFF\\xD6\\x6A\\x10\\x51\\x55"
	"\\xFF\\xD0\\x66\\x6A\\x64\\x66\\x68\\x63\\x6D\\x6A\\x50\\x59\\x29\\xCC\\x89\\xE7"
	"\\x6A\\x44\\x89\\xE2\\x31\\xC0\\xF3\\xAA\\x95\\x89\\xFD\\xFE\\x42\\x2D\\xFE\\x42"
	"\\x2C\\x8D\\x7A\\x38\\xAB\\xAB\\xAB\\x68\\x72\\xFE\\xB3\\x16\\xFF\\x75\\x28\\xFF"
	"\\xD6\\x5B\\x57\\x52\\x51\\x51\\x51\\x6A\\x01\\x51\\x51\\x55\\x51\\xFF\\xD0\\x68"
	"\\xAD\\xD9\\x05\\xCE\\x53\\xFF\\xD6\\x6A\\xFF\\xFF\\x37\\xFF\\xD0\\x68\\xE7\\x79"
	"\\xC6\\x79\\xFF\\x75\\x04\\xFF\\xD6\\xFF\\x77\\xFC\\xFF\\xD0\\x68\\xEF\\xCE\\xE0"
	"\\x60\\x53\\xFF\\xD6\\xFF\\xD0";

//	logInfo("pcre is %s \n",hatsquadbindpcre);
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(hatsquadbindpcre, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("HATSQUADConnect could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				hatsquadbindpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool HATSQUADConnect::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result HATSQUADConnect::handleShellcode(Message **msg)
{
	logPF();
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

//	(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
		const char * match;
		uint16_t port;
		uint32_t host;

		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 1, &match);
		host = * ((uint32_t *) match);
		pcre_free_substring(match);


		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 2, &match);
		port = *(uint16_t *) match;
		port = ntohs(port);
		pcre_free_substring(match);




		logInfo("Detected hat-squad connectback shellcode, %s:%i\n",inet_ntoa(*(in_addr *)&host),port);

		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost((*msg)->getLocalHost(),host,port,30);
		DialogueFactory *diaf;
		if ( (diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL )
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}
		sock->addDialogue(diaf->createDialogue(sock));


		

		return SCH_DONE;
	}
	return SCH_NOTHING;
	
}
