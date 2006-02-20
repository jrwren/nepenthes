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

#include "config.h"
#ifdef HAVE_LIBSSH


#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "vuln-ssh.hpp"
#include "SSHDialogue.hpp"
#include "SSHSocket.hpp"


#include "POLLSocket.cpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod



using namespace nepenthes;


/**
 * constructor for bind socket
 * 
 * @param options the ssh socket options
 */

SSHSocket::SSHSocket(SSH_OPTIONS *options)
{
	m_Type = ST_POLL|ST_BIND;

	m_Bind = ssh_bind_new();
	ssh_bind_set_options(m_Bind,options);

	m_Session = NULL;
}


/**
 * constructor for accept socket
 * 
 * @param options the ssh socket options
 */

SSHSocket::SSHSocket(SSH_SESSION *session)
{
	m_Type = ST_POLL|ST_ACCEPT;
	m_Session = session;

	m_SSHState = SSH_STATE_AUTH;

	m_LastAction = time(0);
	m_TimeoutIntervall = 30;

	struct sockaddr_in addrBind;
	int32_t iSize = sizeof(addrBind);
	getsockname(getSocket(), (struct sockaddr *) &addrBind, (socklen_t *) &iSize);
	m_LocalPort = ntohs( ( (sockaddr_in *)&addrBind)->sin_port );
	m_LocalHost = ( (sockaddr_in *)&addrBind)->sin_addr.s_addr;

	getpeername(getSocket(), (struct sockaddr *) &addrBind, (socklen_t *) &iSize);
	m_RemotePort = ntohs( ( (sockaddr_in *)&addrBind)->sin_port );
	m_RemoteHost = ( (sockaddr_in *)&addrBind)->sin_addr.s_addr;



}



SSHSocket::~SSHSocket()
{
	if (m_Session != NULL)
	{
//		ssh_cleanup(m_Session);
		ssh_disconnect(m_Session);
	}
	if (isAccept())
	{
    		logInfo("SSHSession User '%s' Pass '%s'\n",m_User.c_str(),m_Pass.c_str());
			logInfo("SSHSession %s\n",m_WhatToRead.c_str());
			logInfo("SSHSession Ended (%i bytes)\n",m_WhatToRead.size());
			
	}
}

bool SSHSocket::Init()
{
	if(isBind())
	{
        if(!bindPort())
		{
			logCrit("ERROR Could not init Socket %s\n", strerror(errno));
			return false;
		}
		return true;
	}
	return false;
}

bool SSHSocket::bindPort()
{
	if ( ssh_bind_listen(m_Bind) < 0 )
	{
		logCrit("Error listening to socket: %s\n",ssh_get_error(m_Bind));
		return false;
	}else
	{
		struct sockaddr_in addrBind;
		int32_t iSize = sizeof(addrBind);
		getsockname(getSocket(), (struct sockaddr *) &addrBind, (socklen_t *) &iSize);
		m_LocalPort = ntohs( ( (sockaddr_in *)&addrBind)->sin_port );
		m_LocalHost = ( (sockaddr_in *)&addrBind)->sin_addr.s_addr;


		return true;
	}
}


Socket *SSHSocket::acceptConnection()
{
	logPF();
	SSH_SESSION *session;

	if (( session = ssh_bind_accept(m_Bind)) == NULL)
	{
		logCrit("error accepting a connection : %s\n",ssh_get_error(m_Bind));
		return NULL;
	}

    if(ssh_accept(session))
	{
        logCrit("ssh_accept : %s\n",ssh_get_error(session));
		return NULL;
    }

	SSHSocket *socket = new SSHSocket(session);
	return socket;
}

bool SSHSocket::wantSend()
{
	return false;
}

int32_t SSHSocket::doSend()
{
	return 0;
}

