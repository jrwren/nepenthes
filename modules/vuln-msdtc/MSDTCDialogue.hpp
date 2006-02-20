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

#ifndef HAVE_MSDTCDIALOGUE_HPP
#define HAVE_MSDTCDIALOGUE_HPP

#include "Dialogue.hpp"

using namespace std;

#define DCE_VERSION_MAJOR       0x05
#define DCE_VERSION_MINOR       0x00
#define DCE_PKT_BIND            0x0B
#define DCE_PKT_BINDACK         0x0C
#define DCE_PKT_BINDNACK        0x0D
#define DCE_PKT_REQUEST         0x00
#define DCE_PKT_FAULT           0x03
#define DCE_PKT_RESPONSE        0x02
#define DCE_PKT_ALTCONT         0x0E
#define DCE_PKT_ALTCONTRESP     0x0F
#define DCE_PKT_BINDRESP        0x10

namespace nepenthes
{
	typedef enum {
		MSDTC_STATE_NULL=0,
		MSDTC_STATE_REQUEST_1,
		MSDTC_STATE_DONE
	} msdtc_state;

	class Buffer;

	class MSDTCDialogue : public Dialogue
	{
	public:
		MSDTCDialogue(Socket *socket);
		~MSDTCDialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
		void dump();

	protected:
		msdtc_state	m_State;
		string		m_Shellcode;
		Buffer		*m_Buffer;

	};





}

#endif
