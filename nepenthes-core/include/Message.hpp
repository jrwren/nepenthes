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

#ifndef HAVE_MESSAGE_HPP
#define HAVE_MESSAGE_HPP

#include <time.h>
#include <stdint.h>


namespace nepenthes
{
	class Socket;
	class Responder;



	/**
	 * the Message is our encapsulation class for data we sent and receive.
	 * the deal is, we always have the full context of the Message, 
	 * the socket, and stuff like that
	 */
    class Message
    {

    public:
        Message(char *msg, uint32_t len, uint32_t localport, uint32_t remoteport, 
                uint32_t localhost, uint32_t remotehost, Responder *responder, Socket *socket); // standard msg
        Message(uint32_t localport, uint32_t remoteport, uint32_t localhost, 
                uint32_t remotehost, Responder *responder, Socket *socket); // timeout msg

        virtual ~Message();

        virtual char            *getMsg();
        virtual uint32_t    getSize();
        virtual uint32_t   getLocalHost();
        virtual uint32_t    getLocalPort();
        virtual uint32_t   getRemoteHost();
        virtual uint32_t    getRemotePort();
        virtual time_t          getReceiveTime();
        virtual Socket          *getSocket();
        virtual Responder       *getResponder();

    private:
        char         *m_Msg;
        uint32_t  m_MsgLen;

        uint32_t m_RemoteHost;
        uint32_t  m_RemotePort;

        uint32_t m_LocalHost;
        uint32_t  m_LocalPort;

        time_t        m_ReceiveTime;

        Responder *m_Reply;
        Socket    *m_Socket;

    };


}


#endif
