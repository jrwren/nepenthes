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
#define EV_SOCK_TCP_CONNECT 	3
#define EV_SOCK_TCP_CLOSE 		4
#define EV_SOCK_TCP_RX 			5
#define EV_SOCK_TCP_TX 			6

#define EV_SOCK_UDP_BIND 		7
#define EV_SOCK_UDP_ACCEPT 		8
#define EV_SOCK_UDP_CONNECT 	9
#define EV_SOCK_UDP_CLOSE 		10

#define EV_SOCK_UDS_BIND 		11
#define EV_SOCK_UDS_ACCEPT 		12
#define EV_SOCK_UDS_CONNECT 	13
#define EV_SOCK_UDS_CLOSE 		14

#define EV_SOCK_RAW_BIND 		15
#define EV_SOCK_RAW_ACCEPT 		16
#define EV_SOCK_RAW_CONNECT 	17
#define EV_SOCK_RAW_CLOSE 		18

#define EV_DOWNLOAD 			19

#define EV_SUBMISSION 			20
#define EV_SUBMISSION_UNIQ 		21
#define EV_SUBMISSION_HIT 		22

#define EV_DIALOGUE_ASSIGN_AND_DONE 23

#define EV_SHELLCODE_DONE		24

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
