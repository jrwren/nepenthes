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

#include "config.h"

#ifdef HAVE_LIBPRELUDE
#include <libprelude/prelude.h>
#endif

#include <string>

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "EventHandler.hpp"
#include "EventHandler.cpp"

using namespace std;

#ifdef HAVE_LIBPRELUDE
int32_t add_idmef_object(idmef_message_t *message, const char *object, const char *value);
int32_t add_idmef_object(idmef_message_t *message, const char *object, int32_t i);
#endif

namespace nepenthes
{

	class LogPrelude : public Module , public EventHandler
	{
	public:
		LogPrelude(Nepenthes *);
		~LogPrelude();
		bool Init();
		bool Exit();

		uint32_t handleEvent(Event *event);

		void handleTCPaccept(Event *event);
		void handleTCPclose(Event *event);
		void handleDownload(Event *event);
		void handleSubmission(Event *event);
		void handleShellcodeDone(Event *event);
		void handleDialogueAssignAndDone(Event *event);
		
	protected:
		uint64_t generateID()
		{
			return ((uint64_t) time(0)) << 32 | (uint64_t) rand();
		}

#ifdef HAVE_LIBPRELUDE
		prelude_client_t *m_PreludeClient;
#endif
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;
