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

#include <stdint.h>

namespace nepenthes
{

	class Socket;

#define EV_TIMEOUT 				0

#define EV_SOCK_TCP_BIND 		1
#define EV_SOCK_TCP_ACCEPT 		2
#define EV_SOCK_TCP_ACCEPT_STOP		3
#define EV_SOCK_TCP_CONNECT		4
#define EV_SOCK_TCP_CONNECT_REQ		5
#define EV_SOCK_TCP_CLOSE 		6
#define EV_SOCK_TCP_RX 			7
#define EV_SOCK_TCP_RX_STOP		8
#define EV_SOCK_TCP_TX 			9

#define EV_SOCK_UDP_BIND 		11
#define EV_SOCK_UDP_ACCEPT 		12
#define EV_SOCK_UDP_CONNECT		13
#define EV_SOCK_UDP_CLOSE 		14
#define EV_SOCK_UDP_RX			15
#define EV_SOCK_UDP_TX			16

#define EV_SOCK_UDS_BIND 		21
#define EV_SOCK_UDS_ACCEPT 		22
#define EV_SOCK_UDS_CONNECT 	23
#define EV_SOCK_UDS_CLOSE 		24
#define EV_SOCK_UDS_RX          25
#define EV_SOCK_UDS_TX          26



#define EV_SOCK_RAW_BIND 		31
#define EV_SOCK_RAW_ACCEPT 		32
#define EV_SOCK_RAW_CONNECT 	33
#define EV_SOCK_RAW_CLOSE 		34
#define EV_SOCK_RAW_RX          35
#define EV_SOCK_RAW_TX          36



#define EV_DOWNLOAD 			41
#define EV_DOWNLOAD_DESTROYED		42

#define EV_SUBMISSION 			51
#define EV_SUBMISSION_UNIQ 		52
#define EV_SUBMISSION_HIT 		53
#define EV_SUBMISSION_DROPPED		54

#define EV_DIALOGUE_ASSIGN_AND_DONE 61

#define EV_SHELLCODE			70
#define EV_SHELLCODE_DONE		71
#define EV_SHELLCODE_FAIL		72

#define EV_HEXDUMP				81

#define EV_DNS_QUERY_CREATED		90
#define EV_DNS_QUERY_FAILURE		91
#define EV_DNS_QUERY_SUCCESS		92
#define EV_DNS_QUERY_STOP		93
#define EV_DNS_QUERY_DESTROYED		94

	class Event
    {
    public:
        virtual ~Event(){};
		virtual uint32_t getType()
		{
			return m_EventType;
		};


    protected:
		uint32_t 	m_EventType;
    };
}

#endif
