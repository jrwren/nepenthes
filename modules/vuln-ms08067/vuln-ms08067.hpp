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

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

using namespace std;

namespace nepenthes
{
	typedef enum
	{
        MS08067_STAGE1,
		MS08067_STAGE2,
		MS08067_STAGE3,
		MS08067_STAGE4,
		MS08067_STAGE5,
		MS08067_STAGE6,
		MS08067_STAGE6_TRANS,
		MS08067_STAGE7,
		MS08067_STAGE8,
		MS08067_STAGE8_TRANS,
		MS08067_STAGE9,

		MS08067_DONE

	} ms08067_state;

	// SMB command types we use
	typedef enum
	{
		SMB_NEGPROT = '\x72',
		SMB_SESS_SETUP_ANDX = '\x73',
		SMB_READ_ANDX = '\x2e',
		SMB_WRITE_ANDX = '\x2f',
		SMB_NTCREATE_ANDX = '\xa2',
		SMB_TREECONNECT_ANDX = '\x75',
		SMB_TRANS = '\x25',

	} smb_cmdcode;

	class Buffer;

	class MS08067 : public Module , public DialogueFactory
	{
	public:
		MS08067(Nepenthes *);
		~MS08067();
		Dialogue *createDialogue(Socket *socket);
		bool Init();
		bool Exit();
	};

	class MS08067Dialogue : public Dialogue
	{
	public:
		MS08067Dialogue(Socket *socket);
		~MS08067Dialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
		bool checkPacketValidity(Message *msg);
		bool checkSMBCommand(Message *msg, smb_cmdcode cmdcode);

	protected:
		ms08067_state m_State;
		int stage7_sent_size;
		int dcerpc_bind_data_size;
		Buffer *assembled_dcerpc_data;
		Buffer *m_Buffer;
	};

}
extern nepenthes::Nepenthes *g_Nepenthes;

