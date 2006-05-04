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

#include "x-2.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "Config.hpp"

#include "Download.hpp"

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
X2::X2(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-2";
	m_ModuleDescription = "eXample Module 2 -binding sockets & setting up a dialogue example-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "x-2 Factory";
	m_DialogueFactoryDescription = "eXample Dialogue Factory";

	g_Nepenthes = nepenthes;
}

X2::~X2()
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
bool X2::Init()
{
	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	StringList sList;
	int32_t timeout;
	try
	{
		sList = *m_Config->getValStringList("x-2.ports");
		timeout = m_Config->getValInt("x-2.accepttimeout");
	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	uint32_t i = 0;
	while (i < sList.size())
	{
		m_Nepenthes->getSocketMgr()->bindTCPSocket(0,atoi(sList[i]),0,timeout,this);
		i++;
	}
	return true;
}

bool X2::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new X2Dialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *X2::createDialogue(Socket *socket)
{
	return new X2Dialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}







/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the X2Dialogue, creates a new X2Dialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
X2Dialogue::X2Dialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "X2Dialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_Socket->doRespond("Welcome to dong Shell\n",strlen("Welcome to dong Shell\n"));

	m_Buffer = new Buffer(512);
}

X2Dialogue::~X2Dialogue()
{
	delete m_Buffer;
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * a small and ugly shell where we can use
 * "download protocol://localction:port/path/to/file
 * to trigger a download
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel X2Dialogue::incomingData(Message *msg)
{
/*
	m_Buffer->add(msg->getMsg(),msg->getSize());

	Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(),m_Socket->getLocalPort(), m_Socket->getRemotePort(),
			m_Socket->getLocalHost(), m_Socket->getRemoteHost(), m_Socket, m_Socket);
	if ( g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg) == SCH_DONE )
	{
		msg->getResponder()->doRespond("found encrypt0r\n",strlen("found encrypt0r\n"));
		m_Buffer->clear();
	}
	delete Msg;

	return CL_ASSIGN;
*/
	char *message = (char *)malloc(msg->getSize()+1);
	memset(message,0,msg->getSize()+1);
    memcpy(message,msg->getMsg(),msg->getSize());

	for(uint32_t i=0;i < strlen(message);i++)
	{
		if(!isgraph(message[i]) && message[i] != ' ')
		{
			message[i] = ' ';
		}
	}

#ifdef WIN32
	char *cmd = message;
#else
	char *cmd = strsep(&message, " ");
#endif

	if( !strncmp(cmd, "download",8) )
	{

		uint8_t downloadflags=0;
		if (strcmp(cmd,"downloadbinary") == 0)
		{
			downloadflags |= DF_TYPE_BINARY;
		}

#ifdef WIN32
	char *url = "http://test.de/";
#else
		char *url = strsep(&message, " ");
#endif
		logCrit("Downloading file from \"%s\"\n", url);

        msg->getSocket()->getNepenthes()->getDownloadMgr()->downloadUrl(msg->getLocalHost(),url, msg->getRemoteHost(), msg->getMsg(),downloadflags);

		string sDeineMutter("trying to download file\n");
		msg->getResponder()->doRespond((char *)sDeineMutter.c_str(),sDeineMutter.size());

	}

//	msg->getResponder()->doRespond("deine mutter\n",strlen("deine mutter\n"));
	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel X2Dialogue::outgoingData(Message *msg)
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
ConsumeLevel X2Dialogue::handleTimeout(Message *msg)
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
ConsumeLevel X2Dialogue::connectionLost(Message *msg)
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
ConsumeLevel X2Dialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}




#ifdef WIN32
extern "C" int32_t __declspec(dllexport)  module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#else
extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#endif

{
	if (version == MODULE_IFACE_VERSION) {
        *module = new X2(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
