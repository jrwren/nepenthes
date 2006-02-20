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

#include <ctype.h>

#include "SMBNameDialogue.hpp"
#include "vuln-netbiosname.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"


#include "Message.hpp"


#include "ShellcodeManager.hpp"

#include "Utilities.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;
/*
FIXME
[ module ] =------------------[ hexdump(0x08096a18 , 0x000000d1) ]-------------------=
[ module ] 0x0000  81 00 00 44 20 43 4b 46  44 45 4e 45 43 46 44 45  ...D CKF DENECFDE
[ module ] 0x0010  46 46 43 46 47 41 41 41  41 41 41 41 41 41 41 41  FFCFGAAA AAAAAAAA
[ module ] 0x0020  41 41 41 41 41 00 20 45  4a 45 43 45 4e 43 41 43  AAAAA. E JECENCAC
[ module ] 0x0030  41 43 41 43 41 43 41 43  41 43 41 43 41 43 41 43  ACACACAC ACACACAC
[ module ] 0x0040  41 43 41 43 41 41 41 00  00 00 00 85 ff 53 4d 42  ACACAAA. .....SMB
[ module ] 0x0050  72 00 00 00 00 18 53 c8  00 00 00 00 00 00 00 00  r.....S. ........
[ module ] 0x0060  00 00 00 00 00 00 ff fe  00 00 00 00 00 62 00 02  ........ .....b..
[ module ] 0x0070  50 43 20 4e 45 54 57 4f  52 4b 20 50 52 4f 47 52  PC NETWO RK PROGR
[ module ] 0x0080  41 4d 20 31 2e 30 00 02  4c 41 4e 4d 41 4e 31 2e  AM 1.0.. LANMAN1.
[ module ] 0x0090  30 00 02 57 69 6e 64 6f  77 73 20 66 6f 72 20 57  0..Windo ws for W
[ module ] 0x00a0  6f 72 6b 67 72 6f 75 70  73 20 33 2e 31 61 00 02  orkgroup s 3.1a..
[ module ] 0x00b0  4c 4d 31 2e 32 58 30 30  32 00 02 4c 41 4e 4d 41  LM1.2X00 2..LANMA
[ module ] 0x00c0  4e 32 2e 31 00 02 4e 54  20 4c 4d 20 30 2e 31 32  N2.1..NT  LM 0.12
[ module ] 0x00d0  00                                                .
[ module ] =-------------------------------------------------------------------------=
*/

char smb_session_req0[] = {
0x81, 0x00, 0x00, 0x44, 0x20, 0x43, 0x4b, 0x46, 
0x44, 0x45, 0x4e, 0x45, 0x43, 0x46, 0x44, 0x45, 
0x46, 0x46, 0x43, 0x46, 0x47, 0x45, 0x46, 0x46, 
0x43, 0x43, 0x41, 0x43, 0x41, 0x43, 0x41, 0x43, 
0x41, 0x43, 0x41, 0x43, 0x41, 0x00, 0x20, 0x45, 
0x4b, 0x45, 0x44, 0x46, 0x45, 0x45, 0x49, 0x45, 
0x44, 0x43, 0x41, 0x43, 0x41, 0x43, 0x41, 0x43, 
0x41, 0x43, 0x41, 0x43, 0x41, 0x43, 0x41, 0x43, 
0x41, 0x43, 0x41, 0x43, 0x41, 0x41, 0x41, 0x00 };
char smb_session_reply0[] = {
0x82, 0x00, 0x00, 0x00 };
char smb_negotiate_req0[] = {
0x00, 0x00, 0x00, 0x2f, 0xff, 0x53, 0x4d, 0x42, 
0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x02, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x02, 
0x4e, 0x54, 0x20, 0x4c, 0x4d, 0x20, 0x30, 0x2e, 
0x31, 0x32, 0x00 };
char smb_negotiate_reply0[] = {
0x00, 0x00, 0x00, 0x7d, 0xff, 0x53, 0x4d, 0x42, 
0x72, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x02, 
0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x03, 
0x0a, 0x00, 0x01, 0x00, 0x04, 0x11, 0x00, 0x00, 
0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xfd, 0xe3, 0x00, 0x00, 0x02, 0x9f, 0x77, 0x53, 
0x50, 0x86, 0xc5, 0x01, 0x88, 0xff, 0x08, 0x38, 
0x00, 0x1e, 0xd1, 0x98, 0x18, 0x9e, 0x40, 0x7e, 
0x94, 0x41, 0x00, 0x52, 0x00, 0x42, 0x00, 0x45, 
0x00, 0x49, 0x00, 0x54, 0x00, 0x53, 0x00, 0x47, 
0x00, 0x52, 0x00, 0x55, 0x00, 0x50, 0x00, 0x50, 
0x00, 0x45, 0x00, 0x00, 0x00, 0x46, 0x00, 0x4f, 
0x00, 0x4f, 0x00, 0x42, 0x00, 0x41, 0x00, 0x52, 
0x00, 0x2d, 0x00, 0x30, 0x00, 0x31, 0x00, 0x00, 
0x00 };



