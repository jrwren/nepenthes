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


namespace nepenthes
{
	class Socket;
	class Responder;



    class Message
    {

    public:
        Message(char *msg, unsigned int len, unsigned int localport, unsigned int remoteport, 
                unsigned long localhost, unsigned long remotehost, Responder *responder, Socket *socket); // standard msg
        Message(unsigned int localport, unsigned int remoteport, unsigned long localhost, 
                unsigned long remotehost, Responder *responder, Socket *socket); // timeout msg

        virtual ~Message();

        virtual char            *getMsg();
        virtual unsigned int    getMsgLen();
        virtual unsigned long   getLocalHost();
        virtual unsigned int    getLocalPort();
        virtual unsigned long   getRemoteHost();
        virtual unsigned int    getRemotePort();
        virtual time_t          getReceiveTime();
        virtual Socket          *getSocket();
        virtual Responder       *getResponder();

    private:
        char         *m_Msg;
        unsigned int  m_MsgLen;

        unsigned long m_RemoteHost;
        unsigned int  m_RemotePort;

        unsigned long m_LocalHost;
        unsigned int  m_LocalPort;

        time_t        m_ReceiveTime;

        Responder *m_Reply;
        Socket    *m_Socket;

    };


}


#endif
