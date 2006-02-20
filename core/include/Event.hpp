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

#ifndef HAVE_EVENT_HPP
#define HAVE_EVENT_HPP

namespace nepenthes
{

	class Socket;

	typedef enum
	{
// generic events		
		EV_TIMEOUT=0,

//	tcp	events
		EV_SOCK_TCP_BIND,
		EV_SOCK_TCP_ACCEPT,
		EV_SOCK_TCP_CONNECT,
		EV_SOCK_TCP_CLOSE,
//	udp	events
		EV_SOCK_UDP_BIND,
		EV_SOCK_UDP_ACCEPT,
		EV_SOCK_UDP_CONNECT,
		EV_SOCK_UDP_CLOSE,


//	unix domain socket events
		EV_SOCK_UDS_BIND,
		EV_SOCK_UDS_ACCEPT,
		EV_SOCK_UDS_CONNECT,
		EV_SOCK_UDS_CLOSE,

// 	raw socket events
		EV_SOCK_RAW_BIND,
		EV_SOCK_RAW_ACCEPT,
		EV_SOCK_RAW_CONNECT,
		EV_SOCK_RAW_CLOSE,

// 	download events
		EV_DOWNLOAD,


// 	submission events
		EV_SUBMISSION,
		EV_SUBMISSION_UNIQ,
		EV_SUBMISSION_HIT,




		EV_NULL
	} event_type;

	

	class Event
    {
    public:
        virtual ~Event(){};
		virtual event_type getType()
		{
			return m_EventType;
		};


    protected:
		event_type 	m_EventType;
    };
}

#endif
