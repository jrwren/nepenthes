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
#include "sch_generic_stuttgart.hpp"
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

Stuttgart::Stuttgart(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "Stuttgart";
	m_ShellcodeHandlerDescription = "handles \"stuttgart\" shellcode";
	m_stuttgartPattern = NULL;
}

Stuttgart::~Stuttgart()
{

}

bool Stuttgart::Init()
{
	logPF();

	/*
		004020f3   50               push eax
		004020f4   50               push eax
		004020f5   68 54a654c2      push c254a654                            ; ip
		004020fa   68 0200c50d      push 0dc50002                            ; port
		004020ff   8bfc             mov edi,esp
		00402101   50               push eax
		00402102   6a 01            push 1
		00402104   6a 02            push 2
		00402106   ff55 20          call dword ptr ss:[ebp+20]               ; socket
		00402109   8bd8             mov ebx,eax
		0040210b   6a 10            push 10
		0040210d   57               push edi
		0040210e   53               push ebx
		0040210f   ff55 24          call dword ptr ss:[ebp+24]               ; connect
		00402112   85c0             test eax,eax
		00402114   75 59            jnz short stuttgar.0040216f
		00402116   c745 00 03000000 mov dword ptr ss:[ebp],3                 ; key
		0040211d   50               push eax
		0040211e   6a 04            push 4
		00402120   55               push ebp
		00402121   53               push ebx
		00402122   ff55 2c          call dword ptr ss:[ebp+2c]               ; send
	*/
	const char *stuttgartPattern =
		"\\x50\\x50\\x68(....)\\x68\\x02\\x00"
		"(..)\\x8B\\xFC\\x50\\x6A\\x01\\x6A\\x02\\xFF"
		"\\x55\\x20\\x8B\\xD8\\x6A\\x10\\x57\\x53\\xFF\\x55"
		"\\x24\\x85\\xC0\\x75\\x59\\xC7\\x45\\x00(....)"
		"\\x50\\x6A\\x04\\x55\\x53\\xFF\\x55\\x2C";

	const char *pcreEerror;
	int32_t pcreErrorPos;
	if((m_stuttgartPattern = pcre_compile(stuttgartPattern, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("Stuttgart could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				stuttgartPattern, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool Stuttgart::Exit()
{
	if(m_stuttgartPattern != NULL)
    	free(m_stuttgartPattern);
	return true;

}

sch_result Stuttgart::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 

	if ((matchCount = pcre_exec(m_stuttgartPattern, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0)
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

		logInfo("Link (from stuttgart-shellcode) transfer waiting at %s:%d, key 0x%02x%02x%02x%02x.\n",
				inet_ntoa(*(in_addr *)&address), port, authKey[0], authKey[1], authKey[2], authKey[3]);


		char *url;
		unsigned char *base64Key = g_Nepenthes->getUtilities()->b64encode_alloc(authKey,4);

		asprintf(&url,"link://%s:%i/%s",inet_ntoa(*(in_addr *)&address),port,base64Key);
		g_Nepenthes->getDownloadMgr()->downloadUrl((*msg)->getLocalHost(),url,(*msg)->getRemoteHost(),url,0);
		free(url);
		free(base64Key);

//		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost(0,address,port,30);
//		sock->addDialogue(new LinkDialogue(sock,authKey));

		return SCH_DONE;
	}
	return SCH_NOTHING;
}
