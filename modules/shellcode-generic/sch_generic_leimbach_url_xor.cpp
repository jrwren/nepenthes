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

#include "sch_generic_xor.hpp"
#include "sch_generic_leimbach_url_xor.hpp"


#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "Utilities.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

#define XF_NONE 			0x00001
#define XF_SIZE_INVERT 		0x00002
#define XF_INVERSE_ORDER 	0x00004

#include <cstring>

using namespace nepenthes;

LeimbachUrlXORXOR::LeimbachUrlXORXOR(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "LeimbachUrlXOR";
	m_ShellcodeHandlerDescription = "generic XOR decoder";
    
}

LeimbachUrlXORXOR::~LeimbachUrlXORXOR()
{
}

bool LeimbachUrlXORXOR::Init()
{
	const char * pcreEerror;
	int32_t pcreErrorPos;

	XORPcreHelper test[1]=
	{
		{
			"(.*)(\\xE9\\xBF\\x00\\x00\\x00\\x5F\\x64\\xA1\\x30\\x00\\x00\\x00\\x8B\\x40\\x0C\\x8B\\x70\\x1C\\xAD\\x8B\\x68\\x08\\x8B\\xF7\\x6A\\x03\\x59\\xE8\\x5F\\x00\\x00\\x00\\xE2\\xF9\\x68\\x6F\\x6E\\x00\\x00\\x68\\x75\\x72\\x6C\\x6D\\x54\\xFF\\x16\\x8B\\xE8\\xE8\\x49\\x00\\x00\\x00\\x8B\\xFE\\x83\\xC7\\x10\\x57\\x80\\x37(.)\\x47\\x80\\x3F(.)\\x75\\xF7\\x80\\x37\\x11\\x5F\\x83\\xEC\\x14\\x68\\x65\\x78\\x65\\x00\\x68\\x6F\\x73\\x74\\x2E\\x68\\x73\\x76\\x63\\x68\\x68\\x65\\x72\\x73\\x5C\\x68\\x64\\x72\\x69\\x76\\x8B\\xDC\\x33\\xC0\\x50\\x50\\x53\\x57\\x50\\xFF\\x56\\x0C\\x85\\xC0\\x75\\x07\\x8B\\xDC\\x50\\x53\\xFF\\x56\\x04\\xFF\\x56\\x08\\x51\\x56\\x8B\\x45\\x3C\\x8B\\x54\\x28\\x78\\x03\\xD5\\x52\\x8B\\x72\\x20\\x03\\xF5\\x33\\xC9\\x49\\x41\\xAD\\x03\\xC5\\x33\\xDB\\x0F\\xBE\\x10\\x3A\\xD6\\x74\\x08\\xC1\\xCB\\x0D\\x03\\xDA\\x40\\xEB\\xF1\\x3B\\x1F\\x75\\xE7\\x5A\\x8B\\x5A\\x24\\x03\\xDD\\x66\\x8B\\x0C\\x4B\\x8B\\x5A\\x1C\\x03\\xDD\\x8B\\x04\\x8B\\x03\\xC5\\xAB\\x5E\\x59\\xC3\\xE8\\x3C\\xFF\\xFF\\xFF................)(.*)$",
			"leimbach url xor",
			XF_NONE
		}
	};

	for( uint32_t i = 0; i <= 0; i++ )
	{
		pcre *mypcre;
		if((mypcre = pcre_compile(test[i].m_PCRE, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
		{
			logCrit("LeimbachUrlXORXOR could not compile pattern %i\n\t\"%s\"\n\t Error:\"%s\" at Position %u", i,
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

bool LeimbachUrlXORXOR::Exit()
{
	while(m_Pcres.size()>0)
	{

		pcre_free(m_Pcres.front()->m_Pcre);
		delete m_Pcres.front();
		m_Pcres.pop_front();
	}
    	
	return true;

}

sch_result LeimbachUrlXORXOR::handleShellcode(Message **msg)
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
			uint32_t keysize;
			uint32_t codesize = 0, codesizeLen, totalsize;
			byte stopchar=0;

			keysize = pcre_get_substring((char *) shellcode, (int *)output, (int)result, 3, &match);
			switch ( keysize )
			{
			
			case 1:
				key = *((byte *)match);
				break;
			}
			pcre_free_substring(match);

			codesizeLen = pcre_get_substring((char *) shellcode, (int *)output, (int)result, 4, &match);
			switch ( keysize )
			{

			case 1:
				stopchar = *((byte *)match);
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
				uint32_t j;
            	for( j = 0; decodedMessage[j] != stopchar && j < totalsize; j++ )
				{
                	decodedMessage[j] ^= key;
				}
				if (j < totalsize)
				{
					decodedMessage[j] ^= stopchar;
				}
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




