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
#include "SQLCallback.hpp"
#include "SQLCallback.cpp"

using namespace std;

namespace nepenthes
{

	class SQLQuery;
	class SQLHandler;

	/**
	 * X9 Module
	 * SQL handler example
	 */
	class X9 : public Module , public DialogueFactory
	{
	public:
		X9(Nepenthes *);
		~X9();
		Dialogue *createDialogue(Socket *socket);
		bool Init();
		bool Exit();
	};

	/**
	 * X9Dialogue
	 * shows how to use the SQLCallback and SQLResult
	 */
	class X9Dialogue : public Dialogue , public SQLCallback
	{
	public:
		X9Dialogue(Socket *socket);
		~X9Dialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);

		bool sqlSuccess(SQLResult *result);
		bool sqlFailure(SQLResult *result);

	private:
		list <SQLQuery *>	m_OutstandingQueries;
		SQLHandler 			*m_SQLHandler;

		string m_Buffer;
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;