int32_t SSHSocket::doRecv()
{
	logPF();
	SSH_MESSAGE *message;

	m_LastAction = time(0);

	switch (m_SSHState)
	{
	case SSH_STATE_AUTH:
		logSpam("%s\n","SSH_STATE_AUTH");
		message=ssh_message_get(m_Session);
		if ( message )
		{
			switch ( ssh_message_type(message) )
			{
			case SSH_AUTH_REQUEST:
				switch ( ssh_message_subtype(message) )
				{
				case SSH_AUTH_PASSWORD:
					m_User = ssh_message_auth_user(message);
					m_Pass = ssh_message_auth_password(message);

					logInfo("SSH User '%s' wants to auth with pass '%s'\n",
						   ssh_message_auth_user(message),
						   ssh_message_auth_password(message));
					if ( 1 )// auth_password(ssh_message_auth_user(message),ssh_message_auth_password(message)) )
					{
						m_SSHState = SSH_STATE_CHANNEL;
						ssh_message_auth_reply_success(message,0);
						break;
					}
					// not authenticated, send default message
				case SSH_AUTH_NONE:
				default:
					ssh_message_auth_set_methods(message,SSH_AUTH_PASSWORD);
					ssh_message_reply_default(message);
					break;
				}
				break;
			default:
				ssh_message_reply_default(message);
			}
			ssh_message_free(message);
		}
		break;


	case SSH_STATE_CHANNEL:
		logSpam("%s\n","SSH_STATE_CHANNEL");
		message=ssh_message_get(m_Session);
		if ( message )
		{
			switch ( ssh_message_type(message) )
			{
			case SSH_CHANNEL_REQUEST_OPEN:
				if ( ssh_message_subtype(message)==SSH_CHANNEL_SESSION )
				{
					m_Channel=ssh_message_channel_request_open_reply_accept(message);
					m_SSHState = SSH_STATE_SHELL_OR_SFTP;
					break;
				}
			default:
				ssh_message_reply_default(message);
			}
			ssh_message_free(message);
		}
		break;

	case SSH_STATE_SHELL_OR_SFTP:
		logSpam("%s\n","SSH_STATE_SHELL_OR_SFTP");
		message=ssh_message_get(m_Session);
		if (message == NULL )
        	break;

		logSpam("MESSAGE_TYPE %i\n",ssh_message_type(message));
		logSpam("MESSAGE_SUBTYPE %i\n",ssh_message_subtype(message));
		if ( message && ssh_message_type(message)==SSH_CHANNEL_REQUEST &&
			 ssh_message_subtype(message)==SSH_CHANNEL_REQUEST_SHELL )
		{
			logDebug("%s\n","SSH_SHELL");
//            if(!strcmp(ssh_message_channel_request_subsystem(message),"sftp")){
//			sftp=1;
			m_SSHState = SSH_STATE_DONE;
			ssh_message_channel_request_reply_success(message);

			char *welcome;
			asprintf(&welcome,"Last login: Mon Jan 12 22:03:55 2005 from 212.54.21.23\n\r%s@nepenthes:~$ ",m_User.c_str());
			channel_write(m_Channel,(void *)welcome,strlen(welcome));
			free(welcome);
														

//			channel_write(m_Channel,(void *)"Nepenthes ssh honeypot $Rev$\n",strlen("Nepenthes ssh honeypot $Rev$\n"));
//			break;
			//           }
		}
		else
			if ( message && ssh_message_type(message)==SSH_CHANNEL_REQUEST &&
				 ssh_message_subtype(message)==SSH_CHANNEL_REQUEST_EXEC )
		{
			logDebug("SSH_EXEC %s\n",message->channel_request.command);
			m_SSHState = SSH_STATE_DONE;
            ssh_message_channel_request_reply_success(message);

			setStatus(SS_CLOSED);

			m_WhatToRead.append(message->channel_request.command,strlen(message->channel_request.command));
//			ssh_
		}else
		{
			logWarn("SSH Unknown request %i.%i\n",ssh_message_type(message),ssh_message_subtype(message));

		}
		
		break;

	case SSH_STATE_DONE:
		logSpam("%s\n","SSH_STATE_DONE");
		{
//			BUFFER *buf=buffer_new();
			char dest[256];
			int i=0;
			i=channel_read_nonblocking(m_Channel,dest,256,0);
			if ( i>0 )
			{
//            	printf("%.*s\n",i,dest);
				m_WhatToRead.append(dest,i);
            	printf("CHANNEL %s\n",m_WhatToRead.c_str());
				channel_write(m_Channel,(void *)dest,i);
			}else
			{
				m_Status = SS_CLOSED;
			}
			


		}
		break;

	}
    return 0;
}

bool SSHSocket::checkTimeout()
{
	if (isBind())
	{
		return false;
	}

//	logSpam("TIMEOUT %i %i\n",time(0),m_LastAction+m_TimeoutIntervall);
	if (time(0) > m_LastAction+m_TimeoutIntervall)
	{
		setStatus(SS_TIMEOUT);
		return true;
	}
	return false;
}

bool SSHSocket::handleTimeout()
{
	return false;
}

int32_t SSHSocket::getSocket()
{
	if (isBind() == true)
	{
		return ssh_bind_get_fd(m_Bind);
	}else
	{
		return ssh_get_fd(m_Session);
	}
}


int32_t SSHSocket::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
{
	int fd = getSocket();
	return getsockopt(fd, level, optname, optval, optlen);
}

#endif /* HAVE_LIBSSH */
