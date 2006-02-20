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

#ifndef HAVE_OPTIXDOWNLOADHANDLER_HPP
#define HAVE_OPTIXDOWNLOADHANDLER_HPP

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"
#include "DownloadHandler.hpp"

using namespace std;

namespace nepenthes
{

	class OPTIXDownloadHandler : public DownloadHandler , public DialogueFactory
	{
	public:
		OPTIXDownloadHandler(Nepenthes *);
		~OPTIXDownloadHandler();
		Dialogue *createDialogue(Socket *socket);
		bool download(Download *down);
		bool Init();
		bool Exit();

		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);

		void setSocket(Socket *s);
		void setDialogue(Dialogue *dia);
	protected:
		Socket 		*m_Socket;
		Dialogue 	*m_Dialogue;
	};
}


#endif
