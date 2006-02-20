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

#include "sch_generic_konstanz_xor.hpp"
#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "Utilities.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr


using namespace nepenthes;

KonstanzXOR::KonstanzXOR(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "KonstanzXOR";
	m_ShellcodeHandlerDescription = "Konstanz XOR decoder";
	m_konstanzDecoder = NULL;
}

KonstanzXOR::~KonstanzXOR()
{

}

/*
	00402003   66:b9 0501       mov cx,105
	00402007   e8 ffffffff      call konstanz.0040200b
	0040200b   (ff)c1           inc ecx                         ; note: ff in parenthesis overlaps with previous instruction!
	0040200d   5e               pop esi
	0040200e   304c0e 07        xor byte ptr ds:[esi+ecx+7],cl  ; xor key is index
	00402012  ^e2 fa            loopd short konstanz.0040200e
*/

bool KonstanzXOR::Init()
{
	const char *konstanzDecoder = "\\x33\\xC9\\x66\\xB9(..)\\xE8\\xFF\\xFF\\xFF\\xFF\\xC1\\x5E\\x30\\x4C\\x0E\\x07\\xE2\\xFA(.*)";

	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_konstanzDecoder = pcre_compile(konstanzDecoder, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("KonstanzXOR could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				konstanzDecoder, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool KonstanzXOR::Exit()
{
	if(m_konstanzDecoder != NULL)
    	free(m_konstanzDecoder);
	return true;

}


sch_result KonstanzXOR::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());


	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();

	int32_t offvec[10 * 3];
	int32_t result;

	if( (result = pcre_exec(m_konstanzDecoder, 0, (char *)shellcode, len, 0, 0, offvec, sizeof(offvec)/sizeof(int32_t))) > 0 )
	{
		const char *substring;

		uint16_t payloadLen, payloadSize;
        byte *payload;

		pcre_get_substring((char *)shellcode, offvec, result, 1, &substring);
		payloadLen = *((uint16_t *)substring);
		payloadLen +=1;
		pcre_free_substring(substring);

		payloadSize = pcre_get_substring((char *)shellcode, offvec, result, 2, &substring);

		if (payloadSize < payloadLen )
		{
			pcre_free_substring(substring);
			return SCH_NOTHING;
		}

		payload = (byte *)malloc((uint32_t)payloadLen);
		memcpy(payload, substring, (uint32_t)payloadLen);
		pcre_free_substring(substring);

		logDebug("Found konstanzbot XOR decoder, payload is 0x%04x bytes long.\n", (uint32_t)payloadLen);



		for( uint32_t i = 0; i < payloadLen; i++ )
			payload[i] ^= (i+1);

//		g_Nepenthes->getUtilities()->hexdump(payload, payloadLen);

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


