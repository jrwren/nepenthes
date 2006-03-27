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

#include <stdint.h>

#include "sch_namespace_konstanzxor.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"

#include "LogManager.hpp"
#include "Utilities.hpp"

#include "parser.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

NamespaceKonstanzXOR::NamespaceKonstanzXOR(sc_shellcode *sc)
{
	m_ShellcodeHandlerName = sc_get_namespace_by_numeric(sc->nspace);
	m_ShellcodeHandlerName += "::";
	m_ShellcodeHandlerName += sc->name;

	m_Shellcode = sc;

}

NamespaceKonstanzXOR::~NamespaceKonstanzXOR()
{

}

bool NamespaceKonstanzXOR::Init()
{
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if ( (m_Pcre = pcre_compile(m_Shellcode->pattern, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL )
	{
		logCrit("%s could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				m_ShellcodeHandlerName.c_str(), pcreEerror, pcreErrorPos);
		return false;
	} else
	{
		logInfo("%s loaded ...\n",m_ShellcodeHandlerName.c_str());
	}

//	printf("%s\n",m_Shellcode->pattern);
//	g_Nepenthes->getUtilities()->hexdump((byte *)m_Shellcode->pattern,m_Shellcode->pattern_size);
	return true;
}

bool NamespaceKonstanzXOR::Exit()
{
	return true;
}

sch_result NamespaceKonstanzXOR::handleShellcode(Message **msg)
{
	logSpam("%s checking %i...\n",m_ShellcodeHandlerName.c_str(), (*msg)->getSize());

	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 

// size
	const char  *sizeMatch	=   NULL;
	uint16_t codeSize		= 	0;

// post
	const char  *postMatch	=   NULL;
	uint16_t 	postSize	= 	0;
	

	if ( (matchCount = pcre_exec(m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)ovec, sizeof(ovec)/sizeof(int32_t))) > 0 )
	{
		logCrit("MATCH %s  matchCount %i map_items %i \n",m_ShellcodeHandlerName.c_str(), matchCount, m_Shellcode->map_items);
		int32_t i;
		for ( i=0; i < m_Shellcode->map_items; i++ )
		{
			if (m_Shellcode->map[i] == sc_none)
					continue;

			logInfo(" i = %i map_items %i , map = %s\n",i,m_Shellcode->map_items, sc_get_mapping_by_numeric(m_Shellcode->map[i]));
			const char *match = NULL;
			int matchSize = pcre_get_substring((char *) shellcode, (int *)ovec, (int)matchCount, i, &match);

			switch ( m_Shellcode->map[i] )
			{

			case sc_size:
				logSpam("sc_size %i\n",matchSize);
				sizeMatch = match;
				codeSize = *(uint16_t *)match;
				logSpam("\t value %0x\n",*(unsigned int *)match);
				break;

			case sc_post:
				logSpam("sc_post %i\n",matchSize);
				postMatch = match;
				postSize = matchSize;
				break;

			default:
				logCrit("%s not used mapping %s\n",m_ShellcodeHandlerName.c_str(), sc_get_mapping_by_numeric(m_Shellcode->map[i]));
			}
		}


		if (codeSize > postSize )
		{
			postSize = codeSize;
		}

		byte *decodedMessage = (byte *)malloc((uint32_t)postSize);
		memcpy(decodedMessage,postMatch, (uint32_t)postSize);


		logDebug("Found konstanzbot XOR decoder, size %i is %i bytes long.\n", codeSize,postSize);



		for( uint32_t i = 0; i < postSize; i++ )
			decodedMessage[i] ^= (i+1);

		// recompose the message with our new shellcode.
		Message *nmsg;
		nmsg = new Message((char *)decodedMessage, postSize, (*msg)->getLocalPort(), (*msg)->getRemotePort(),
			   (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());

		delete *msg;
		*msg = nmsg;

		free(decodedMessage);
		pcre_free_substring(postMatch);
		pcre_free_substring(sizeMatch);
	
		return SCH_REPROCESS;
	}

	return SCH_NOTHING;
}