/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the SMBNameDialogue, creates a new SMBNameDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
SMBNameDialogue::SMBNameDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "SMBNameDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_Buffer = new Buffer(512);

	m_State = SMBName_NULL;
}

SMBNameDialogue::~SMBNameDialogue()
{
	switch (m_State)
	{

	case SMBName_NEGOTIATE:
	case SMBName_NULL:
		logWarn("Unknown SMBName exploit %i bytes State %i\n",m_Buffer->getSize(), m_State);
//		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte *) m_Buffer->getData(), m_Buffer->getSize());
		break;


	case SMBName_DONE:
		break;
	}

	delete m_Buffer;
}

typedef struct
{
	unsigned char 	m_RequestType;
	unsigned char 	m_Flags;
	uint16_t 	m_Length;
} smb_header;

/**
 * Dialogue::incomingData(Message *)
 * 
 * a small and ugly shell where we can use
 * "download protocol://localction:port/path/to/file
 * to trigger a download
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel SMBNameDialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getSize());

	switch (m_State)
	{
	case SMBName_NULL:
		{
			char *buffer = (char *)m_Buffer->getData();
			smb_header *sh = (smb_header *)buffer;
			buffer = buffer + 3;

			if (sh->m_RequestType == 0x81)
			{
				m_State = SMBName_NEGOTIATE;
				uint32_t len;
                len = ntohs(sh->m_Length) + sizeof(smb_header);
				logInfo("%i %i \n",len,sizeof(smb_header));
				logInfo("SMB Session Request %i\n\%.*s\n",m_Buffer->getSize(),ntohs(sh->m_Length),buffer);
//				m_Buffer->cut(len);
				m_Buffer->clear();
			}
			

/*		if ( m_Buffer->getSize()== sizeof(smb_session_req0) &&
			 memcmp(m_Buffer->getData(),smb_session_req0,sizeof(smb_session_req0)) == 0 )
		{
			logSpam("SMB Session request %i %i\n", m_Buffer->getSize(),sizeof(smb_session_req0));
			msg->getResponder()->doRespond(smb_session_reply0,sizeof(smb_session_reply0));
			m_State = SMBName_NEGOTIATE;
			m_Buffer->cut(sizeof(smb_session_req0));
		}else
		{
			return CL_DROP;
		}
*/		
		}
		break;

	case SMBName_NEGOTIATE:
		if ( m_Buffer->getSize()== sizeof(smb_negotiate_req0) &&
			 memcmp(m_Buffer->getData(),smb_negotiate_req0,sizeof(smb_negotiate_req0)) == 0 )
		{
			logSpam("SMB Negotiate request %i\n", m_Buffer->getSize());
			msg->getResponder()->doRespond(smb_negotiate_reply0,sizeof(smb_negotiate_reply0));
			m_State = SMBName_DONE;
			m_Buffer->cut(sizeof(smb_negotiate_req0));
		}else
		{
			return CL_DROP;
		}
		break;


	case SMBName_DONE:
		break;
	}
	

	

	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel SMBNameDialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

/**
 * Dialogue::handleTimeout(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel SMBNameDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionLost(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel SMBNameDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel SMBNameDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}


