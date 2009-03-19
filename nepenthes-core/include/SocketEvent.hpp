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
#include "Message.hpp"

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
		SocketEvent(Socket *parent, Socket *child, uint32_t e)
		{
			m_Socket = child;
			m_EventType = e;
			m_ParentSocket = parent;
		}
		~SocketEvent()
		{
		}
		virtual Socket *getParentSocket()
		{
			return m_ParentSocket;
		}
		virtual Socket *getSocket()
		{
			return m_Socket;
		}
	private:
		Socket *m_ParentSocket;
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
		// ShellcodeEvent(Socket *socket, ShellcodeHandler *handler, const char *trigger, bool known, uint32_t e)
		ShellcodeEvent(Message *msg, ShellcodeHandler *handler, const char *trigger, bool known, uint32_t e)
		{
			// m_Socket = socket;
			m_Message = msg;
			m_SCHandler = handler;
			m_Trigger = trigger;
			m_Known = known;
			m_EventType = e;
		}
		~ShellcodeEvent()
		{
		}
		virtual bool getKnown ()
		{
			return m_Known;
		}
		virtual Message *getMessage ()
		{
			return m_Message;
		}
		virtual Socket *getSocket()
		{
			return m_Message->getSocket();
		}
		virtual ShellcodeHandler *getShellcodeHandler()
		{
			return m_SCHandler;
		}
		virtual const char * getTrigger ()
		{
			return m_Trigger;
		}

	private:
		// Socket *m_Socket;
		Message *m_Message;
		ShellcodeHandler *m_SCHandler;
		const char *m_Trigger;
		bool m_Known;
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

#ifdef HAVE_DEBUG_LOGGING
#define HEXDUMP(socket,data,size)							\
{															\
	HexdumpEvent *he = new HexdumpEvent(socket,data,size); 	\
	g_Nepenthes->getEventMgr()->handleEvent(he);			\
	delete he;												\
}															
#else	// HAVE_DEBUG_LOGGING
#define HEXDUMP(socket,data,size)
#endif	// HAVE_DEBUG_LOGGING



	class HexdumpEvent : public Event
	{
	public:
		HexdumpEvent(Socket *s, void *data, uint32_t size)
		{
			m_EventType = EV_HEXDUMP;
			m_Socket = s;
			m_Size = size;
			m_Data = data;
		}

		virtual Socket *getSocket()
		{
			return m_Socket;
		}

		virtual void *getData()
		{
			return m_Data;
		}

		virtual uint32_t getSize()
		{
			return m_Size;
		}

	private:
		Socket 		*m_Socket;
		void 		*m_Data;
		uint32_t	m_Size;
	};


}
