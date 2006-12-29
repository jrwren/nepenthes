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

#include "vuln-sav.hpp"

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
 * creates a new VulnSAV Module, 
 * VulnSAV is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
VulnSAV::VulnSAV(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-sav";
	m_ModuleDescription = "emulate the bug in symantec antivirus product";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "SAV Factory";
	m_DialogueFactoryDescription = "Symantec Antivirus Client Dialogue Factory";

	g_Nepenthes = nepenthes;
}

VulnSAV::~VulnSAV()
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
bool VulnSAV::Init()
{
/*	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}
*/
	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,2967,0,30,this);
	return true;
}

bool VulnSAV::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new SAVDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *VulnSAV::createDialogue(Socket *socket)
{
	return new SAVDialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}







/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the SAVDialogue, creates a new SAVDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
SAVDialogue::SAVDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "SAVDialogue";
	m_DialogueDescription = "Symantec Antivirus Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

    m_Buffer = new Buffer(512);
}

SAVDialogue::~SAVDialogue()
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
ConsumeLevel SAVDialogue::incomingData(Message *msg)
{

	m_Buffer->add(msg->getMsg(),msg->getSize());

	if ( m_Buffer->getSize() > 0xcd0 )
	{
		Message *Msg = new Message((char *)m_Buffer->getData(), m_Buffer->getSize(),m_Socket->getLocalPort(), m_Socket->getRemotePort(),
								   m_Socket->getLocalHost(), m_Socket->getRemoteHost(), m_Socket, m_Socket);
		sch_result sch;
		sch = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg);
		delete Msg;

		if ( sch == SCH_DONE )
		{
			m_Buffer->clear();
			return CL_ASSIGN_AND_DONE;
		}
		
	}

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
ConsumeLevel SAVDialogue::outgoingData(Message *msg)
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
ConsumeLevel SAVDialogue::handleTimeout(Message *msg)
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
ConsumeLevel SAVDialogue::connectionLost(Message *msg)
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
ConsumeLevel SAVDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}




extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new VulnSAV(nepenthes);
		return (1);
	} else
	{
		return (0);
	}
}
