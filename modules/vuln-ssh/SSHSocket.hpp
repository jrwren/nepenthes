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

#ifndef HAVE_SSHSOCKET_HPP
	#define HAVE_SSHSOCKET_HPP

	#undef PACKAGE_BUGREPORT 
	#undef PACKAGE_NAME
	#undef PACKAGE_STRING
	#undef PACKAGE_TARNAME
	#undef PACKAGE_VERSION


extern "C"
{

#include <libssh/libssh.h>
#include <libssh/server.h>
}

	#undef PACKAGE_BUGREPORT 
	#undef PACKAGE_NAME
	#undef PACKAGE_STRING
	#undef PACKAGE_TARNAME
	#undef PACKAGE_VERSION


#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "POLLSocket.hpp"

using namespace std;

namespace nepenthes
{

	typedef enum
	{
		SSH_STATE_AUTH,
		SSH_STATE_CHANNEL,
		SSH_STATE_SHELL_OR_SFTP,
		SSH_STATE_DONE,
	}ssh_state;

	class SSHSocket : public POLLSocket
	{
	public:
		SSHSocket(SSH_OPTIONS *options);
		SSHSocket(SSH_SESSION *session);
		~SSHSocket();

		bool Init();

		Socket * acceptConnection();
		bool bindPort();
		bool wantSend();
		int32_t doSend();
		int32_t doRecv();
        bool checkTimeout();
		bool handleTimeout();
		int32_t getSocket();
		int32_t getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen);

	private:
		SSH_BIND 	*m_Bind;
		SSH_SESSION *m_Session;
		CHANNEL *m_Channel;

		ssh_state   m_SSHState;

		string m_WhatToRead;
		string m_User;
		string m_Pass;
	};
}

#endif

#endif /* HAVE_LIBSSH */
