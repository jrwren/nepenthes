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
#include <netinet/in.h>
#include <arpa/inet.h>

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_lsass_hod_connect.hpp"
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

HODConnect::HODConnect(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "HODConnect";
	m_ShellcodeHandlerDescription = "handles oc192 dcom bindshell";
	m_pcre = NULL;
}

HODConnect::~HODConnect()
{

}

bool HODConnect::Init()
{
	logPF();

	const char *oc192bindpcre = //".*(\\xEB\\x10\\x5B\\x4B\\x33\\xC9\\x66\\xB9\\x25\\x01\\x80\\x34\\x0B\\x99\\xE2\\xFA\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF\\x70\\x62\\x99\\x99\\x99\\xC6\\xFD.*\\xF9\\x7E\\xE0\\x5F\\xE0).*";

	"\\xEB\\x10\\x5B\\x4B\\x33\\xC9\\x66\\xB9\\x25\\x01\\x80\\x34\\x0B\\x99\\xE2\\xFA"
	"\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF\\x70\\x62\\x99\\x99\\x99\\xC6\\xFD\\x38\\xA9"
	"\\x99\\x99\\x99\\x12\\xD9\\x95\\x12\\xE9\\x85\\x34\\x12\\xF1\\x91\\x12\\x6E\\xF3"
	"\\x9D\\xC0\\x71\\x02\\x99\\x99\\x99\\x7B\\x60\\xF1\\xAA\\xAB\\x99\\x99\\xF1\\xEE"
	"\\xEA\\xAB\\xC6\\xCD\\x66\\x8F\\x12\\x71\\xF3\\x9D\\xC0\\x71\\x1B\\x99\\x99\\x99"
	"\\x7B\\x60\\x18\\x75\\x09\\x98\\x99\\x99\\xCD\\xF1\\x98\\x98\\x99\\x99\\x66\\xCF"
	"\\x89\\xC9\\xC9\\xC9\\xC9\\xD9\\xC9\\xD9\\xC9\\x66\\xCF\\x8D\\x12\\x41\\xF1(."
	"...)\\xF1\\x9B\\x99(..)\\x12\\x55\\xF3\\x89\\xC8\\xCA\\x66\\xCF"
	"\\x81\\x1C\\x59\\xEC\\xD3\\xF1\\xFA\\xF4\\xFD\\x99\\x10\\xFF\\xA9\\x1A\\x75\\xCD"
	"\\x14\\xA5\\xBD\\xF3\\x8C\\xC0\\x32\\x7B\\x64\\x5F\\xDD\\xBD\\x89\\xDD\\x67\\xDD"
	"\\xBD\\xA4\\x10\\xC5\\xBD\\xD1\\x10\\xC5\\xBD\\xD5\\x10\\xC5\\xBD\\xC9\\x14\\xDD"
	"\\xBD\\x89\\xCD\\xC9\\xC8\\xC8\\xC8\\xF3\\x98\\xC8\\xC8\\x66\\xEF\\xA9\\xC8\\x66"
	"\\xCF\\x9D\\x12\\x55\\xF3\\x66\\x66\\xA8\\x66\\xCF\\x91\\xCA\\x66\\xCF\\x85\\x66"
	"\\xCF\\x95\\xC8\\xCF\\x12\\xDC\\xA5\\x12\\xCD\\xB1\\xE1\\x9A\\x4C\\xCB\\x12\\xEB"
	"\\xB9\\x9A\\x6C\\xAA\\x50\\xD0\\xD8\\x34\\x9A\\x5C\\xAA\\x42\\x96\\x27\\x89\\xA3"
	"\\x4F\\xED\\x91\\x58\\x52\\x94\\x9A\\x43\\xD9\\x72\\x68\\xA2\\x86\\xEC\\x7E\\xC3"
	"\\x12\\xC3\\xBD\\x9A\\x44\\xFF\\x12\\x95\\xD2\\x12\\xC3\\x85\\x9A\\x44\\x12\\x9D"
	"\\x12\\x9A\\x5C\\x32\\xC7\\xC0\\x5A\\x71\\x99\\x66\\x66\\x66\\x17\\xD7\\x97\\x75"
	"\\xEB\\x67\\x2A\\x8F\\x34\\x40\\x9C\\x57\\x76\\x57\\x79\\xF9\\x52\\x74\\x65\\xA2"
	"\\x40\\x90\\x6C\\x34\\x75\\x60\\x33\\xF9\\x7E\\xE0\\x5F\\xE0";


//	logInfo("pcre is %s \n",oc192bindpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(oc192bindpcre, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("HODConnect could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				oc192bindpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool HODConnect::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result HODConnect::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

//	(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);




	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
//		g_Nepenthes->getUtilities()->hexdump((unsigned char *)shellcode,len);
		const char * match;
		uint16_t port;
        uint32_t host;

		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 1, &match);
        host = * ((uint32_t *) match) ^ 0x99999999;
		pcre_free_substring(match);

		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 2, &match);
		port = *(uint16_t *) match;
		port = ntohs(port);
		port = port^0x9999;
		pcre_free_substring(match);

		logInfo("Detected Lsass HoD connectback shellcode, %s:%u  \n", inet_ntoa(*(in_addr *)&host), port);


		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost((*msg)->getLocalHost(),host,port,30);
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

