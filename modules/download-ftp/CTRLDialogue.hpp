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

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"
#include "DNSCallback.hpp"

using namespace std;

namespace nepenthes
{

	typedef enum 
	{
		FTP_CONNECTED,
		FTP_USER,
		FTP_PASS,
		FTP_TYPE,
		FTP_PORT,
//		FTP_EPASV,
		FTP_RETR,
		FTP_QUIT
	} ftp_down_state;

	class Download;
	class FTPContext;
	class Buffer;

	class CTRLDialogue : public Dialogue
	{
	public:
		CTRLDialogue(Socket *socket, Download *down);
		~CTRLDialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);

        void setContext(FTPContext *context);
		void setDownload(Download *down);


		void  sendUser();
		bool parseUser(char *msg);

		void  sendPass();
		bool parsePass(char *msg);

		void  sendType();
		bool parseType(char *msg);

		void  sendPort();
		bool parsePort(char *msg);

		void  sendRetr();
		bool parseRetr(char *msg);

		void  sendQuit();
		bool parseQuit(char *msg);

	protected:
		Download 	*m_Download;
        FTPContext  *m_Context;
		Buffer		*m_Buffer;

		ftp_down_state m_State;
	};
}
