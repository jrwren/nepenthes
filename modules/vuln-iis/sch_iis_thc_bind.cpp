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


#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_iis_thc_bind.hpp"
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

THCBind::THCBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "THCBind";
	m_ShellcodeHandlerDescription = "handles thc iis bindshells";
	m_pcre = NULL;
}

THCBind::~THCBind()
{

}

/*

*/

bool THCBind::Init()
{
	logPF();

	const char *thcconnectpcre = 
	"\xeb\x23\x7a\x69\x02\x05\x6c\x59\xf8\x1d\x9c\xde\x8c\xd1\x4c"
	"\x70\xd4\x03\xf0\x27\x20\x20\x30\x08\x57\x53\x32\x5f\x33\x32"
	"\x2e\x44\x4c\x4c\x01\xeb\x05\xe8\xf9\xff\xff\xff\x5d\x83\xed"
	"\x2a\x6a\x30\x59\x64\x8b\x01\x8b\x40\x0c\x8b\x70\x1c\xad\x8b"
	"\x78\x08\x8d\x5f\x3c\x8b\x1b\x01\xfb\x8b\x5b\x78\x01\xfb\x8b"
	"\x4b\x1c\x01\xf9\x8b\x53\x24\x01\xfa\x53\x51\x52\x8b\x5b\x20"
	"\x01\xfb\x31\xc9\x41\x31\xc0\x99\x8b\x34\x8b\x01\xfe\xac\x31"
	"\xc2\xd1\xe2\x84\xc0\x75\xf7\x0f\xb6\x45\x05\x8d\x44\x45\x04"
	"\x66\x39\x10\x75\xe1\x66\x31\x10\x5a\x58\x5e\x56\x50\x52\x2b"
	"\x4e\x10\x41\x0f\xb7\x0c\x4a\x8b\x04\x88\x01\xf8\x0f\xb6\x4d"
	"\x05\x89\x44\x8d\xd8\xfe\x4d\x05\x75\xbe\xfe\x4d\x04\x74\x21"
	"\xfe\x4d\x22\x8d\x5d\x18\x53\xff\xd0\x89\xc7\x6a\x04\x58\x88"
	"\x45\x05\x80\x45\x77\x0a\x8d\x5d\x74\x80\x6b\x26\x14\xe9\x78"
	"\xff\xff\xff\x89\xce\x31\xdb\x53\x53\x53\x53\x56\x46\x56\xff"
	"\xd0\x97\x55\x58\x66\x89\x30\x6a\x10\x55\x57\xff\x55\xd4\x4e"
	"\x56\x57\xff\x55\xcc\x53\x55\x57\xff\x55\xd0\x97\x8d\x45\x88"
	"\x50\xff\x55\xe4\x55\x55\xff\x55\xe8\x8d\x44\x05\x0c\x94\x53"
	"\x68\x2e\x65\x78\x65\x68\x5c\x63\x6d\x64\x94\x31\xd2\x8d\x45"
	"\xcc\x94\x57\x57\x57\x53\x53\xfe\xc6\x01\xf2\x52\x94\x8d\x45"
	"\x78\x50\x8d\x45\x88\x50\xb1\x08\x53\x53\x6a\x10\xfe\xce\x52"
	"\x53\x53\x53\x55\xff\x55\xec\x6a\xff\xff\x55\xe0";

//	logInfo("pcre is %s \n",thcconnectpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(thcconnectpcre, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("THCBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				thcconnectpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool THCBind::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}



sch_result THCBind::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
        const char * pCode;
		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 1, &pCode);

		logInfo("THC Bind 31337  %i\n",(*msg)->getSize());

		
		Socket *socket;
		if ((socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,31337,60,30)) == NULL)
		{
			logCrit("%s","Could not bind socket %u \n",31337);
			return SCH_DONE;
		}

		DialogueFactory *diaf;
		if ((diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL)
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return SCH_DONE;
		}
		socket->addDialogueFactory(diaf);
		pcre_free_substring(pCode);

		return SCH_DONE;
	}
	return SCH_NOTHING;
}
