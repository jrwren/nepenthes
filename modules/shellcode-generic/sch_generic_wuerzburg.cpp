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
#include "sch_generic_wuerzburg.hpp"
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

Wuerzburg::Wuerzburg(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "Wuerzburg";
	m_ShellcodeHandlerDescription = "handles \"wuerzburg\" shellcode";
	m_wuerzburgPattern = NULL;
}

Wuerzburg::~Wuerzburg()
{

}

bool Wuerzburg::Init()
{
	logPF();

	/*
		0040200c   eb 27            jmp short wuerzbur.00402035
		0040200e   90               nop
		0040200f   90               nop
		00402010   90               nop
		00402011   90               nop
		00402012   90               nop
		00402013   90               nop
		00402014   5d               pop ebp
		00402015   33c9             xor ecx,ecx
		00402017   66:b9 2502       mov cx,225
		0040201b   8d75 05          lea esi,dword ptr ss:[ebp+5]
		0040201e   8bfe             mov edi,esi
		00402020   8a06             mov al,byte ptr ds:[esi]
		00402022   3c 99            cmp al,99
		00402024   75 05            jnz short wuerzbur.0040202b
		00402026   46               inc esi
		00402027   8a06             mov al,byte ptr ds:[esi]
		00402029   2c 30            sub al,30
		0040202b   46               inc esi
		0040202c   34 99            xor al,99
		0040202e   8807             mov byte ptr ds:[edi],al
		00402030   47               inc edi
		00402031  ^e2 ed            loopd short wuerzbur.00402020
		00402033   eb 0a            jmp short wuerzbur.0040203f
		00402035   e8 daffffff      call wuerzbur.00402014
	*/
	const char *wuerzburgPattern =
		"\\xEB\\x27(..)(....)\\x5D\\x33\\xC9\\x66\\xB9..\\x8D"
		"\\x75\\x05\\x8B\\xFE\\x8A\\x06\\x3C.\\x75\\x05"
		"\\x46\\x8A\\x06\\x2C.\\x46\\x34.\\x88\\x07"
		"\\x47\\xE2\\xED\\xEB\\x0A\\xE8\\xDA\\xFF\\xFF\\xFF";

	const char *pcreEerror;
	int32_t pcreErrorPos;
	if((m_wuerzburgPattern = pcre_compile(wuerzburgPattern, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("Stuttgart could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				m_wuerzburgPattern, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool Wuerzburg::Exit()
{
	if(m_wuerzburgPattern != NULL)
    	free(m_wuerzburgPattern);
	return true;

}

sch_result Wuerzburg::handleShellcode(Message **msg)
{
	logPF();
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 

	if ((matchCount = pcre_exec(m_wuerzburgPattern, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0)
	{
		uint16_t netPort, port;
		uint32_t address;
		const char *match;

		pcre_get_substring((char *)shellcode, (int *)ovec, (int)matchCount, 1, &match);
		memcpy(&netPort, match, 2);
		port = ntohs(netPort);
		pcre_free_substring(match);

		pcre_get_substring((char *)shellcode, (int *)ovec, (int)matchCount, 2, &match);
		memcpy(&address, match, 4);
		pcre_free_substring(match);

		address ^= 0xaaaaaaaa;
		
		logInfo("Wuerzburg transfer waiting at %s:%d.\n",
				inet_ntoa(*(in_addr *)&address), port);

		char *url;

		if (asprintf(&url,"csend://%s:%d",inet_ntoa(*(in_addr *)&address), port) == -1) {
			logCrit("Memory allocation error\n");
			exit(EXIT_FAILURE);
		}
		g_Nepenthes->getDownloadMgr()->downloadUrl((*msg)->getLocalHost(),url, (*msg)->getRemoteHost(), url,0);
		free(url);

//		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost(0,address,port,30);
//		sock->addDialogue(new LinkDialogue(sock,authKey));

		return SCH_DONE;
	}
	return SCH_NOTHING;
}
