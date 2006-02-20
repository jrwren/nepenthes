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
		SocketEvent(Socket *socket, event_type e)
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
		MessageEvent(Message *msg, event_type e)
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

}
