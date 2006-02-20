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

#ifdef WIN32
	// win32 cant do this

#else

#include <unistd.h>


#include <sys/types.h>
#include <errno.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
	   
#include "FILESocket.hpp"
#include "DialogueFactory.hpp"
#include "Packet.hpp"
#include "Message.hpp"
#include "Dialogue.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

FILESocket::FILESocket(Nepenthes *nepenthes, char *filepath, int32_t flags)
{
	logPF();
	m_Nepenthes = nepenthes;
	m_FilePath = filepath;
	m_Flags = flags;

	setLocalHost(0);
	setLocalPort(0);

	setRemoteHost(0);
	setRemotePort(0);
	m_Status = SS_NULL;
	m_Type = ST_ACCEPT | ST_FILE;
	m_Polled = false;
}

FILESocket::~FILESocket()
{

}

bool FILESocket::bindPort()
{
	return 0;
}
bool FILESocket::Init()
{
	logPF();
	if((m_Socket = open(m_FilePath.c_str(),m_Flags) ) <= 0)
	{
		logCrit("Could not open file %s flags %i\n",m_FilePath.c_str(),m_Flags);
		m_Status = SS_CLOSED;
		return false;
	}
	return true;
}
bool FILESocket::Exit()
{
	close(m_Socket);
	return true;
}
bool FILESocket::connectHost()
{
	return false;
}
Socket * FILESocket::acceptConnection()
{
	return NULL;
}
bool FILESocket::wantSend()
{
	return false;
}

int32_t FILESocket::doSend()
{
	return 0;
}
int32_t FILESocket::doRecv()
{
	logPF();
//	ssize_t read(int32_t fd, void *buf, size_t count);

	char szBuffer[2048];
	int32_t readbytes=0;
	if((readbytes = read(m_Socket,szBuffer,2048)) > 0)
	{
		Message Msg(szBuffer,readbytes,0,0,0,0,this,this);
		if (m_Dialogues.front()->incomingData(&Msg) == CL_DROP)
			m_Status = SS_CLOSED;
	}
    return readbytes;
}
int32_t FILESocket::doWrite(char *msg, uint32_t len)
{
	return 0;
}

bool FILESocket::checkTimeout()
{
	return false;
}

bool FILESocket::handleTimeout()
{
	return false;
}

bool FILESocket::doRespond(char *msg, uint32_t len)
{
	return false;
}

#endif // WIN32

