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

#include <netinet/in.h>

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_asn1_iis.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

ASN1IISBase64::ASN1IISBase64(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "ASN1IISBase64";
	m_ShellcodeHandlerDescription = "handles oc192 dcom bindshell";
	m_pcre = NULL;
}

ASN1IISBase64::~ASN1IISBase64()
{

}

bool ASN1IISBase64::Init()
{
	logPF();
	const char *oc192bindpcre =	"GET.*Authorization.*Negotiate (.*)";
//								"GET.*Authorization: Negotiate YIIQeg"



//	logInfo("pcre is %s \n",oc192bindpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(oc192bindpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("ASN1IISBase64 could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				oc192bindpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool ASN1IISBase64::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result ASN1IISBase64::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getSize());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getSize();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

//	(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);




	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
		logInfo("Found ASN1Base64 .. %i\n",len);
//		g_Nepenthes->getUtilities()->hexdump((unsigned char *)shellcode,len);
		const char * pCode;

		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &pCode);

		// this is bullshit, we have to add some stuff to the pcre so it only takes alphanumerics base64 style
		unsigned char *decoded = g_Nepenthes->getUtilities()->b64decode_alloc((unsigned char *)pCode);
		uint32_t decodedsize = 3*((strlen(pCode)+3)/4);
//		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(unsigned char *)decoded,decodedsize);

		
		pcre_free_substring(pCode);

		Message *nmsg;
		nmsg = new Message((char *)decoded, decodedsize, (*msg)->getLocalPort(), (*msg)->getRemotePort(),
			   (*msg)->getLocalHost(), (*msg)->getRemoteHost(), (*msg)->getResponder(), (*msg)->getSocket());
		delete *msg;
		*msg = nmsg;
		free(decoded); 
		return SCH_REPROCESS;
	}
	return SCH_NOTHING;
}
