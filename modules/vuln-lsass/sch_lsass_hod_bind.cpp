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

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_lsass_hod_bind.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

HODBind::HODBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "HODBind";
	m_ShellcodeHandlerDescription = "handles oc192 dcom bindshell";
	m_pcre = NULL;
}

HODBind::~HODBind()
{

}

bool HODBind::Init()
{
	logPF();
//	const char *oc192bindpcre = ".*(\\x46\\x00\\x58\\x00\\x4E\\x00\\x42\\x00\\x46\\x00\\x58\\x00\\x46\\x00\\x58\\x00\\x4E\\x00\\x42\\x00\\x46\\x00\\x58\\x00\\x46\\x00\\x58\\x00\\x46\\x00\\x58\\x00\\x46\\x00\\x58\\x00.*\\x6a\\x6d\\xca\\xdd\\xe4\\xf0\\x90\\x80\\x2f\\xa2\\x04).*";
	const char *oc192bindpcre = //".*(\\xEB\\x10\\x5A\\x4A\\x33\\xC9\\x66\\xB9\\x7D\\x01\\x80\\x34\\x0A\\x99\\xE2\\xFA\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF\\x70\\x95\\x98\\x99\\x99\\xC3\\xFD.*\\x99\\xFA\\xF5\\xF6\\xEA\\xFC\\xEA\\xF6\\xFA\\xF2\\xFC\\xED\\x99).*";
//	const char *oc192bindpcre = ".*(\\xEB\\00\\x10\\00\\x5A\\00\\x4A\\x33\\00\\xEA\\00\\xF6\\00.*)";
//																									fa 00 fa 00  fc 00 e9 00 ed 00 99
	"\\xEB\\x10\\x5A\\x4A\\x33\\xC9\\x66\\xB9\\x7D\\x01\\x80\\x34\\x0A\\x99\\xE2\\xFA"
	"\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF\\x70\\x95\\x98\\x99\\x99\\xC3\\xFD\\x38\\xA9"
	"\\x99\\x99\\x99\\x12\\xD9\\x95\\x12\\xE9\\x85\\x34\\x12\\xD9\\x91\\x12\\x41\\x12"
	"\\xEA\\xA5\\x12\\xED\\x87\\xE1\\x9A\\x6A\\x12\\xE7\\xB9\\x9A\\x62\\x12\\xD7\\x8D"
	"\\xAA\\x74\\xCF\\xCE\\xC8\\x12\\xA6\\x9A\\x62\\x12\\x6B\\xF3\\x97\\xC0\\x6A\\x3F"
	"\\xED\\x91\\xC0\\xC6\\x1A\\x5E\\x9D\\xDC\\x7B\\x70\\xC0\\xC6\\xC7\\x12\\x54\\x12"
	"\\xDF\\xBD\\x9A\\x5A\\x48\\x78\\x9A\\x58\\xAA\\x50\\xFF\\x12\\x91\\x12\\xDF\\x85"
	"\\x9A\\x5A\\x58\\x78\\x9B\\x9A\\x58\\x12\\x99\\x9A\\x5A\\x12\\x63\\x12\\x6E\\x1A"
	"\\x5F\\x97\\x12\\x49\\xF3\\x9A\\xC0\\x71\\x1E\\x99\\x99\\x99\\x1A\\x5F\\x94\\xCB"
	"\\xCF\\x66\\xCE\\x65\\xC3\\x12\\x41\\xF3\\x9C\\xC0\\x71\\xED\\x99\\x99\\x99\\xC9"
	"\\xC9\\xC9\\xC9\\xF3\\x98\\xF3\\x9B\\x66\\xCE\\x75\\x12\\x41\\x5E\\x9E\\x9B\\x99"
	"(..)\\xAA\\x59\\x10\\xDE\\x9D\\xF3\\x89\\xCE\\xCA\\x66\\xCE\\x69\\xF3\\x98"
	"\\xCA\\x66\\xCE\\x6D\\xC9\\xC9\\xCA\\x66\\xCE\\x61\\x12\\x49\\x1A\\x75\\xDD\\x12"
	"\\x6D\\xAA\\x59\\xF3\\x89\\xC0\\x10\\x9D\\x17\\x7B\\x62\\x10\\xCF\\xA1\\x10\\xCF"
	"\\xA5\\x10\\xCF\\xD9\\xFF\\x5E\\xDF\\xB5\\x98\\x98\\x14\\xDE\\x89\\xC9\\xCF\\xAA"
	"\\x50\\xC8\\xC8\\xC8\\xF3\\x98\\xC8\\xC8\\x5E\\xDE\\xA5\\xFA\\xF4\\xFD\\x99\\x14"
	"\\xDE\\xA5\\xC9\\xC8\\x66\\xCE\\x79\\xCB\\x66\\xCE\\x65\\xCA\\x66\\xCE\\x65\\xC9"
	"\\x66\\xCE\\x7D\\xAA\\x59\\x35\\x1C\\x59\\xEC\\x60\\xC8\\xCB\\xCF\\xCA\\x66\\x4B"
	"\\xC3\\xC0\\x32\\x7B\\x77\\xAA\\x59\\x5A\\x71\\x76\\x67\\x66\\x66\\xDE\\xFC\\xED"
	"\\xC9\\xEB\\xF6\\xFA\\xD8\\xFD\\xFD\\xEB\\xFC\\xEA\\xEA\\x99\\xDA\\xEB\\xFC\\xF8"
	"\\xED\\xFC\\xC9\\xEB\\xF6\\xFA\\xFC\\xEA\\xEA\\xD8\\x99\\xDC\\xE1\\xF0\\xED\\xCD"
	"\\xF1\\xEB\\xFC\\xF8\\xFD\\x99\\xD5\\xF6\\xF8\\xFD\\xD5\\xF0\\xFB\\xEB\\xF8\\xEB"
	"\\xE0\\xD8\\x99\\xEE\\xEA\\xAB\\xC6\\xAA\\xAB\\x99\\xCE\\xCA\\xD8\\xCA\\xF6\\xFA"
	"\\xF2\\xFC\\xED\\xD8\\x99\\xFB\\xF0\\xF7\\xFD\\x99\\xF5\\xF0\\xEA\\xED\\xFC\\xF7"
	"\\x99\\xF8\\xFA\\xFA\\xFC\\xE9\\xED\\x99\\xFA\\xF5\\xF6\\xEA\\xFC\\xEA\\xF6\\xFA"
	"\\xF2\\xFC\\xED\\x99";
    


//	logInfo("pcre is %s \n",oc192bindpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(oc192bindpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("HODBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				oc192bindpcre, pcreEerror, pcreErrorPos);
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

//	(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);




	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
//		g_Nepenthes->getUtilities()->hexdump((unsigned char *)shellcode,len);
		const char * match;
		uint16_t port;
        
		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &match);

        port = ntohs(*(uint32_t *) match^ 0x9999);
        logInfo("Detected Lsass HOD listenshell shellcode, :%u \n", port);
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
