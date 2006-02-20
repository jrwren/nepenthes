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

#include "MSSQLDialogue.hpp"
#include "vuln-mssql.hpp"
#include "mssql-shellcodes.h"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"

#include "Utilities.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia

using namespace nepenthes;

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the MSSQLDialogue, creates a new MSSQLDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
MSSQLDialogue::MSSQLDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "MSSQLDialogue";
	m_DialogueDescription = "talking to MS02-061 exploiters";

	m_ConsumeLevel = CL_ASSIGN;
}

MSSQLDialogue::~MSSQLDialogue()
{

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
ConsumeLevel MSSQLDialogue::incomingData(Message *msg)
{
//	logWarn(" UDP MSG '%.*s'\n",msg->getMsgLen(), msg->getMsg());
	uint32_t ip=msg->getRemoteHost();

	if (msg->getMsgLen() >= sizeof(thc_badbuffer)-1 &&
		memcmp(msg->getMsg(),thc_badbuffer,sizeof(thc_badbuffer)-1) == 0
		)
	{
		logInfo("THCSql bindport 31337 from %s:%i \n",inet_ntoa(*(in_addr *)&ip),msg->getRemotePort());
        Socket *socket;
		if ((socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,31337,60,30)) == NULL)
		{
			logCrit("%s","Could not bind socket 31337 \n");
			return CL_DROP;
		}
		
		DialogueFactory *diaf;
		if ((diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL)
		{
			logCrit("%s","No WinNTShell DialogueFactory availible \n");
			return CL_DROP;
		}

		socket->addDialogueFactory(diaf);
	}else
	if ( msg->getMsgLen() >= sizeof(sql_slammer)-1 && memcmp(msg->getMsg(),sql_slammer,sizeof(sql_slammer)-1) == 0 )
	{
		
		logInfo("%s:%i asked us to join his SQLSlammer Party \n",inet_ntoa(*(in_addr *)&ip),msg->getRemotePort());
	}
	else
	{	// hexdump it
		g_Nepenthes->getUtilities()->hexdump(STDTAGS,(byte*)msg->getMsg(),msg->getMsgLen());

	}

	return CL_DROP;
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
ConsumeLevel MSSQLDialogue::outgoingData(Message *msg)
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
ConsumeLevel MSSQLDialogue::handleTimeout(Message *msg)
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
ConsumeLevel MSSQLDialogue::connectionLost(Message *msg)
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
ConsumeLevel MSSQLDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}






