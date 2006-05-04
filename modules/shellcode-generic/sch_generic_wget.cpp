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


 
#include "sch_generic_wget.hpp"
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


Genericwget::Genericwget(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "Genericwget";
	m_ShellcodeHandlerDescription = "generic wget decoder";
	m_pcre = NULL;

}

Genericwget::~Genericwget()
{

}

bool Genericwget::Init()
{
	const char *urlpcre = ".*(wget.*)$";
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(urlpcre, PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL)
	{
		logCrit("Genericwget could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				urlpcre, pcreEerror, pcreErrorPos);
		return false;
	}

	return true;
}

bool Genericwget::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;
}

sch_result Genericwget::handleShellcode(Message **msg)
{
	logPF();
	unsigned char *shellcode = (unsigned char *)(*msg)->getMsg();
	uint32_t len = (*msg)->getSize();
	int32_t piOutput[10 * 3];
	int32_t iResult=0;

	if((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, (int *)piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
		const char * pUrl;

		pcre_get_substring((char *) shellcode, (int *)piOutput, (int)iResult, 1, &pUrl);

		logInfo("Detected generic wget Shellcode: \"%s\"\n", pUrl);

		string htmlenc(pUrl);
		string htmldec;

		pcre_free_substring(pUrl);
		
		uint32_t i=0;
		uint32_t j=0;
		for ( i=0;i<htmlenc.size();i++,j++ )
		{
			if ( htmlenc[i] == '%' )
			{
				if (i+3 <= htmlenc.size())
				{
					string num = htmlenc.substr(i+1,2);
					
					byte thisbyte = (byte)strtol(num.c_str(),NULL,16);
//					printf("decoding %s -> %x\n",num.c_str(),(int)thisbyte);
					i +=2;
					htmldec += thisbyte;
				}
			} else
			{
				htmldec += htmlenc[i];
			}
		}

		i=4;
		uint32_t start=0;
		uint32_t stopp=0;


		while (htmldec[i] == ' ')
		{
			i++;
		}

		start = i;

		while (htmldec[i] != '&' &&
			   htmldec[i] != ';')
		{
			i++;
		}
		stopp = i;

		string url = htmldec.substr(start,stopp-start);

		

		

		if ( (int)url.find("://",0) == -1 )
		{
			url = "http://" + url;
		}

		logSpam("url %s\n",url.c_str());

		for (i=0;i<url.size();i++)
		{
			if (isprint(url[i]) == false)
			{
				logWarn("wget url contained unprintable chars \n");
				return SCH_NOTHING;
			}
		}

		g_Nepenthes->getDownloadMgr()->downloadUrl((*msg)->getLocalHost(),(char *)url.c_str(),(*msg)->getRemoteHost(),"generic wget decoder",0);

		
		return SCH_DONE;
	}
    return SCH_NOTHING;
}
