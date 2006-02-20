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

#include "Dialogue.hpp"

using namespace std;

namespace nepenthes
{
	class UploadQuery;
	class Buffer;


	typedef enum 
	{
		HTTP_UPLOAD_STATE_NULL,
		HTTP_UPLOAD_STATE_ERROR,
		HTTP_UPLOAD_STATE_SUCCESS
	} http_upload_state;

	class HTTPUPDialogue : public Dialogue
	{
	public:
		HTTPUPDialogue(Socket *socket, UploadQuery *query);
		~HTTPUPDialogue();

		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
		ConsumeLevel connectionEstablished();

	private:
		UploadQuery *m_UploadQuery;
		Buffer 		*m_Buffer;

		http_upload_state m_State;
	};
}
