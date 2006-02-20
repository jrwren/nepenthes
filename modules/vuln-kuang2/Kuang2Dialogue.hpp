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








#define K2_HELO           0x324B4F59
#define K2_ERROR          0x52525245
#define K2_DONE           0x454E4F44
#define K2_QUIT           0x54495551
#define K2_DELETE_FILE    0x464C4544
#define K2_RUN_FILE       0x464E5552
#define K2_FOLDER_INFO    0x464E4946
#define K2_DOWNLOAD_FILE  0x464E5744
#define K2_UPLOAD_FILE    0x46445055
#define K2_UPLOAD_FILE    0x46445055
#define K2_UPLOAD_FILE_2  0x0000687f
#define K2_UPLOAD_FILE_3  0x00007620
#define K2_UPLOAD_FILE_4  0x00004820

namespace nepenthes
{
	typedef struct
	{
		unsigned int command;
		union
		{
			char bdata[1024-4];
			struct
			{
				unsigned int param;
				char sdata[1024-8];
			};
		};
	} Kuang2Message;


	typedef enum
	{
		KUANG2_NONE,
		KUANG2_FILETRANSFERR
	} kuang2_state;

	class Buffer;
	class Download;

	class Kuang2Dialogue : public Dialogue
	{
	public:
		Kuang2Dialogue(Socket *socket);
		~Kuang2Dialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
	protected:
		kuang2_state m_State;
		Buffer 		*m_Buffer;
		Download *m_Download;

		string m_FileName;
		unsigned int m_FileSize;
	};
}


