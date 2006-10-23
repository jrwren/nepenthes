/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter
 * Copyright (C) 2005  Georg Wicherski
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

#include <ctype.h>


#include "log-irc.hpp"
#include "IrcDialogue.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "LogHandler.cpp"

#include "DialogueFactoryManager.hpp"
#include "DNSManager.hpp"
#include "DNSResult.hpp"
#include "Nepenthes.hpp"
#include "Config.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * The Constructor
 * creates a new X2 Module, 
 * X2 is an example for binding a socket & setting up the Dialogue & DialogueFactory
 * 
 * 
 * it can be used as a shell emu to allow trigger commands 
 * 
 * 
 * sets the following values:
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
LogIrc::LogIrc(Nepenthes *nepenthes)// : LogHandler(nepenthes->getLogMgr())
{
	m_ModuleName        = "log-irc";
	m_ModuleDescription = "log to irc (optionally using tor)";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;

	m_DNSCallbackName	= "log-irc DNSCallback";

	m_State = LIRC_INIT;

	m_IrcDialogue = NULL;
}


LogIrc::~LogIrc()
{

}


/**
 * Module::Init()
 * 
 * binds the port, adds the DialogueFactory to the Socket
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool LogIrc::Init()
{
	switch (m_State)
	{
	case LIRC_INIT:
		
		m_ModuleManager = m_Nepenthes->getModuleMgr();

		if ( m_Config == NULL )
		{
			logCrit("I need a config\n");
			return false;
		}

		try
		{
			m_UseTor = (bool)m_Config->getValInt("log-irc.use-tor");
			m_TorServer = m_Config->getValString("log-irc.tor.server");
			m_TorPort 	= m_Config->getValInt("log-irc.tor.port");

			m_IrcServer = m_Config->getValString("log-irc.irc.server.name");
			m_IrcPort 	= m_Config->getValInt("log-irc.irc.server.port");
			m_IrcPass   	= m_Config->getValString("log-irc.irc.server.pass");
			
			m_IrcNick   	= m_Config->getValString("log-irc.irc.user.nick");
			m_IrcIdent   	= m_Config->getValString("log-irc.irc.user.ident");
			m_IrcUserInfo	= m_Config->getValString("log-irc.irc.user.userinfo");
			m_IrcUserModes  = m_Config->getValString("log-irc.irc.user.usermodes");

			m_IrcChannel   = m_Config->getValString("log-irc.irc.channel.name");
			m_IrcChannelPass= m_Config->getValString("log-irc.irc.channel.pass");

		}
		catch ( ... )
		{
			logCrit("Error setting needed vars, check your config\n");
			return false;
		}
		
		try
		{
			setLogPattern(m_Config->getValString("log-irc.tag-pattern"));
		}
		catch (...)
		{
			m_LogPatternNumeric = 0;
		}
		
		try
		{
			m_ConnectCommand = string(m_Config->getValString("log-irc.irc.connect-command")) + "\r\n";
		}
		catch (...)
		{
		}
		
		m_State = LIRC_NULL;
		doStart();
		break;
	default:
		logCrit("Calling Init() in invalid State %i \n",(int32_t)m_State);
	}
//	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,10002,0,45,this);

	g_Nepenthes->getLogMgr()->addLogger(this,l_dl|l_sub);
	return true;
}

bool LogIrc::doStart()
{
	logPF();
	if ( m_UseTor )
	{
		switch ( m_State )
		{
		case LIRC_NULL:
			m_State = LIRC_RESOLV_TOR;
			g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_TorServer.c_str(),this);
			break;

		case LIRC_RESOLV_TOR:
			m_State = LIRC_RESOLV_IRC;
			g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_IrcServer.c_str(),this);
			break;

		default:
			logCrit("Calling doStart() in invalid State %i \n",(int32_t)m_State);

		}
	} else
	{
		switch ( m_State )
		{
		case LIRC_NULL:
			m_State = LIRC_RESOLV_IRC;
			g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_IrcServer.c_str(),this);
			break;
		default:
			logCrit("Calling doStart() in invalid State %i \n",(int32_t)m_State);

		}
	}
	return true;

}

bool LogIrc::doStopp()
{
	logPF();
	m_State = LIRC_NULL;
	m_IrcDialogue = NULL;
    return true;
}

bool LogIrc::doRestart()
{
	logPF();
	doStopp();
	doStart();
	return true;
}

bool LogIrc::Exit()
{
	if (g_Nepenthes->getLogMgr()->delLogger(this) == true)
	{
		logDebug("Unregisterd from logmanager\n");
	}else
	{
		logWarn("Could not unregister from logmanager\n");
	}
	return true;
}

bool LogIrc::dnsResolved(DNSResult *result)
{
	switch (m_State)
	{
	case LIRC_RESOLV_TOR:
		{
			m_State = LIRC_RESOLV_IRC;
			list <uint32_t> resolved = result->getIP4List();
            m_TorIP = resolved.front();
			logSpam("Resolved tor host %s to %s \n",result->getDNS().c_str(),inet_ntoa(*(in_addr *)&m_TorIP));
			g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_IrcServer.c_str(),this);
			
		}
		break;

	case LIRC_RESOLV_IRC:
		{// connect tor, create dialogue, assign dialogue, 
		
			list <uint32_t> resolved = result->getIP4List();
			m_IrcIP = resolved.front();
			logSpam("Resolved Irc host %s to %s \n",result->getDNS().c_str(),inet_ntoa(*(in_addr *)&m_IrcIP));

			Socket *socket;
			if (m_UseTor)
			{
            		socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,m_TorIP,m_TorPort,300);
			}else
			{
				socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,m_IrcIP,m_IrcPort,300);
			}

			m_IrcDialogue = new IrcDialogue(socket, this);
			socket->addDialogue(m_IrcDialogue);
			m_IrcDialogue = NULL;
		}
		
		break;
	default:
		logCrit("Calling doStart() in invalid State %i \n",(int32_t)m_State);



/*	list <uint32_t> resolved = result->getIP4List();

	list <uint32_t>::iterator it;
	for (it=resolved.begin();it!=resolved.end();it++)
	{
		logSpam( "DNS has ip %s \n",inet_ntoa(*(in_addr *)&*it));
		char *reply;
		asprintf(&reply,"DNS %s has ip %s (context %8x)\n",result->getDNS().c_str(), inet_ntoa(*(in_addr *)&*it), (uint32_t)result->getObject());
		m_Socket->doRespond(reply,strlen(reply));
		free(reply);
		
//		logSpam("foooo %s \n",msg.c_str());
	}
*/	}
	return true;
}

