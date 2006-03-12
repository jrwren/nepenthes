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

#define XF_NONE 			0x00001
#define XF_SIZE_INVERT 		0x00002
#define XF_INVERSE_ORDER 	0x00004

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

	XORPcreHelper test[17]=
	{
		{
			"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\x66\\xB9(.)\\xFF\\x80\\x73\\x0E(.)\\x43\\xE2\\xF9)(.*)$", 
			"rbot 64k",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\xB1(.)\\x80\\x73\\x0C(.)\\x43\\xE2\\xF9)(.*)$",			
			"rbot 265 byte",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x10\\x5A\\x4A\\x33\\xC9\\x66\\xB9(..)\\x80\\x34\\x0A(.)\\xE2\\xFA\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF)(.*)$",		
			"bielefeld",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\x66\\xB9(..)\\x80\\x73\\x0E(.)\\x43\\xE2\\xF9)(.*)$",		
			"halle",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x19\\x5E\\x31\\xC9\\x81\\xE9(....)\\x81\\x36(....)\\x81\\xEE\\xFC\\xFF\\xFF\\xFF\\xE2\\xF2\\xEB\\x05\\xE8\\xE2\\xFF\\xFF\\xFF)(.*)$", 			
			"adenau xor",
			XF_SIZE_INVERT
		},
		
		{
			"(.*)(\\xEB\\x03\\x5D\\xEB\\x05\\xE8\\xF8\\xFF\\xFF\\xFF\\x8B\\xC5\\x83\\xC0\\x11\\x33\\xC9\\x66\\xB9(..)\\x80\\x30(.)\\x40\\xE2\\xFA)(.*)$",	
			"kaltenborn xor",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x10\\x5A\\x4A\\x31\\xC9\\x66\\xB9\(..)\\x80\\x34\\x0A(.)\\xE2\\xFA\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF)(.*)$",
			"deggendorf xor",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x0F\\x5B\\x33\\xC9\\x66\\xB9(..)\\x80\\x33(.)\\x43\\xE2\\xFA\\xEB\\x05\\xE8\\xEC\\xFF\\xFF\\xFF)(.*)$",
			"langenfeld xor",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x03\\x5D\\xEB\\x05\\xE8\\xF8\\xFF\\xFF\\xFF\\x83\\xC5\\x15\\x90\\x90\\x90\\x8B\\xC5\\x33\\xC9\\x66\\xB9(..)\\x50\\x80\\x30(.)\\x40\\xE2\\xFA)(.*)$",
			"saalfeld xor",
			XF_NONE
		},
		{
			"(.*)(\\x31\\xC9\\x83\\xE9(.)\\xD9\\xEE\\xD9\\x74\\x24\\xF4\\x5B\\x81\\x73\\x13(....)\\x83\\xEB\\xFC\\xE2\\xF4)(.*)$",
			"schoenberg xor",
			XF_SIZE_INVERT
		},
		{
			"(.*)(\\x33\\xC0\\xF7\\xD0\\x8B\\xFC\\xF2\\xAF\\x57\\x33\\xC9\\xB1(.)\\x90\\x90\\x90\\x90\\x80\\x37(.)\\x47\\xE2\\xFA.*\\xFF\\xFF\\xFF\\xFF)(.*)$",
			"rosengarten xor",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x0F\\x8B\\x34\\x24\\x33\\xC9\\x80\\xC1(.)\\x80\\x36(.)\\x46\\xE2\\xFA\\xC3\\xE8\\xEC\\xFF\\xFF\\xFF)(.*)$",
			"schauenburg xor",
			XF_NONE
		},
		{
			"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x58\\x83\\xC0\\x1B\\x8D\\xA0\\x01\\xFC\\xFF\\xFF\\x83\\xE4\\xFC\\x8B\\xEC\\x33\\xC9\\x66\\xB9(..)\\x80\\x30(.)\\x40\\xE2\\xFA)(.*)$",
			"lichtenfels xor",
			XF_NONE

		},
		{
			"(.*)(\\xC9\\x83\\xE9(.)\\xD9\\xEE\\xD9\\x74\\x24\\xF4\\x5B\\x81\\x73\\x13(....)\\x83\\xEB\\xFC\\xE2\\xF4)(.*)$",
			"Metasploit PexEnvSub",
			XF_SIZE_INVERT
		},
		{
			"(.*)(\\x2B\\xC9\\x83\\xE9(.)\\xE8\\xFF\\xFF\\xFF\\xFF\\xC0\\x5E\\x81\\x76\\x0E(....)\\x83\\xEE\\xFC\\xE2\\xF4)(.*)$",
			"Metasploit Pex",
			XF_SIZE_INVERT
		},
		{
			"(.*)(\\xEB\\x0E\\x5B\\x4B\\x33\\xC9\\xB1(.)\\x80\\x34\\x0B(.)\\xE2\\xFA\\xEB\\x05\\xE8\\xED\\xFF\\xFF\\xFF)(.*)$",
			"leimbach xor",
			XF_NONE
		},
		{
			"(.*)(\\xEB.\\xEB.\\xE8.*\\xB1(.).*\\x80..(.).*\\xE2.)(.*)$",																	
			"generic mwcollect",
			XF_NONE
		}
	};


	//logDebug("Amount of generic PCRE Helpers: %i\n", sizeof(test));
	for( uint32_t i = 0; i < 17; i++ )
	{
		pcre *mypcre;
		if((mypcre = pcre_compile(test[i].m_PCRE, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
		{
			logCrit("GenericXOR could not compile pattern %i\n\t\"%s\"\n\t Error:\"%s\" at Position %u", i,
					test[i], pcreEerror, pcreErrorPos);
			return false;
		}else
		{
			logDebug("Adding %s \n",test[i].m_Name);
			XORPcreContext *ctx = new XORPcreContext;
			ctx->m_Pcre = mypcre;
			ctx->m_Name = test[i].m_Name;
			ctx->m_Options = test[i].m_Options;
			m_Pcres.push_back(ctx);

			logSpam("PCRE %i compiled \n",i);
		}
	}

	return true;
}

bool GenericXOR::Exit()
{
	while(m_Pcres.size()>0)
	{

		pcre_free(m_Pcres.front()->m_Pcre);
		delete m_Pcres.front();
		m_Pcres.pop_front();
	}
    	
	return true;

}

sch_result GenericXOR::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());

	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getSize();
	int32_t output[10 * 3];

	list <XORPcreContext *>::iterator it;
	uint32_t i;
	for (it=m_Pcres.begin(), i=0; it != m_Pcres.end();it++,i++)
	{
		int32_t result=0;
		if((result = pcre_exec((*it)->m_Pcre, 0, (char *) shellcode, len, 0, 0, (int *)output, sizeof(output)/sizeof(int32_t))) > 0)
		{
//			logSpam("PCRE %i %x matches %i \n",i,*it,result);
			const char *preload;
			uint32_t preloadSize;
			preloadSize = pcre_get_substring((char *) shellcode, (int *)output, (int)result, 1, &preload);


			const char *xordecoder;
			uint32_t xordecoderSize;
			xordecoderSize = pcre_get_substring((char *) shellcode, (int *)output, (int)result, 2, &xordecoder);			


			const char *match;
			byte key=0;
			uint32_t longkey=0;
			uint32_t keysize;
			uint32_t codesize = 0, codesizeLen, totalsize;

			codesizeLen = pcre_get_substring((char *) shellcode, (int *)output, (int)result, 3, &match);
			switch ( codesizeLen )
			{
			case 4:
				// this is a special case, for dword xor we have to invert the size
				if ( (*it)->m_Options & XF_SIZE_INVERT )
				{
					logSpam("Inverting Size %i\n",codesize);
					codesize = 0 - (uint32_t)*((uint32_t *)match);
				} else
				{
					codesize = (uint32_t)*((uint32_t *)match);
				}
				break;

			case 2:
				codesize = (uint32_t)*((uint16_t *)match);
				break;

			case 1:
				if ( (*it)->m_Options & XF_SIZE_INVERT )
				{
					logSpam("Inverting Size %i\n",codesize);
					codesize = 256 - (uint32_t)*((byte *)match);
				} else
				{
					codesize = (uint32_t)*((byte *)match);
				}
				break;
			}

			pcre_free_substring(match);



			keysize = pcre_get_substring((char *) shellcode, (int *)output, (int)result, 4, &match);
			switch ( keysize )
			{
			
			case 1:
				key = *((byte *)match);
				break;

			case 4:
				longkey = *((uint32_t *)match);
				break;

			}
			
			pcre_free_substring(match);



			totalsize = pcre_get_substring((char *) shellcode, (int *)output, (int)result, 5, &match);
			byte *decodedMessage = (byte *)malloc(totalsize);
			memcpy(decodedMessage, match, totalsize);
			pcre_free_substring(match);

			logInfo("Detected generic XOR decoder %s size length has %d bytes, size is %d, totalsize %d.\n",(*it)->m_Name.c_str(), codesizeLen, codesize, totalsize);

				

			switch(keysize)
			{
			case 1:
				if( codesize > totalsize )
					logWarn("%s\n", "codesize > totalsize - broken shellcode?");

            	for( uint32_t j = 0; j < codesize && j < totalsize; j++ )
					decodedMessage[j] ^= key;
				break;

			case 4:
				if( codesize*4 > totalsize )
					logWarn("%s\n", "codesize > totalsize - broken shellcode?");

//				LogSpam("codesize %i totalsize %i", codesize, totalsize);

				for( uint32_t j = 0; j < codesize && (j+1)*4 < totalsize; j++ )
					*(uint32_t *)(decodedMessage+(j*4) ) ^= longkey;
//				g_Nepenthes->getUtilities()->hexdump(l_crit, decodedMessage, totalsize);
				break;
			}

			char *newshellcode = (char *)malloc(len*sizeof(char));
			memset(newshellcode,0x90,len);
			memcpy(newshellcode,preload,preloadSize);

			memcpy(newshellcode+preloadSize+xordecoderSize,decodedMessage,totalsize);

			pcre_free_substring(preload);
			pcre_free_substring(xordecoder);

//			g_Nepenthes->getUtilities()->hexdump(l_crit,(byte *)newshellcode, len);			

			Message *newMessage = new Message((char *)newshellcode, len, (*msg)->getLocalPort(), (*msg)->getRemotePort(),
				   (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());

			delete *msg;

			*msg = newMessage;

			free(decodedMessage);
			free(newshellcode);
			return SCH_REPROCESS;
		}

	}
	return SCH_NOTHING;
}




