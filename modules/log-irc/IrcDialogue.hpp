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

using namespace std;

namespace nepenthes
{
	typedef struct
	{
		unsigned char ucVersion;
		unsigned char ucCommand;
		unsigned short usDestPort;
		unsigned long ulDestAddr;
		char szUser[1024];

	} socks4_header_t;


	typedef enum 
	{
		IRCDIA_REQUEST_SEND,
		IRCDIA_CONNECTED,
	} irc_dia_state;

	class LogIrc;
	class Buffer;

	class IrcDialogue : public Dialogue
	{
	public:
		IrcDialogue(Socket *socket, LogIrc * logirc);
		~IrcDialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);

		void 	logIrc(unsigned int mask, const char *message);

	protected:

		void processBuffer();
		void processLine(string *line);
		bool m_Pinged;
		LogIrc 	*m_LogIrc;

		irc_dia_state m_State;

		string 	m_NickName;

		Buffer 	*m_Buffer;
	};

}

