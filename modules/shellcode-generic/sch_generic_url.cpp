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

#include <stdio.h>
 
#include "sch_generic_url.hpp"
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


GenericUrl::GenericUrl(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "GenericUrl";
	m_ShellcodeHandlerDescription = "generic Url decoder";
	m_pcre = NULL;

	fprintf(stderr,"\n");
	fprintf(stderr,"The generic url shellcodehandler is based on \n");
	fprintf(stderr,"mwcollects generic url shellcodehandler \n");
	fprintf(stderr,"mwcollect is\n"); 
	fprintf(stderr,"Copyright (c) 2005, Honeynet Project\n");
	fprintf(stderr,"All rights reserved.\n");
	fprintf(stderr,"published on a bsd license\n");
	fprintf(stderr,"and written by Georg Wicherski\n");
	fprintf(stderr,"http://www.mwcollect.org for more information about mwcollect\n");
	fprintf(stderr,"\n");


}

GenericUrl::~GenericUrl()
{

}

bool GenericUrl::Init()
{
	const char *urlpcre = ".*((http|https|ftp):\\/\\/[@a-zA-Z0-9\\-\\/\\\\\\.\\+:]+).*";
//	".*((http|https|ftp):\\/\\/[a-zA-Z0-9\\/\\\\\\.\\+:]+).*\\xDF+.*$";
	//"^.*\\xEB.((http|https|ftp):\\/\\/.*?)\\xDF+.*$";
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(urlpcre, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("GenericUrl could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				urlpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool GenericUrl::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;
}

sch_result GenericUrl::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());

	bool bMatch=false;
	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getSize();
	int32_t piOutput[10 * 3];
	int32_t iResult=0;

	if((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
//		HEXDUMP(m_Socket,shellcode,len);
		const char * pUrl;

		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 1, &pUrl);

		logInfo("Detected generic prepended unencoded URL Shellcode: \"%s\"\n", pUrl);
		
		g_Nepenthes->getDownloadMgr()->downloadUrl((*msg)->getLocalHost(),(char *)pUrl,(*msg)->getRemoteHost(),"URL Detected in Shellcode",0);
		pcre_free_substring(pUrl);
		bMatch = true;
	}

	if(bMatch == true)
    	return SCH_DONE;

    return SCH_NOTHING;
}
