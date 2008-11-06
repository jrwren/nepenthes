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

#include "sch_generic_linkxor.hpp"
#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "Utilities.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

#ifndef min
#define min(a,b) ((a) > (b) ? (b) : (a))
#endif

#include <cstring>

using namespace nepenthes;

LinkXOR::LinkXOR(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "LinkXOR";
	m_ShellcodeHandlerDescription = "link-bot XOR decoder";
	m_linkDecoder = NULL;
}

LinkXOR::~LinkXOR()
{

}
/*

; stuttgart
00402007   EB 15            JMP SHORT stuttgar.0040201E
00402009   B9 8BE61341      MOV ECX,4113E68B
0040200E   81F1 D8E71341    XOR ECX,4113E7D8
00402014   5E               POP ESI
00402015   807431 FF A2     XOR BYTE PTR DS:[ECX+ESI-1],0A2
0040201A  ^E2 F9            LOOPD SHORT stuttgar.00402015
0040201C   EB 05            JMP SHORT stuttgar.00402023
0040201E   E8 E6FFFFFF      CALL stuttgar.00402009

*/

bool LinkXOR::Init()
{
	const char *linkDecoder = "\\xEB\\x15\\xB9(....)\\x81\\xF1(....)\\x5E\\x80\\x74\\x31\\xFF(.)\\xE2\\xF9\\xEB\\x05\\xE8\\xE6\\xFF\\xFF\\xFF(.*)";

	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_linkDecoder = pcre_compile(linkDecoder, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("LinkXOR could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				linkDecoder, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool LinkXOR::Exit()
{
	if(m_linkDecoder != NULL)
    	free(m_linkDecoder);
	return true;

}

sch_result LinkXOR::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());


	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t offvec[10 * 3];
	int32_t result;

	if( (result = pcre_exec(m_linkDecoder, 0, (char *)shellcode, len, 0, 0, (int *)offvec, sizeof(offvec)/sizeof(int32_t))) > 0 )
	{
		const char *substring;

		uint32_t a, b, payloadLen;
		uint32_t realLen;
		byte key;
		byte *payload;

		pcre_get_substring((char *)shellcode, (int *)offvec, (int)result, 1, &substring);
		a = *((uint32_t *)substring);
		pcre_free_substring(substring);

		pcre_get_substring((char *)shellcode, (int *)offvec, (int)result, 2, &substring);
		b = *((uint32_t *)substring);
		pcre_free_substring(substring);

		payloadLen = a ^ b;

		pcre_get_substring((char *)shellcode, (int *)offvec, (int)result, 3, &substring);
		key = *substring;
		pcre_free_substring(substring);

		logInfo("Found linkbot XOR decoder, key 0x%02x, payload is 0x%04x bytes long.\n", key, payloadLen);

		if ( (realLen = pcre_get_substring((char *)shellcode, (int *)offvec, (int)result, 4, &substring)) < payloadLen)
		{
			logWarn("linkbot XOR decoder expected len %i actual len %i\n",payloadLen,realLen);
			payloadLen = realLen;
		}

		payload = (byte *)malloc(payloadLen);
		memcpy(payload, substring, payloadLen);
		pcre_free_substring(substring);

		for( uint32_t i = 0; i < payloadLen; i++ )
			payload[i] ^= key;

		//g_Nepenthes->getUtilities()->hexdump(l_crit, payload, payloadLen);

		// recompose the message with our new shellcode.
		Message *nmsg;
		nmsg = new Message((char *)payload, payloadLen, (*msg)->getLocalPort(), (*msg)->getRemotePort(),
			   (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());

		delete *msg;   // FIXME	Message muss _komplett_ im header implementiert werden, oder gegen das modul gelinked
		*msg = nmsg;
		free(payload);
		return SCH_REPROCESS;
	}

	return SCH_NOTHING;
}


