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

#include "Event.hpp"

namespace nepenthes
{
	class Socket;

	class SocketEvent : public Event
	{
	public:
		SocketEvent(Socket *socket, uint32_t e)
		{
			m_Socket = socket;
			m_EventType = e;
		}
		~SocketEvent()
		{
		}
		virtual Socket *getSocket()
		{
			return m_Socket;
		}
	private:
		Socket *m_Socket;
	};




	class Message;

	class MessageEvent: public Event
	{
	public:
		MessageEvent(Message *msg, uint32_t e)
		{
			m_Message = msg;
			m_EventType = e;
		}
		virtual ~MessageEvent()
		{
		}
		virtual Message *getMessage()
		{
			return m_Message;
		}
	private:
		Message *m_Message;
	};



	class ShellcodeHandler;

	class ShellcodeEvent : public Event
	{
	public:
		ShellcodeEvent(Socket *socket, ShellcodeHandler *handler, uint32_t e)
		{
			m_Socket = socket;
			m_SCHandler = handler;
			m_EventType = e;
		}
		~ShellcodeEvent()
		{
		}
		virtual Socket *getSocket()
		{
			return m_Socket;
		}
		virtual ShellcodeHandler *getShellcodeHandler()
		{
			return m_SCHandler;
		}
	private:
		Socket *m_Socket;
		ShellcodeHandler *m_SCHandler;
	};



	class Dialogue;

	class DialogueEvent : public Event
	{
	public:
		DialogueEvent(Socket *socket, Dialogue *dia, uint32_t e)
		{
			m_Socket = socket;
			m_Dialogue = dia;
			m_EventType = e;
		}

		~DialogueEvent()
		{
		}

		virtual Socket *getSocket()
		{
			return m_Socket;
		}

		virtual Dialogue *getDialogue()
		{
			return m_Dialogue;
		}
	private:
		Socket *m_Socket;
		Dialogue *m_Dialogue;
	};


}
