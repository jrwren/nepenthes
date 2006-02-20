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

#include <ctype.h>

#include "x-6.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "Nepenthes.hpp"


#include "DNSManager.hpp"
#include "DNSResult.hpp"
#include "DNSQuery.hpp"

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
 * creates a new X6 Module, 
 * X6 is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
X6::X6(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-2";
	m_ModuleDescription = "eXample Module 2 -binding sockets & setting up a dialogue example-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "x-2 Factory";
	m_DialogueFactoryDescription = "eXample Dialogue Factory";

	g_Nepenthes = nepenthes;

}

X6::~X6()
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
bool X6::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,10003,0,45,this);
	return true;
}

bool X6::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new X6Dialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *X6::createDialogue(Socket *socket)
{
	return new X6Dialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}


/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the X6Dialogue, creates a new X6Dialogue
 * 
 * 
 * @param socket the Socket the Dialogue has to use
 */
X6Dialogue::X6Dialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "X6Dialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;
	m_DNSCallbackName = "x-6 eXample module";

	m_Socket->doRespond("Welcome to dns Shell\n",strlen("Welcome to dns Shell\n"));
}

X6Dialogue::~X6Dialogue()
{

}

/**
 * Dialogue::incomingData(Message *)
 * 
 * "dns domain.org" will trigger a domain a record lookup
 * "txt domain.org" will trigger a domain txt record lookup
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel X6Dialogue::incomingData(Message *msg)
{
	char *freemessage = strdup(msg->getMsg());
	char *message = freemessage;

	if (message == NULL)
	{
		return CL_ASSIGN;
	}

	for(uint32_t i=0;i < strlen(message);i++)
	{
		if(!isgraph(message[i]) && message[i] != ' ')
		{
			message[i] = ' ';
		}
	}

	char *cmd = strsep(&message, " ");

	if( !strcmp(cmd, "dns") )
	{
		char *url;
		while( (url  = strsep(&message, " ")) != NULL)
		{
				if (strlen(url) > 3)
				{
					g_Nepenthes->getDNSMgr()->addDNS(this,url,this);
				}

		}
		string sDeineMutter("DNS trying to resolve\n");
		msg->getResponder()->doRespond((char *)sDeineMutter.c_str(),sDeineMutter.size());

	} else
	if ( !strcmp(cmd, "txt") )
	{
		char *url;
		while ( (url  = strsep(&message, " ")) != NULL )
		{

			if (strlen(url) > 3)
			{
				g_Nepenthes->getDNSMgr()->addDNS(this,url,this);
			}
		}
		string sDeineMutter("TXT trying to resolve\n");
		msg->getResponder()->doRespond((char *)sDeineMutter.c_str(),sDeineMutter.size());

	}

	if (freemessage != NULL)
	{
    	free(freemessage);
	}

//	msg->getResponder()->doRespond("deine mutter\n",strlen("deine mutter\n"));
	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * 
 * @param msg
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel X6Dialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

/**
 * Dialogue::handleTimeout(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel X6Dialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionLost(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel X6Dialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel X6Dialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}


/**
 * the domain we queried got resolved
 * 
 * @param result the DNSResult for out query
 * 
 * @return returns true
 */
bool X6Dialogue::dnsResolved(DNSResult *result)
{

	if ( result->getQueryType() & DNS_QUERY_A )
	{

		list <uint32_t> resolved = result->getIP4List();

		list <uint32_t>::iterator it;
		int32_t i=0;
		for ( it=resolved.begin();it!=resolved.end();it++ )
		{
			printf("NUM %i\n",(int)i);
			logSpam( "domain %s has ip %s \n",result->getDNS().c_str(),inet_ntoa(*(in_addr *)&*it));
			char *reply;
			asprintf(&reply,"domain %s has A %s (context %08x)\n",result->getDNS().c_str(), inet_ntoa(*(in_addr *)&*it), (uint32_t)((intptr_t)result->getObject()));
			m_Socket->doRespond(reply,strlen(reply));
			free(reply);

//		logSpam("foooo %s \n",msg.c_str());
			i++;
		}
		printf("NUM %i DONE\n",(int)i);
	} else
	if ( result->getQueryType() & DNS_QUERY_TXT )
	{
		string reply = "domain ";
		reply += result->getDNS();
		reply += " has TXT '";
		reply += result->getTXT();
		reply += "'\n";

		m_Socket->doRespond((char *)reply.data(),reply.size());

	}
	return true;
}


/**
 * the domain we queried could not get resolved
 * 
 * @param result the DNSResult for our query so we at least know what failed and have access to the additional data
 * 
 * @return returns true
 */
bool X6Dialogue::dnsFailure(DNSResult *result)
{
	if ( result->getQueryType() & DNS_QUERY_A )
	{
		logSpam("domain %s has no A, resolve error\n",result->getDNS().c_str());
		char *reply;
		asprintf(&reply,"domain '%s' could not resolve A\n", result->getDNS().c_str());
		m_Socket->doRespond(reply,strlen(reply));
		free(reply);
	} else
	if ( result->getQueryType() & DNS_QUERY_TXT )
	{
		logSpam("domain %s has no TXT, resolve error\n",result->getDNS().c_str());
		char *reply;
		asprintf(&reply,"domain '%s' could not resolve TXT\n", result->getDNS().c_str());
		m_Socket->doRespond(reply,strlen(reply));
		free(reply);
	}
	return true;
}



extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new X6(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
