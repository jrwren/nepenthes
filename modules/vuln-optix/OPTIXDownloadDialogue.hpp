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


#ifndef HAVE_OPTIXDOWNLOADDIALOGUE_HPP
#define HAVE_OPTIXDOWNLOADDIALOGUE_HPP

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

#include <pcre.h>

using namespace std;

namespace nepenthes
{
	typedef enum
	{
        OPTIX_DL_FILEINFO,
		OPTIX_DL_FILETRANSFERR,
	}optix_download_state;


	class Download;
	class Buffer;

	class OPTIXDownloadDialogue : public Dialogue
	{
	public:
		OPTIXDownloadDialogue(Socket *socket);
		~OPTIXDownloadDialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);

	protected:
		optix_download_state m_State;
		Download 			*m_Download;
		pcre 				*m_pcre;
		Buffer 				*m_Buffer;
		uint32_t		m_FileSize;
    };
};

#endif
