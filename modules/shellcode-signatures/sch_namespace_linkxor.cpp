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

#include "sch_namespace_linkxor.hpp"

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

NamespaceLinkXOR::NamespaceLinkXOR(sc_shellcode *sc)
{
	m_ShellcodeHandlerName = sc_get_namespace_by_numeric(sc->nspace);
	m_ShellcodeHandlerName += "::";
	m_ShellcodeHandlerName += sc->name;

	m_Shellcode = sc;

}

NamespaceLinkXOR::~NamespaceLinkXOR()
{

}

bool NamespaceLinkXOR::Init()
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

	printf("%s\n",m_Shellcode->pattern);
//	g_Nepenthes->getUtilities()->hexdump((byte *)m_Shellcode->pattern,m_Shellcode->pattern_size);
	return true;
}

bool NamespaceLinkXOR::Exit()
{
	return true;
}

sch_result NamespaceLinkXOR::handleShellcode(Message **msg)
{
	logSpam("%s checking %i...\n",m_ShellcodeHandlerName.c_str(), (*msg)->getSize());

	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t ovec[10 * 3];
	int32_t matchCount; 

//	"\\xEB\\x15\\xB9(....)\\x81\\xF1(....)\\x5E\\x80\\x74\\x31\\xFF(.)\\xE2\\xF9\\xEB\\x05\\xE8\\xE6\\xFF\\xFF\\xFF(.*)";



// size
	const char  *sizeAMatch	=   NULL;
	uint32_t	sizeA 		= 	0;

	const char  *sizeBMatch	=	NULL;
	uint32_t	sizeB 		= 	0;

	uint32_t codeSize		= 	0;

// key
	const char *keyMatch	=	NULL;
	byte		byteKey		= 	0;


// data after xor
	const char  *postMatch  =   NULL;
	uint32_t    postSize    =   0;




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
				if (sizeAMatch == NULL)
				{
					sizeAMatch = match;
					sizeA = *((uint32_t *)match);
				}else
				{
					sizeBMatch = match;
					sizeB = *((uint32_t *)match);
				}
				logSpam("\t value %0x\n",*(unsigned int *)match);
				break;

			case sc_key:
				logSpam("sc_key %i\n",matchSize);
				byteKey = *(byte *)match;
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

		codeSize = sizeA ^ sizeB;
		logInfo("Found linkbot XOR decoder, key 0x%02x, payload is 0x%04x bytes long.\n", byteKey, codeSize);
		
// create buffer for decoding part of the message
		byte *decodedMessage = (byte *)malloc(postSize);
		memcpy(decodedMessage, postMatch, postSize);



		if ( codeSize > postSize )
			logWarn("codeSize (%i) > postSize (%i), maybe broken xor?\n",codeSize,postSize);

		for ( uint32_t j = 0; j < codeSize && j < postSize; j++ )
			decodedMessage[j] ^= byteKey;

		g_Nepenthes->getUtilities()->hexdump(l_crit,(byte *)decodedMessage, postSize);			

		Message *newMessage = new Message((char *)decodedMessage, postSize, (*msg)->getLocalPort(), (*msg)->getRemotePort(),
										  (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());

		delete *msg;

		*msg = newMessage;

		free(decodedMessage);

		pcre_free_substring(sizeAMatch);
		pcre_free_substring(sizeBMatch);
		pcre_free_substring(keyMatch);
		pcre_free_substring(postMatch);

		return SCH_REPROCESS;
	}

	return SCH_NOTHING;
}




