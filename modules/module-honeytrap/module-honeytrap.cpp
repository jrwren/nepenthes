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

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>




#include "module-honeytrap.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "EventManager.hpp"

#include "SocketEvent.hpp"
#include "DialogueFactoryManager.hpp"
#include "Utilities.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "Config.hpp"

#include "Download.hpp"

#include "PCAPSocket.hpp"

#include "Socket.cpp"
#include "POLLSocket.cpp"

#include "EventHandler.cpp"

#include "TrapSocket.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;


/*
 * This module is derived from honeytrap (honeytrap.sf.net) by Werner Tillmann
 *
 * The idea is really good, and it as it was easy to make a nepenthes module of it, we stole it
 *
 */ 




/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;
ModuleHoneyTrap *g_ModuleHoneytrap;

/**
 * The Constructor
 * creates a new ModuleHoneyTrap Module, 
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
ModuleHoneyTrap::ModuleHoneyTrap(Nepenthes *nepenthes)
{
	m_ModuleName        = "module-honeytrap";
	m_ModuleDescription = "open closed ports to accept connections - idea from http://honeytrap.sf.net ";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;
	g_ModuleHoneytrap = this;

}

ModuleHoneyTrap::~ModuleHoneyTrap()
{

}


/**
 * Module::Init()
 * 
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool ModuleHoneyTrap::Init()
{

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}


	

	string mode;
	try
	{

		mode = m_Config->getValString("module-honeytrap.listen_mode");
	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	logInfo("Supported honeytrap modes %s, choosen mode %s\n",
			TrapSocket::getSupportedModes().c_str(),
			mode.c_str());

	Socket *s = NULL;
#ifdef HAVE_PCAP
	if (mode == "pcap")
	{
		string device;
		try
		{
			device = m_Config->getValString("module-honeytrap.pcap.device");
		} catch (...)
		{
			return false;
		}
		s = new TrapSocket(device);
		if (s->Init() != true)
			return false;
	}
#endif 

#ifdef HAVE_IPQ
	if (mode == "ipq")
	{
		s = new TrapSocket(true);
		if (s->Init() != true)
			return false;
	}
#endif 

#ifdef HAVE_IPFW
	if (mode == "ipfw")
	{
		uint16_t port;
		try
		{
			port = m_Config->getValInt("module-honeytrap.divert.port");
		} catch (...)
		{
			return false;
		}
		s = new TrapSocket(port);
		if (s->Init() != true)
			return false;

	}
#endif 

	if (s == NULL)
	{
		logCrit("Invalid mode\n");
		return false;
	}


	

	m_Events.set(EV_SOCK_TCP_ACCEPT);
	m_Events.set(EV_SOCK_TCP_CLOSE);
	REG_EVENT_HANDLER(this);


	return true;
}


bool ModuleHoneyTrap::Exit()
{
	map<connection_t ,Socket *,cmp_connection_t>::iterator it;

	for (it = m_Sockets.begin(); it != m_Sockets.end();it++)
	{
		g_Nepenthes->getSocketMgr()->removePOLLSocket((POLLSocket*)it->second);
	}

	m_Sockets.clear();
	m_Events.reset();

	return true;
}



bool ModuleHoneyTrap::socketDel(Socket *s)
{
	logPF();
	logSpam("connection tracking has %i entries\n",m_Sockets.size());
	connection_t c;
	memset(&c,0,sizeof(connection_t));
	c.m_RemoteHost = s->getRemoteHost();
	c.m_RemotePort = s->getRemotePort();
	c.m_LocalHost  = s->getLocalHost();
	c.m_LocalPort  = s->getLocalPort();

	if (m_Sockets.count(c) == 0)
	{
		logWarn("Can not delete untracked socket\n");
		return false;
	}

	logSpam("erasing socket from tracker\n");
	m_Sockets.erase(c);
    return true;
}


bool ModuleHoneyTrap::socketExists(uint32_t remotehost, uint16_t remoteport, uint32_t localhost, uint16_t localport)
{
	logPF();
	logSpam("connection tracking has %i entries\n",m_Sockets.size());
	connection_t c;
	memset(&c,0,sizeof(connection_t));
	c.m_RemoteHost = remotehost;
	c.m_RemotePort = remoteport;
	c.m_LocalHost  = localhost;
	c.m_LocalPort  = localport;

	if (m_Sockets.count(c) > 0)
	{
		logSpam("Socket exists\n");
       	return true;
	}

	logSpam("Socket does not exist\n");
	return false;
}

bool ModuleHoneyTrap::socketAdd(uint32_t remotehost, uint16_t remoteport, uint32_t localhost, uint16_t localport, Socket *s)
{
	logPF();


	connection_t c;
	memset(&c,0,sizeof(connection_t));
	c.m_RemoteHost = remotehost;
	c.m_RemotePort = remoteport;
	c.m_LocalHost  = localhost;
	c.m_LocalPort  = localport;

	if (m_Sockets.count(c) > 0)
	{
		logCrit("duplicate socket in tracker\n");
    	return false;
	}

	m_Sockets[c] = s;

	return true;
}


uint32_t ModuleHoneyTrap::handleEvent(Event *event)
{
	logPF();



	if (!(((SocketEvent *)event)->getSocket()->getType() & ST_ACCEPT) )
	{
		logSpam("Not a accept socket, dropping\n");
		return 0;

	}

	connection_t c;
	c.m_RemoteHost = ((SocketEvent *)event)->getSocket()->getRemoteHost();
	c.m_RemotePort = ((SocketEvent *)event)->getSocket()->getRemotePort();
	c.m_LocalHost  = ((SocketEvent *)event)->getSocket()->getLocalHost();
	c.m_LocalPort  = ((SocketEvent *)event)->getSocket()->getLocalPort();

	if (m_Sockets.count(c) == 0)
	{
		string rhost = inet_ntoa(*(in_addr *)&c.m_RemoteHost);
		string lhost = inet_ntoa(*(in_addr *)&c.m_LocalHost);

		logInfo("Connection %s:%i %s:%i unknown, dropping\n", rhost.c_str(),c.m_RemotePort,
				 lhost.c_str(),c.m_LocalPort);
		return 0;
	}




	switch(event->getType())
	{
	case EV_SOCK_TCP_ACCEPT:
		((PCAPSocket *)m_Sockets[c])->active();
		break;

	case EV_SOCK_TCP_CLOSE:
		((PCAPSocket *)m_Sockets[c])->dead();
		break;
	}

	return 0;

}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new ModuleHoneyTrap(nepenthes);
		return 1;
	}
	else
	{
		return 0;
	}
}





