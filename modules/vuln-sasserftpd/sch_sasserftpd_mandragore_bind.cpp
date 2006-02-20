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
#include "sch_sasserftpd_mandragore_bind.hpp"
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

MandragoreBind::MandragoreBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "MandragoreBind";
	m_ShellcodeHandlerDescription = "mandragore sasserftpd bondshells";
	m_pcre = NULL;
}

MandragoreBind::~MandragoreBind()
{

}

bool MandragoreBind::Init()
{
	logPF();

	m_pcre = NULL;

//    const char *thcconnectpcre = ".*(\\xEB\\x0F\\x8B\\x34\\x24\\x33\\xC9\\x80\\xC1\\xDD\\x80\\x36\\xDE\\x46\\xE2\\xFA.*\\xB9\\x7F\\xEE\\xDE\\x55\\x9E\\xD2\\x55\\x9E\\xC2\\x55\\xDE\\x21\\xAE\\xD6\\x21\\xC8\\x21\\x0E).*";
	const char *pcre =
		"\\xEB\\x0F\\x8B\\x34\\x24\\x33\\xC9\\x80\\xC1\\xDD\\x80\\x36\\xDE\\x46\\xE2\\xFA"
		"\\xC3\\xE8\\xEC\\xFF\\xFF\\xFF\\xBA\\xB9\\x51\\xD8\\xDE\\xDE\\x60..."
		".\\xB6\\xED\\xEC\\xDE\\xDE\\xB6\\xA9\\xAD\\xEC\\x81\\x8A\\x21\\xCB.."
		"..\\x49\\x47\\x8C\\x8C\\x8C\\x8C\\x9C\\x8C\\x9C\\x8C\\x36\\xD5\\xDE\\xDE"
		"\\xDE\\x89\\x8D\\x9F\\x8D\\xB1\\xBD\\xB5\\xBB\\xAA\\x9F\\xDE\\x89\\x21\\xC8\\x21"
		"\\x0E\\x4D\\xB4\\xDE\\xB6\\xDC\\xDE(..)\\x55\\x1A\\xB4\\xCE\\x8E\\x8D\\x36"
		"\\xDB\\xDE\\xDE\\xDE\\xBC\\xB7\\xB0\\xBA\\xDE\\x89\\x21\\xC8\\x21\\x0E\\xB4\\xDF"
		"\\x8D\\x36\\xD9\\xDE\\xDE\\xDE\\xB2\\xB7\\xAD\\xAA\\xBB\\xB0\\xDE\\x89\\x21\\xC8"
		"\\x21\\x0E\\xB4\\xDE\\x8A\\x8D\\x36\\xD9\\xDE\\xDE\\xDE\\xBF\\xBD\\xBD\\xBB\\xAE"
		"\\xAA\\xDE\\x89\\x21\\xC8\\x21\\x0E\\x55\\x06\\xED\\x1E\\xB4\\xCE\\x87\\x55\\x22"
		"\\x89\\xDD\\x27\\x89\\x2D\\x75\\x55\\xE2\\xFA\\x8E\\x8E\\x8E\\xB4\\xDF\\x8E\\x8E"
		"\\x36\\xDA\\xDE\\xDE\\xDE\\xBD\\xB3\\xBA\\xDE\\x8E\\x36\\xD1\\xDE\\xDE\\xDE\\x9D"
		"\\xAC\\xBB\\xBF\\xAA\\xBB\\x8E\\xAC\\xB1\\xBD\\xBB\\xAD\\xAD\\x9F\\xDE\\x18\\xD9"
		"\\x9A\\x19\\x99\\xF2\\xDF\\xDF\\xDE\\xDE\\x5D\\x19\\xE6\\x4D\\x75\\x75\\x75\\xBA"
		"\\xB9\\x7F\\xEE\\xDE\\x55\\x9E\\xD2\\x55\\x9E\\xC2\\x55\\xDE\\x21\\xAE\\xD6\\x21"
		"\\xC8\\x21\\x0E";


//	logInfo("pcre is %s \n",pcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(pcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("MandragoreBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				pcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool MandragoreBind::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}



sch_result MandragoreBind::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
        const char * pCode;
		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &pCode);

        uint16_t port = *(uint16_t *)pCode;
		port^=0xdede;
		port = ntohs(port);
		logInfo("Mandragore Bind %i  %i\n",port,(*msg)->getSize());

		Socket *sock;

		if ((sock = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,60,30)) == NULL)
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
		sock->addDialogueFactory(diaf);
		pcre_free_substring(pCode);

		return SCH_DONE;
	}
	return SCH_NOTHING;
}

