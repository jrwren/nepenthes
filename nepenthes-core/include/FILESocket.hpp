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

#ifdef WIN32

#else

#include "Socket.hpp"
#include "Responder.hpp"


namespace nepenthes
{
	class Dialogue;

	class FILESocket : public Socket
	{
public:
    FILESocket(Nepenthes *nepenthes, char *filepath, int32_t flags);
	~FILESocket();
		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int32_t doSend();
		int32_t doRecv();
		int32_t doWrite(char *msg, uint32_t len);
		bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, uint32_t len);
	private:
		string m_FilePath;
		int32_t m_Flags;
	};
}

#endif