bool LogIrc::dnsFailure(DNSResult *result)
{
	logPF();

	logWarn("LogIrc DNS %s has no ip, resolve error, retrying ... \n", result->getDNS().c_str());
	g_Nepenthes->getDNSMgr()->addDNS(this,(char *)result->getDNS().c_str(),this);
    return true;
}

void LogIrc::log(uint32_t mask, const char *message)
{
	if (m_IrcDialogue != NULL)
		m_IrcDialogue->logIrc(mask, message);
}

string LogIrc::getTorServer()
{
	return m_TorServer;
}

string LogIrc::getIrcServer()
{
	return m_IrcServer;
}

uint32_t LogIrc::getIrcIP()
{
	return m_IrcIP;
}

uint16_t LogIrc::getIrcPort()
{
	return m_IrcPort;
}

string LogIrc::getIrcPass()
{
	return m_IrcPass;
}

string LogIrc::getIrcNick()
{
	return m_IrcNick;
}

string LogIrc::getIrcIdent()
{
	return m_IrcIdent;
}

string LogIrc::getIrcUserInfo()
{
	return m_IrcUserInfo;
}

string LogIrc::getIrcChannel()
{
	return m_IrcChannel;
}

string LogIrc::getIrcChannelPass()
{
	return m_IrcChannelPass;
}

string LogIrc::getIrcUserModes()
{
	return m_IrcUserModes;
}

string LogIrc::getConnectCommand()
{
	return m_ConnectCommand;
}


bool LogIrc::logMaskMatches(uint32_t mask)
{
	if(!m_LogPatternNumeric)
	{
		// copied from common's original code
		return ((mask & l_dl || mask & l_sub) && mask & l_mgr && !(mask & l_spam)) || (mask & l_warn || mask & l_crit);
	}
	else
	{
		return (mask & m_LogPatternNumeric);
	}
}

void LogIrc::setLogPattern(const char *patternString)
{	
	// this code just plain sucks, does not detect wrong pattern names...
	m_LogPatternNumeric = g_Nepenthes->getLogMgr()->parseTagString(patternString);
}

void LogIrc::setDialogue(IrcDialogue *dia)
{
	m_IrcDialogue = dia;
}

bool LogIrc::useTor()
{
	return m_UseTor;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new LogIrc(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

