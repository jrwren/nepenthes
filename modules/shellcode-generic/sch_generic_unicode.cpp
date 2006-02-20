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


#include "sch_generic_unicode.hpp"
#include "LogManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"


#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr


using namespace nepenthes;


GenericUniCode::GenericUniCode(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "GenericUniCode";
	m_ShellcodeHandlerDescription = "generic UniCode decoder";
}

GenericUniCode::~GenericUniCode()
{

}

bool GenericUniCode::Init()
{
	return true;
}

bool GenericUniCode::Exit()
{
	return true;
}

sch_result GenericUniCode::handleShellcode(Message **msg)
{
	logPF();
	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	uint32_t i;

	uint32_t uni=0;
	uint32_t maxuni=0;

	uint32_t start=0;//,stopp;
	uint32_t maxstart=0;
	uint32_t maxstopp=0;


	for ( i=0;i<len;i+=2 )
	{
		if ( shellcode[i] == 0 )
		{
			if ( uni == 0 )
			{
				start = i;
			}
			uni++;
		} else
		{
			if ( uni > maxuni )
			{
				maxstart = start;
				maxstopp = i;
				maxuni = uni;
			}

			uni=0;
		}
	}

	for ( i=1;i<len;i+=2 )
	{
		if ( shellcode[i] == 0 )
		{
			if ( uni == 0 )
			{
				start = i;
			}
			uni++;
		} else
		{
			if ( uni > maxuni )
			{
				maxstart = start;
				maxstopp = i;
				maxuni = uni;
			}
			uni=0;
		}
	}


	

	if ( maxuni > 2000 )
	{
		logInfo("Got unicode Exploit %i 00  %i -> %i bytes \n",maxuni,maxstart,maxstopp);

		byte *output;
        uint32_t outputLen=0;

		unicodeTryDecode(shellcode, len, &output, &outputLen);

//		g_Nepenthes->getUtilities()->hexdump(l_crit, output, outputLen);			

		Message *newMessage = new Message((char *)output, outputLen, (*msg)->getLocalPort(), (*msg)->getRemotePort(),
										  (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());

		delete *msg;

		*msg = newMessage;

		free(output);
		return SCH_REPROCESS;
	}else
	{
    	return SCH_NOTHING;
	} 
}



uint32_t GenericUniCode::unicodeLength(uint8_t *unicode, uint32_t len)
{
        uint32_t size=0;
		uint32_t lencopy = len;

        uint8_t state = 0;

//        printf("\tlen %i\n",len);
        while( len )
        {
//              printf("\tlen %i state %i size %i \n",len,state, size);
                switch(state)
                {

                        case 0:
                                if( *unicode == 0 )
								{
									state = 1;
								}
                                else
                                {
//                                        printf("\tlen %i state %i size %i \n",len,state, size);
                                        return size;
                                }
                                break;
                        case 1:
                                state = 0;
                                break;
                }
                *unicode++;
                size++;
                len--;
        }
        return lencopy;
}

uint32_t GenericUniCode::unicodeTryDecode(uint8_t *unicode, uint32_t len, uint8_t **decoded, uint32_t *decodedLength)
{
        *decoded = (uint8_t *)malloc(len);
        memset((*decoded),0x90,len);

        uint8_t *destPtr = *decoded;
        *decodedLength = 0;

        while( len )
        {
//                printf("len %i decodedLength %i \n",len, (*decodedLength));
                if( *unicode == 0 )
                {
//                      len = len - unicodeLength(unicode,len);
                        uint32_t size =  unicodeLength(unicode,len);
                        if ( size > 10 )
                        {
//                                printf("\t\tsize %i > 10 \n\n",size);
                                uint32_t i;
                                for (i=0;i<size/2;i++)
                                {
                                        destPtr[i] = unicode[i*2+1];
                                }

                                len = len - size;
                                (*decodedLength) = (*decodedLength) + size/2;
                                destPtr += size/2;
                                unicode += size;

                        }else
                        {
//                                printf("\t\tsize %i <= 10 \n\n",size);
                                len--;
                                (*decodedLength)++;
                                *destPtr = *unicode;
                                destPtr++;
                                unicode++;

                        }


                }
                else
                {
                        len--;
                        (*decodedLength)++;
                        *destPtr = *unicode;
                        destPtr++;
                        unicode++;
                }

//              *unicode++;
        }

        return 0;
}


