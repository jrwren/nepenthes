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

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_generic_link_trans.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"

#include "DownloadManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

#include <cstring>

using namespace nepenthes;

LinkTrans::LinkTrans(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "LinkTrans";
	m_ShellcodeHandlerDescription = "handles linkbot/linkshellcode connectback transfers";
	m_pcre = NULL;
}

LinkTrans::~LinkTrans()
{

}

bool LinkTrans::Init()
{
	logPF();


	/*
004020b7   53               push ebx
004020b8   53               push ebx
004020b9   68 3e2fd9fe      push fed92f3e                            ; ip   \ sockaddr_in
004020be   68 02003c19      push 193c0002                            ; port /
004020c3   8bd4             mov edx,esp
004020c5   8bd8             mov ebx,eax
004020c7   6a 10            push 10
004020c9   52               push edx
004020ca   53               push ebx
004020cb   ba 6330605a      mov edx,5a603063
004020d0   ffd6             call esi                                 ; connect()
004020d2   50               push eax
004020d3   b4 02            mov ah,2
004020d5   50               push eax
004020d6   55               push ebp
004020d7   53               push ebx
004020d8   ba 005860e2      mov edx,e2605800
004020dd   ffd6             call esi                                 ; recv()
004020df   bf acac0685      mov edi,8506acac                         ; this is the auth key
004020e4   ffe5             jmp ebp                                  ; jump to stage 2
*/


	const char *linkPCRE = ".*\\x53\\x53\\x68(....)\\x68\\x02\\x00(..)\\x8B\\xD4\\x8B\\xD8\\x6A"
	//                                        ^^^^->ip             ^^-> port
		"\\x10\\x52\\x53\\xBA\\x63\\x30\\x60\\x5A\\xFF\\xD6\\x50\\xB4\\x02\\x50\\x55\\x53\\xBA"
		"\\x00\\x58\\x60\\xE2\\xFF\\xD6\\xBF(....)\\xFF\\xE5.*";
	//                                       ^^^^-> auth key

    


//	logInfo("pcre is %s \n", linkPCRE);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(linkPCRE, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("LinkTrans could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				linkPCRE, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool LinkTrans::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result LinkTrans::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 

	if ((matchCount = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0)
	{
		uint16_t netPort, port;
		uint32_t address;
		const char *match;
		unsigned char authKey[4];

		pcre_get_substring((char *)shellcode, (int *)ovec, (int)matchCount, 1, &match);
		memcpy(&address, match, 4);
		pcre_free_substring(match);
		

		pcre_get_substring((char *)shellcode, (int *)ovec, (int)matchCount, 2, &match);
        memcpy(&netPort, match, 2);
		port = ntohs(netPort);
		pcre_free_substring(match);

		pcre_get_substring((char *)shellcode, (int *)ovec, (int)matchCount, 3, &match);
		memcpy(authKey, match, 4);
		pcre_free_substring(match);

		logInfo("Link connectback-shellcode transfer waiting at %s:%d, key 0x%02x%02x%02x%02x.\n",
				inet_ntoa(*(in_addr *)&address), port, authKey[0], authKey[1], authKey[2], authKey[3]);


		char *url;
		unsigned char *base64Key = g_Nepenthes->getUtilities()->b64encode_alloc(authKey,4);

		if (asprintf(&url,"link://%s:%i/%s",inet_ntoa(*(in_addr *)&address),port,base64Key) == -1) {
			logCrit("Memory allocation error\n");
			exit(EXIT_FAILURE);
		}
		g_Nepenthes->getDownloadMgr()->downloadUrl((*msg)->getLocalHost(),url,
			(*msg)->getRemoteHost(),"Connect-Back Transfer (linkbot) Detected in Shellcode",0);
		free(url);
		free(base64Key);

//		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost(0,address,port,30);
//		sock->addDialogue(new LinkDialogue(sock,authKey));

		return SCH_DONE;
	}
	return SCH_NOTHING;
}
