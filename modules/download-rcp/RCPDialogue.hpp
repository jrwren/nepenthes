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

#ifndef HAVE_RCPDIALOGUE_HPP
#define HAVE_RCPDIALOGUE_HPP


#include "Dialogue.hpp"

namespace nepenthes
{
	class Buffer;
	class Download;

	typedef enum 
	{
		RCP_STATE_REQUEST,
		RCP_STATE_FILESTATS,
		RCP_STATE_FILE
	} rcp_state;

	class RCPDialogue : public Dialogue
	{
	public:
		RCPDialogue(Socket *socket, Download *down);
		~RCPDialogue();

		ConsumeLevel connectionEstablished();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);



	protected:
		Buffer 	 *m_Buffer;
		Download *m_Download;
		uint32_t m_MaxFileSize;
		uint32_t m_ExpectedFileSize;
		rcp_state m_State;

	};
}

#endif
