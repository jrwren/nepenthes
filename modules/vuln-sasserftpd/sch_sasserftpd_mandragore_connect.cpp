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
#include "sch_sasserftpd_mandragore_connect.hpp"
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

MandragoreConnect::MandragoreConnect(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "MandragoreConnect";
	m_ShellcodeHandlerDescription = "mandragore sasserftpd bondshells";
	m_pcre = NULL;
}

MandragoreConnect::~MandragoreConnect()
{

}


bool MandragoreConnect::Init()
{
	logPF();

	m_pcre = NULL;

	// FIXME THIS PCRE IS BROKEN
    const char *thcconnectpcre = //".*(\\xEB\\x0F\\x8B\\x34\\x24\\x33\\xC9\\x80\\xC1\\xB6\\x80\\x36\\xDE\\x46\\xE2\\xFA.*\\x55\\x9E\\xC2\\x55\\xDE\\x21\\xAE\\xD6\\x21\\xC8\\x21\\x0E).*";

	"\\xEB\\x0F\\x8B\\x34\\x24\\x33\\xC9\\x80\\xC1\\xB6\\x80\\x36\\xDE\\x46\\xE2\\xFA"
	"\\xC3\\xE8\\xEC\\xFF\\xFF\\xFF\\xBA\\xB9\\x51\\xD8\\xDE\\xDE\\x60..."
	".\\xB6\\xED\\xEC\\xDE\\xDE\\xB6\\xA9\\xAD\\xEC\\x81\\x8A\\x21\\xCB.."
	"..\\x49\\x47\\x8C\\x8C\\x8C\\x8C\\x9C\\x8C\\x9C\\x8C\\x36\\xD5\\xDE\\xDE"
	"\\xDE\\x89\\x8D\\x9F\\x8D\\xB1\\xBD\\xB5\\xBB\\xAA\\x9F\\xDE\\x89\\x21\\xC8\\x21"
	"\\x0E\\x4D\\xB6(....)\\xB6\\xDC\\xDE(..)\\x55\\x1A\\xB4\\xCE"
	"\\x8E\\x8D\\x36\\xD6\\xDE\\xDE\\xDE\\xBD\\xB1\\xB0\\xB0\\xBB\\xBD\\xAA\\xDE\\x89"
	"\\x21\\xC8\\x21\\x0E\\xB4\\xCE\\x87\\x55\\x22\\x89\\xDD\\x27\\x89\\x2D\\x75\\x55"
	"\\xE2\\xFA\\x8E\\x8E\\x8E\\xB4\\xDF\\x8E\\x8E\\x36\\xDA\\xDE\\xDE\\xDE\\xBD\\xB3"
	"\\xBA\\xDE\\x8E\\x36\\xD1\\xDE\\xDE\\xDE\\x9D\\xAC\\xBB\\xBF\\xAA\\xBB\\x8E\\xAC"
	"\\xB1\\xBD\\xBB\\xAD\\xAD\\x9F\\xDE\\x18\\xD9\\x9A\\x19\\x99\\xF2\\xDF\\xDF\\xDE"
	"\\xDE\\x5D\\x19\\xE6\\x4D\\x75\\x75\\x75\\xBA\\xB9\\x7F\\xEE\\xDE\\x55\\x9E\\xD2"
	"\\x55\\x9E\\xC2\\x55\\xDE\\x21\\xAE\\xD6\\x21\\xC8\\x21\\x0E";

	logInfo("pcre is %s \n",thcconnectpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(thcconnectpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("MandragoreConnect could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				thcconnectpcre, pcreEerror, pcreErrorPos);
		return false;
	}else
	{
		logInfo("Compiled Pattern %s \n",__PRETTY_FUNCTION__);
	}
	return true;
}

bool MandragoreConnect::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}



sch_result MandragoreConnect::handleShellcode(Message **msg)
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
		uint32_t address;
		const char *match;
        
		pcre_get_substring((char *)shellcode, ovec, matchCount, 1, &match);
		memcpy(&address, match, 4);
		address=address^0xdededede;
		pcre_free_substring(match);
		

		pcre_get_substring((char *)shellcode, ovec, matchCount, 2, &match);
        memcpy(&netPort, match, 2);
		netPort ^= 0xdede;
		port = ntohs(netPort);
		pcre_free_substring(match);


		logInfo("Mandragore ConnectBack Shell at %s:%d, \n",	inet_ntoa(*(in_addr *)&address), port);

		Socket *sock = g_Nepenthes->getSocketMgr()->connectTCPHost(0,address,port,30);
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

