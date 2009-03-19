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

/* vuln-ftp created by Harald Lampesberger, contact harald.lampesberger@fork.at 
 * thx to the developers of nepenthes for the help! */

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

using namespace std;

namespace nepenthes
{

	class Buffer;

        typedef enum
        {
		FTP_NULL,
		FTP_USER,
                FTP_PASS,
		FTP_DONE
        } ftp_state;
	
	typedef enum {
		FREEFTPD,
		WARFTPD_USER,
		WARFTPD_PASS,
		UNKNOWN	
	} ftp_exploit;
	
	class FTPd : public Module , public DialogueFactory
	{
	public:
		FTPd(Nepenthes *);
		~FTPd();
		Dialogue *createDialogue(Socket *socket);
		bool Init();
		bool Exit();
	};

	class FTPdDialogue : public Dialogue
	{
	public:
		FTPdDialogue(Socket *socket);
		~FTPdDialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
		
		void dump();
		ftp_exploit identExploit(string line);
		const char *identExploitString(ftp_exploit);
	protected:
		Buffer *m_Buffer;
		Buffer *m_Shellcode;
		ftp_state m_state;		
	};

}
extern nepenthes::Nepenthes *g_Nepenthes;
