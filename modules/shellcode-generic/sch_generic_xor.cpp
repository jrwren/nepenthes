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


/* Additional notes:
 *
 * The pcre's and processing logic in this module is derived from mwcollect written by Georg Wicherski
 *
 * if you got any idea what has to be done to relicense bsd code on a gpl license mail us
 * wikipedia states bsd code can be relicensed on to gpl, but we got no information what has to be done
 * 
 *
 */



#include "sch_generic_xor.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"
#include "Message.cpp"
#include "LogManager.hpp"
#include "Utilities.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr


using namespace nepenthes;

GenericXOR::GenericXOR(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "GenericXOR";
	m_ShellcodeHandlerDescription = "generic XOR decoder";
    
	
	fprintf(stderr,"\n");
	fprintf(stderr,"Parts of the generic xor shellcodehandler are based on \n");
	fprintf(stderr,"mwcollects generic xor shellcodehandler \n");
	fprintf(stderr,"mwcollect is\n"); 
	fprintf(stderr,"Copyright (c) 2005, Honeynet Project\n");
	fprintf(stderr,"All rights reserved.\n");
	fprintf(stderr,"published on a bsd license\n");
	fprintf(stderr,"and written by Georg Wicherski\n");
	fprintf(stderr,"http://www.mwcollect.org for more information about mwcollect\n");
	fprintf(stderr,"\n");

}

GenericXOR::~GenericXOR()
{

}

bool GenericXOR::Init()
{
	const char * pcreEerror;
	int32_t pcreErrorPos;

	const char *test[]=
	{
		"\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\x66\\xB9(.)\\xFF\\x80\\x73\\x0E(.)\\x43\\xE2\\xF9(.*)$", // rbot 64k 
		"\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\xB1(.)\\x80\\x73\\x0C(.)\\x43\\xE2\\xF9(.*)$",			// rbot 265 byte
		"\\xEB.\\xEB.\\xE8.*\\xB1(.).*\\x80..(.).*\\xE2.(.*)$",																	// generic mwcollect
		"\\xEB\\x10\\x5A\\x4A\\x33\\xC9\\x66\\xB9(..)\\x80\\x34\\x0A(.)\\xE2\\xFA\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF(.*)$",		// bielefeld
		 "\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\x66\\xB9(..)\\x80\\x73\\x0E(.)\\x43\\xE2\\xF9(.*)$",	// halle	
		NULL
	};

	for( uint32_t i = 0; test[i]; i++ )
	{
		pcre *mypcre;
		if((mypcre = pcre_compile(test[i], PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
		{
			logCrit("GenericXOR could not compile pattern %i\n\t\"%s\"\n\t Error:\"%s\" at Position %u", i,
					test[i], pcreEerror, pcreErrorPos);
			return false;
		}else
		{
			m_Pcres.push_back(mypcre);
			logSpam("PCRE %i compiled \n",i);
		}
	}

	return true;
}

bool GenericXOR::Exit()
{
	while(m_Pcres.size()>0)
	{
		free(m_Pcres.front());
		m_Pcres.pop_front();
	}
    	
	return true;

}

sch_result GenericXOR::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());

	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();
	int32_t output[10 * 3];

	list <pcre *>::iterator it;
	uint32_t i;
	for (it=m_Pcres.begin(), i=0; it != m_Pcres.end();it++,i++)
	{
		int32_t result=0;
		if((result = pcre_exec(*it, 0, (char *) shellcode, len, 0, 0, output, sizeof(output)/sizeof(int32_t))) > 0)
		{
/*			logSpam("PCRE %i %x matches %i \n",i,*it,result);
			if (i== 3)
			{
				logInfo("Happy crashing %i\n",i);
				g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *)shellcode, len);
			}
*/
			const char *match;
			byte key;
			uint32_t codesize = 0, codesizeLen, totalsize;

			codesizeLen = pcre_get_substring((char *) shellcode, output, result, 1, &match);
			if( codesizeLen == 2 )
				codesize = (uint32_t)*((uint16_t *)match);
			else
				codesize = (uint32_t)*((byte *)match);
			pcre_free_substring(match);


			pcre_get_substring((char *) shellcode, output, result, 2, &match);
			key = *((byte *)match);
			pcre_free_substring(match);

			


			totalsize = pcre_get_substring((char *) shellcode, output, result, 3, &match);
			byte *decodedMessage = (byte *)malloc(totalsize);
			memcpy(decodedMessage, match, totalsize);
			pcre_free_substring(match);

			logDebug("Detected generic XOR decoder #%i size length has %d bytes, size is %d, totalsize %d.\n",i, codesizeLen, codesize, totalsize);

			if( codesize > totalsize )
				logWarn("%s\n", "codesize > totalsize - broken shellcode?");

			for( uint32_t j = 0; j < codesize && j < totalsize; j++ )
				decodedMessage[j] ^= key;

			//g_Nepenthes->getUtilities()->hexdump(l_crit, decodedMessage, totalsize);

			Message *newMessage = new Message((char *)decodedMessage, totalsize, (*msg)->getLocalPort(), (*msg)->getRemotePort(),
				   (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());

			delete *msg;

			*msg = newMessage;

			free(decodedMessage);
			return SCH_REPROCESS;
		}

	}
	return SCH_NOTHING;
}

