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

#ifndef HAVE_TFTPDIALOGUE_HPP
#define HAVE_TFTPDIALOGUE_HPP

#include "TFTPDownloadHandler.hpp"

namespace nepenthes
{
	class TFTPDialogue : public Dialogue
	{
	public:
		TFTPDialogue(Socket *socket);
		~TFTPDialogue();

		void setDownload(Download *down);
		void setMaxFileSize(unsigned long i);
		void setMaxRetries(unsigned int i);
		int setRequest(char *file);
		char *getRequest();

		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);



	protected:
		Download *m_Download;
		unsigned long m_MaxFileSize;

		unsigned int m_MaxRetries;
		unsigned int m_Retries;

		char 			*m_LastSendPacket;
		unsigned int 	m_LastSendLength;
		unsigned int	m_Blocks;
	};
}

#endif
