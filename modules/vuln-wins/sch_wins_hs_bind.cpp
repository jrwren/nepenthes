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

#include <arpa/inet.h>
#include <netinet/in.h>

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_wins_hs_bind.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "SocketManager.hpp"
#include "DialogueFactoryManager.hpp"


using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr



HATSQUADBind::HATSQUADBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "HATSQUADBind";
	m_ShellcodeHandlerDescription = "handles hat-squad wins bindshell";
	m_pcre = NULL;
}

HATSQUADBind::~HATSQUADBind()
{

}

bool HATSQUADBind::Init()
{
	logPF();
	const char hatsquadbindpcre[] = ".*("
									"\\x33\\xC9\\x83\\xE9"
									"\\xAF\\xD9\\xEE\\xD9\\x74\\x24\\xF4\\x5B\\x81\\x73\\x13\\xBB"
									"\\x1E\\xD3\\x6A\\x83\\xEB\\xFC\\xE2\\xF4\\x47\\x74\\x38\\x25\\x53\\xE7\\x2C\\x95"
									"\\x44\\x7E\\x58\\x06\\x9F\\x3A\\x58\\x2F\\x87\\x95\\xAF\\x6F\\xC3\\x1F\\x3C\\xE1"
									"\\xF4\\x06\\x58\\x35\\x9B\\x1F\\x38\\x89\\x8B\\x57\\x58\\x5E\\x30\\x1F\\x3D\\x5B"
									"\\x7B\\x87\\x7F\\xEE\\x7B\\x6A\\xD4\\xAB\\x71\\x13\\xD2\\xA8\\x50\\xEA\\xE8\\x3E"
									"\\x9F\\x36\\xA6\\x89\\x30\\x41\\xF7\\x6B\\x50\\x78\\x58\\x66\\xF0\\x95\\x8C\\x76"
									"\\xBA\\xF5\\xD0\\x46\\x30\\x97\\xBF\\x4E\\xA7\\x7F\\x10\\x5B\\x7B\\x7A\\x58\\x2A"
									"\\x8B\\x95\\x93\\x66\\x30\\x6E\\xCF\\xC7\\x30\\x5E\\xDB\\x34\\xD3\\x90\\x9D\\x64"
									"\\x57\\x4E\\x2C\\xBC\\x8A\\xC5\\xB5\\x39\\xDD\\x76\\xE0\\x58\\xD3\\x69\\xA0\\x58"
									"\\xE4\\x4A\\x2C\\xBA\\xD3\\xD5\\x3E\\x96\\x80\\x4E\\x2C\\xBC\\xE4\\x97\\x36\\x0C"
									"\\x3A\\xF3\\xDB\\x68\\xEE\\x74\\xD1\\x95\\x6B\\x76\\x0A\\x63\\x4E\\xB3\\x84\\x95"
									"\\x6D\\x4D\\x80\\x39\\xE8\\x4D\\x90\\x39\\xF8\\x4D\\x2C\\xBA\\xDD\\x76\\xD3\\x0F"
									"\\xDD\\x4D\\x5A\\x8B\\x2E\\x76\\x77\\x70\\xCB\\xD9\\x84\\x95\\x6D\\x74\\xC3\\x3B"
									"\\xEE\\xE1\\x03\\x02\\x1F\\xB3\\xFD\\x83\\xEC\\xE1\\x05\\x39\\xEE\\xE1\\x03\\x02"
									"\\x5E\\x57\\x55\\x23\\xEC\\xE1\\x05\\x3A\\xEF\\x4A\\x86\\x95\\x6B\\x8D\\xBB\\x8D"
									"\\xC2\\xD8\\xAA\\x3D\\x44\\xC8\\x86\\x95\\x6B\\x78\\xB9\\x0E\\xDD\\x76\\xB0\\x07"
									"\\x32\\xFB\\xB9\\x3A\\xE2\\x37\\x1F\\xE3\\x5C\\x74\\x97\\xE3\\x59\\x2F\\x13\\x99"
									"\\x11\\xE0\\x91\\x47\\x45\\x5C\\xFF\\xF9\\x36\\x64\\xEB\\xC1\\x10\\xB5\\xBB\\x18"
									"\\x45\\xAD\\xC5\\x95\\xCE\\x5A\\x2C\\xBC\\xE0\\x49\\x81\\x3B\\xEA\\x4F\\xB9\\x6B"
									"\\xEA\\x4F\\x86\\x3B\\x44\\xCE\\xBB\\xC7\\x62\\x1B\\x1D\\x39\\x44\\xC8\\xB9\\x95"
									"\\x44\\x29\\x2C\\xBA\\x30\\x49\\x2F\\xE9\\x7F\\x7A\\x2C\\xBC\\xE9\\xE1\\x03\\x02"
									"\\x54\\xD0\\x33\\x0A\\xE8\\xE1\\x05\\x95\\x6B\\x1E\\xD3\\x6A"
									").*";
//	logInfo("pcre is %s \n",hatsquadbindpcre);
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(hatsquadbindpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("HATSQUADBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				hatsquadbindpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool HATSQUADBind::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result HATSQUADBind::handleShellcode(Message **msg)
{
	logPF();
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

//	(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
        logInfo("%s","Detected hat-squad (static) bind shellcode :101 \n");

		Socket *socket;
		if ((socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,101,60,30)) == NULL)
		{
			logCrit("Could not bind socket %u \n",101);
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
