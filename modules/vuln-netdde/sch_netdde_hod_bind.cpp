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
#include "sch_netdde_hod_bind.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"

#include "DialogueFactory.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

HODBind::HODBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "HODBind";
	m_ShellcodeHandlerDescription = "handles house of dabus netdde bindshells";
	m_pcre = NULL;
}

HODBind::~HODBind()
{

}

bool HODBind::Init()
{
	logPF();

	const char *thcconnectpcre = //".*(\\xeb\\x70\\x56\\x33\\xc0\\x64\\x8b\\x40\\x30\\x85\\xc0\\x78\\x0c\\x8b\\x40\\x0c.*\\x28\\xff\\x55\\x0c).*";
	"\\xEB\\x70\\x56\\x33\\xC0\\x64\\x8B\\x40\\x30\\x85\\xC0\\x78\\x0C\\x8B\\x40\\x0C"
	"\\x8B\\x70\\x1C\\xAD\\x8B\\x40\\x08\\xEB\\x09\\x8B\\x40\\x34\\x8D\\x40\\x7C\\x8B"
	"\\x40\\x3C\\x5E\\xC3\\x60\\x8B\\x6C\\x24\\x24\\x8B\\x45\\x3C\\x8B\\x54\\x05\\x78"
	"\\x03\\xD5\\x8B\\x4A\\x18\\x8B\\x5A\\x20\\x03\\xDD\\xE3\\x34\\x49\\x8B\\x34\\x8B"
	"\\x03\\xF5\\x33\\xFF\\x33\\xC0\\xFC\\xAC\\x84\\xC0\\x74\\x07\\xC1\\xCF\\x0D\\x03"
	"\\xF8\\xEB\\xF4\\x3B\\x7C\\x24\\x28\\x75\\xE1\\x8B\\x5A\\x24\\x03\\xDD\\x66\\x8B"
	"\\x0C\\x4B\\x8B\\x5A\\x1C\\x03\\xDD\\x8B\\x04\\x8B\\x03\\xC5\\x89\\x44\\x24\\x1C"
	"\\x61\\xC3\\xEB\\x3D\\xAD\\x50\\x52\\xE8\\xA8\\xFF\\xFF\\xFF\\x89\\x07\\x83\\xC4"
	"\\x08\\x83\\xC7\\x04\\x3B\\xF1\\x75\\xEC\\xC3\\x8E\\x4E\\x0E\\xEC\\x72\\xFE\\xB3"
	"\\x16\\x7E\\xD8\\xE2\\x73\\xAD\\xD9\\x05\\xCE\\xD9\\x09\\xF5\\xAD\\xA4\\x1A\\x70"
	"\\xC7\\xA4\\xAD\\x2E\\xE9\\xE5\\x49\\x86\\x49\\xCB\\xED\\xFC\\x3B\\xE7\\x79\\xC6"
	"\\x79\\x83\\xEC\\x60\\x8B\\xEC\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5E"
	"\\xE8\\x3D\\xFF\\xFF\\xFF\\x8B\\xD0\\x83\\xEE\\x36\\x8D\\x7D\\x04\\x8B\\xCE\\x83"
	"\\xC1\\x10\\xE8\\x9D\\xFF\\xFF\\xFF\\x83\\xC1\\x18\\x33\\xC0\\x66\\xB8\\x33\\x32"
	"\\x50\\x68\\x77\\x73\\x32\\x5F\\x8B\\xDC\\x51\\x52\\x53\\xFF\\x55\\x04\\x5A\\x59"
	"\\x8B\\xD0\\xE8\\x7D\\xFF\\xFF\\xFF\\xB8\\x01\\x63\\x6D\\x64\\xC1\\xF8\\x08\\x50"
	"\\x89\\x65\\x34\\x33\\xC0\\x66\\xB8\\x90\\x01\\x2B\\xE0\\x54\\x83\\xC0\\x72\\x50"
	"\\xFF\\x55\\x24\\x33\\xC0\\x50\\x50\\x50\\x50\\x40\\x50\\x40\\x50\\xFF\\x55\\x14"
	"\\x8B\\xF0\\x33\\xC0\\x33\\xDB\\x50\\x50\\x50\\xB8\\x02\\x01(..)\\xFE\\xCC"
	"\\x50\\x8B\\xC4\\xB3\\x10\\x53\\x50\\x56\\xFF\\x55\\x18\\x53\\x56\\xFF\\x55\\x1C"
	"\\x53\\x8B\\xD4\\x2B\\xE3\\x8B\\xCC\\x52\\x51\\x56\\xFF\\x55\\x20\\x8B\\xF0\\x33"
	"\\xC9\\xB1\\x54\\x2B\\xE1\\x8B\\xFC\\x57\\x33\\xC0\\xF3\\xAA\\x5F\\xC6\\x07\\x44"
	"\\xFE\\x47\\x2D\\x57\\x8B\\xC6\\x8D\\x7F\\x38\\xAB\\xAB\\xAB\\x5F\\x33\\xC0\\x8D"
	"\\x77\\x44\\x56\\x57\\x50\\x50\\x50\\x40\\x50\\x48\\x50\\x50\\xFF\\x75\\x34\\x50"
	"\\xFF\\x55\\x08\\xF7\\xD0\\x50\\xFF\\x36\\xFF\\x55\\x10\\xFF\\x77\\x38\\xFF\\x55"
	"\\x28\\xFF\\x55";
//	logInfo("pcre is %s \n",thcconnectpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(thcconnectpcre, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("HODBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				thcconnectpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool HODBind::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}



sch_result HODBind::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
		const char * match;
		uint16_t port;

		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 1, &match);

		port = ntohs(*(uint32_t *) match);
		logInfo("Detected NetDDE HOD listenshell shellcode, :%u \n", port);
		pcre_free_substring(match);

		Socket *socket;
		if ((socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,60,30)) == NULL)
		{
			logCrit("%s","Could not bind socket %u \n",port);
			return SCH_DONE;
		}

		DialogueFactory *diaf;
		if ((diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL)
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}

		socket->addDialogueFactory(diaf);
		return SCH_DONE;

	}
	return SCH_NOTHING;
}

