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
#include "sch_msmq_hod_bind.hpp"
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
	m_ShellcodeHandlerDescription = "handles house of dabus msmq bindshells";
	m_pcre = NULL;
}

HODBind::~HODBind()
{

}



bool HODBind::Init()
{
	logPF();

	const char *thcconnectpcre = //".*(\\x29\\xc9\\x83\\xe9\\xb0\\xd9\\xee\\xd9\\x74\\x24\\xf4\\x5b\\x81\\x73\\x13\\x19.*\\x4a\\x0a\\xd2\\xc8\\xc9\\xf5\\x04\\x37).*";
	"\\x29\\xC9\\x83\\xE9\\xB0\\xD9\\xEE\\xD9\\x74\\x24\\xF4\\x5B\\x81\\x73\\x13\\x19"
	"\\xF5\\x04\\x37\\x83\\xEB\\xFC\\xE2\\xF4\\xE5\\x9F\\xEF\\x7A\\xF1\\x0C\\xFB\\xC8"
	"\\xE6\\x95\\x8F\\x5B\\x3D\\xD1\\x8F\\x72\\x25\\x7E\\x78\\x32\\x61\\xF4\\xEB\\xBC"
	"\\x56\\xED\\x8F\\x68\\x39\\xF4\\xEF\\x7E\\x92\\xC1\\x8F\\x36\\xF7\\xC4\\xC4\\xAE"
	"\\xB5\\x71\\xC4\\x43\\x1E\\x34\\xCE\\x3A\\x18\\x37\\xEF\\xC3\\x22\\xA1\\x20\\x1F"
	"\\x6C\\x10\\x8F\\x68\\x3D\\xF4\\xEF\\x51\\x92\\xF9\\x4F\\xBC\\x46\\xE9\\x05\\xDC"
	"\\x1A\\xD9\\x8F\\xBE\\x75\\xD1\\x18\\x56\\xDA\\xC4\\xDF\\x53\\x92\\xB6\\x34\\xBC"
	"\\x59\\xF9\\x8F\\x47\\x05\\x58\\x8F\\x77\\x11\\xAB\\x6C\\xB9\\x57\\xFB\\xE8\\x67"
	"\\xE6\\x23\\x62\\x64\\x7F\\x9D\\x37\\x05\\x71\\x82\\x77\\x05\\x46\\xA1\\xFB\\xE7"
	"\\x71\\x3E\\xE9\\xCB\\x22\\xA5\\xFB\\xE1\\x46\\x7C\\xE1\\x51\\x98\\x18\\x0C\\x35"
	"\\x4C\\x9F\\x06\\xC8\\xC9\\x9D\\xDD\\x3E\\xEC\\x58\\x53\\xC8\\xCF\\xA6\\x57\\x64"
	"\\x4A\\xA6\\x47\\x64\\x5A\\xA6\\xFB\\xE7\\x7F\\x9D(..)\\x7F\\xA6\\x8D\\xD6"
	"\\x8C\\x9D\\xA0\\x2D\\x69\\x32\\x53\\xC8\\xCF\\x9F\\x14\\x66\\x4C\\x0A\\xD4\\x5F"
	"\\xBD\\x58\\x2A\\xDE\\x4E\\x0A\\xD2\\x64\\x4C\\x0A\\xD4\\x5F\\xFC\\xBC\\x82\\x7E"
	"\\x4E\\x0A\\xD2\\x67\\x4D\\xA1\\x51\\xC8\\xC9\\x66\\x6C\\xD0\\x60\\x33\\x7D\\x60"
	"\\xE6\\x23\\x51\\xC8\\xC9\\x93\\x6E\\x53\\x7F\\x9D\\x67\\x5A\\x90\\x10\\x6E\\x67"
	"\\x40\\xDC\\xC8\\xBE\\xFE\\x9F\\x40\\xBE\\xFB\\xC4\\xC4\\xC4\\xB3\\x0B\\x46\\x1A"
	"\\xE7\\xB7\\x28\\xA4\\x94\\x8F\\x3C\\x9C\\xB2\\x5E\\x6C\\x45\\xE7\\x46\\x12\\xC8"
	"\\x6C\\xB1\\xFB\\xE1\\x42\\xA2\\x56\\x66\\x48\\xA4\\x6E\\x36\\x48\\xA4\\x51\\x66"
	"\\xE6\\x25\\x6C\\x9A\\xC0\\xF0\\xCA\\x64\\xE6\\x23\\x6E\\xC8\\xE6\\xC2\\xFB\\xE7"
	"\\x92\\xA2\\xF8\\xB4\\xDD\\x91\\xFB\\xE1\\x4B\\x0A\\xD4\\x5F\\xF6\\x3B\\xE4\\x57"
	"\\x4A\\x0A\\xD2\\xC8\\xC9\\xF5\\x04\\x37";

//	logInfo("pcre is %s \n",thcconnectpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(thcconnectpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
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

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
		const char * match;
		uint16_t port;

		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &match);

		port = ntohs(*(uint32_t *) match);
		port ^= 0x0437;
		logInfo("Detected MSMQ HOD listenshell shellcode, :%u \n", port);
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

