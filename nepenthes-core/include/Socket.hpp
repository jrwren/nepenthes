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

#ifndef HAVE_SOCKET_HPP
#define HAVE_SOCKET_HPP

#ifdef WIN32
#define socklen_t int32_t
#endif

#include <sys/socket.h>


#include <list>
#include <string>
using namespace std;


#include "Responder.hpp"



#define ST_BIND 	0x00001
#define ST_ACCEPT	0x00002
#define ST_CONNECT	0x00004

#define ST_TCP		0x00010 // tcp socket
#define ST_UDP		0x00020 // udp socket
#define ST_UDS		0x00040	// unix domain socket
#define ST_RAW		0x00080	// raw socket
#define ST_POLL		0x00100	// pollonly socket
#define ST_FILE		0x00200 // open a file (/dev/urandom) and check for readability
#define ST_NODEL	0x00400 // dont delete this socket

#define ST_RAW_UDP  0x00800
#define ST_RAW_TCP  0x01000

typedef enum
{
	SS_CONNECTING,
	SS_TIMEOUT,
	SS_RECONNECT,
	SS_CLOSED,		// intended to use with udp&tftp
	SS_CLEANQUIT,	// dont allow any more writing on the socket, if the send que is empty, close socket and set status to SS_CLOSED
	SS_NULL			// cool sockets without problems
} socket_state;

namespace nepenthes
{
	class DialogueFactory;
	class Dialogue;
	class Socket;
	class Nepenthes;


    class Socket : public Responder 
    {
    public:
        virtual ~Socket (){};

        virtual bool addDialogueFactory(DialogueFactory *diaf);
        virtual bool addDialogue(Dialogue *dia);

        virtual bool Init()=0;
        virtual bool Exit()=0;

        virtual bool connectHost()=0;
        virtual bool bindPort()=0; 
        virtual Socket* acceptConnection()=0;

		virtual bool wantSend()=0;

        virtual int32_t doSend()=0;
        virtual int32_t doRecv()=0;

        virtual int32_t doWrite(char *msg,uint32_t len)=0;

        virtual bool checkTimeout()=0;
		virtual bool handleTimeout()=0;

		/**
		 * get a description of the socket
		 * 
		 * @return a string formated like this
		 *         (tcp|udp|raw|uds|poll) (bind|connect|accept) ip:port <-> ip:port
		 */
		virtual string getDescription();

        virtual int32_t   getStatus();
        virtual void  setStatus(socket_state i);
        virtual void  setPolled();
        virtual void  unsetPolled();
        virtual bool  isPolled();

        virtual int32_t   getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen);


        virtual int32_t   getSocket();
        virtual void  setSocket(int32_t i);

        

        virtual int32_t   getType();

        virtual int32_t   getLocalPort();
        virtual int32_t   getRemotePort();
        virtual void  setLocalPort(int32_t i);
        virtual void  setRemotePort(int32_t i);


        virtual void          setRemoteHost(uint32_t i);
        virtual void          setLocalHost(uint32_t i);
        virtual uint32_t getLocalHost();
        virtual uint32_t getRemoteHost();
        virtual list <DialogueFactory *>   * getFactories();
        virtual list <Dialogue *>          * getDialogst();

        virtual time_t getBindTimeout();
        virtual time_t getTimeout();

        virtual Nepenthes *getNepenthes();



        virtual bool isAccept();
        virtual bool isConnect();
        virtual bool isBind();

    protected:
        list <DialogueFactory *>    m_DialogueFactories;
        list <Dialogue *>           m_Dialogues;


        int32_t       		m_ReconnectMax;
        int32_t       		m_ReconnectTries;

        uint32_t	m_Type;     // udp / tcp // bind / connect / accept
        int32_t       		m_Socket;

        socket_state 	m_Status;

        uint32_t    m_RemoteHost;
        uint32_t    m_RemotePort;
        string          m_RemoteHostString;

        uint32_t    m_LocalHost;
        uint32_t    m_LocalPort;
        string          m_LocalHostString;

        time_t 		m_TimeoutIntervall;        // intervall between time(NULL) and m_tLastSocketAction
        time_t 		m_BindTimeoutIntervall;      // bind()' sockets can have a different timeout than their childs
        time_t 		m_LastAction;

        bool 		m_Polled;
        bool 		m_CanSend;

        Nepenthes   *m_Nepenthes;

    };



}


#endif

