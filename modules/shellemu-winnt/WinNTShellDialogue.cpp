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

#include <arpa/inet.h>
#include <errno.h>
#include <string>

#include "WinNTShellDialogue.hpp"
#include "Socket.hpp"
#include "Message.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "shellemu-winnt.hpp"

#include "VFS.hpp"

using namespace nepenthes;
using namespace std;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia | l_shell

#define WIN32_HEADER "Microsoft Windows 2000 [Version 5.00.2195]\n(C) Copyright 1985-2000 Microsoft Corp.\n\nC:\\WINDOWS\\System32>"

WinNTShellDialogue::WinNTShellDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "WinNTShellDialogue";
	m_DialogueDescription = "for now just write the shell stuff to disk";

	m_ConsumeLevel = CL_ASSIGN;

	m_File = NULL;
	m_VFS.Init(this);
	if (socket != NULL && socket->getType() & ST_ACCEPT)
	{
		m_Socket->doRespond(WIN32_HEADER,strlen(WIN32_HEADER));
	}
}

WinNTShellDialogue::~WinNTShellDialogue()
{
	if (m_File != NULL)
		fclose(m_File);
}

ConsumeLevel WinNTShellDialogue::connectionEstablished()
{
	m_Socket->doRespond(WIN32_HEADER,strlen(WIN32_HEADER));
	return m_ConsumeLevel;
}

ConsumeLevel WinNTShellDialogue::incomingData(Message *msg)
{
/*
#ifndef NDEBUG
	if (m_File == NULL && m_Socket != NULL )
	{
		uint32_t host = m_Socket->getRemoteHost();
		string filename = "/tmp/";
		filename += inet_ntoa(*(in_addr *)&host);
		logInfo("trying to open file %s \n",filename.c_str());
		if ( (m_File = fopen(filename.c_str(),"a+")) == NULL)
		{
			logCrit("Could not open file %s (%s)\n",filename.c_str(),strerror(errno));
		}
	}
#endif
*/
	string smsg(msg->getMsg(),msg->getSize());
	string foo = m_VFS.execute(&smsg);

	if (foo.size() > 0 && m_Socket != NULL)
	{
		
		m_Socket->doRespond((char *)foo.c_str(),foo.size());
	}
/*
#ifndef NDEBUG
	if (m_File != NULL)
	{
    	fwrite((void *)msg->getMsg(),msg->getSize(),1,m_File);
		fflush(m_File);
	}
#endif
*/
	return CL_ASSIGN;
}

ConsumeLevel WinNTShellDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

ConsumeLevel WinNTShellDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel WinNTShellDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

ConsumeLevel WinNTShellDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

