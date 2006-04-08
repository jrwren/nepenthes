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

NamespaceKonstanzXOR::NamespaceKonstanzXOR(sc_shellcode *sc): NamespaceShellcodeHandler(sc)
{

}

NamespaceKonstanzXOR::~NamespaceKonstanzXOR()
{

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
		logSpam("MATCH %s  matchCount %i map_items %i \n",m_ShellcodeHandlerName.c_str(), matchCount, m_MapItems);
		int32_t i;
		for ( i=0; i < m_MapItems; i++ )
		{
			if (m_Map[i] == sc_none)
					continue;

			logSpam(" i = %i map_items %i , map = %s\n",i,m_MapItems, sc_get_mapping_by_numeric(m_Map[i]));
			const char *match = NULL;
			int matchSize = pcre_get_substring((char *) shellcode, (int *)ovec, (int)matchCount, i, &match);

			switch ( m_Map[i] )
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
				logCrit("%s not used mapping %s\n",m_ShellcodeHandlerName.c_str(), sc_get_mapping_by_numeric(m_Map[i]));
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




