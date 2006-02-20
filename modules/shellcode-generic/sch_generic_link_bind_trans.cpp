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
#include "sch_generic_link_bind_trans.hpp"
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

using namespace nepenthes;

LinkBindTrans::LinkBindTrans(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "LinkBindTrans";
	m_ShellcodeHandlerDescription = "handles linkbot/linkshellcode bind transfers";
	m_pcre = NULL;
}

LinkBindTrans::~LinkBindTrans()
{

}

bool LinkBindTrans::Init()
{
	logPF();


	/*
		00402111   ba 83538300      mov edx,835383
		00402116   ffd6             call esi                                 ; socket()
		00402118   53               push ebx
		00402119   53               push ebx
		0040211a   53               push ebx
		0040211b   68 0200d63a      push 3ad60002                            ; 3ad6 <- port
		00402120   8bd4             mov edx,esp
		00402122   8bd8             mov ebx,eax
		00402124   6a 10            push 10
		00402126   52               push edx
		00402127   53               push ebx
		00402128   ba 0090a6c2      mov edx,c2a69000
		0040212d   ffd6             call esi                                 ; bind()
		0040212f   40               inc eax
		00402130   50               push eax
		00402131   53               push ebx
		00402132   ba 7a3b73a1      mov edx,a1733b7a
		00402137   ffd6             call esi                                 ; listen()
		00402139   50               push eax
		0040213a   50               push eax
		0040213b   53               push ebx
		0040213c   ba 10d36900      mov edx,69d310
		00402141   ffd6             call esi                                 ; accept()
		00402143   8bd8             mov ebx,eax
		00402145   33c0             xor eax,eax
		00402147   50               push eax
		00402148   b4 02            mov ah,2
		0040214a   50               push eax
		0040214b   55               push ebp
		0040214c   53               push ebx
		0040214d   ba 005860e2      mov edx,e2605800
		00402152   ffd6             call esi                                 ; recv()
		00402154   bf 1cf174c0      mov edi,c074f11c                         ; authentication key
		00402159   ffe5             jmp ebp
	*/
	const char *pcre =
		"\\xba\\x83\\x53\\x83\\x00\\xff\\xd6\\x53\\x53\\x53\\x68\\x02\\x00"
		"(..)\\x8b\\xd4\\x8b\\xd8\\x6a\\x10\\x52\\x53\\xba\\x00\\x90"
		"\\xa6\\xc2\\xff\\xd6\\x40\\x50\\x53\\xba\\x7a\\x3b\\x73\\xa1\\xff"
		"\\xd6\\x50\\x50\\x53\\xba\\x10\\xd3\\x69\\x00\\xff\\xd6\\x8b\\xd8"
		"\\x33\\xc0\\x50\\xb4\\x02\\x50\\x55\\x53\\xba\\x00\\x58\\x60\\xe2"
		"\\xff\\xd6\\xbf(....)\\xff\\xe5";

	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(pcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("LinkTrans could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				pcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool LinkBindTrans::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result LinkBindTrans::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();

	int32_t ovec[10 * 3];
	int32_t matchCount; 

	if ((matchCount = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, ovec, sizeof(ovec)/sizeof(int32_t))) > 0)
	{
		uint16_t netPort, port;
		const char *match;
		unsigned char authKey[4];

		pcre_get_substring((char *)shellcode, ovec, matchCount, 1, &match);
        memcpy(&netPort, match, 2);
		port = ntohs(netPort);
		pcre_free_substring(match);

		pcre_get_substring((char *)shellcode, ovec, matchCount, 2, &match);
		memcpy(authKey, match, 4);
		pcre_free_substring(match);

		logInfo("Link bind-shellcode transfer requires port %d, key 0x%02x%02x%02x%02x.\n",
				port, authKey[0], authKey[1], authKey[2], authKey[3]);


		char *url;
		unsigned char *base64Key = g_Nepenthes->getUtilities()->b64encode_alloc(authKey,4);

		uint32_t remoteHost = (*msg)->getRemoteHost();
		asprintf(&url,"blink://%s:%i/%s",inet_ntoa(*(in_addr *)&remoteHost),port,base64Key);
		g_Nepenthes->getDownloadMgr()->downloadUrl(url,(*msg)->getRemoteHost(),url);
		free(url);
		free(base64Key);

//		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost(0,address,port,30);
//		sock->addDialogue(new LinkDialogue(sock,authKey));

		return SCH_DONE;
	}
	return SCH_NOTHING;
}
