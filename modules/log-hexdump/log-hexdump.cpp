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
/* modified for log-hexdump module */

#include "log-hexdump.hpp"

#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SocketEvent.hpp"
#include "Buffer.hpp"
#include "Config.hpp"

#include "Utilities.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_ev | l_hlr


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * Constructor
 * creates a new LogHexdump Module, where x% is public Module, public EventHandler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the EventHandlerName
 * - sets the EventHandlerDescription
 * - sets the EventHandlers Timeout
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
LogHexdump::LogHexdump(Nepenthes *nepenthes)
{
	m_ModuleName        = "log-hexdump";
	m_ModuleDescription = "logs hexdump to hexdumps files";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = "LogHexdumpEventHandler";
	m_EventHandlerDescription = "dump hex data to files";

	m_Timeout = 0;
	g_Nepenthes = nepenthes;

}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
LogHexdump::~LogHexdump()
{

}



/**
 * bool Module::Init()
 * setup Module specific values 
 * here:
 * - register als EventHandler
 * - set wanted events
 * 
 * @return returns true if everything was fine, else false
 *         returning false will showup errors in warning a module
 */
bool LogHexdump::Init()
{

	m_HexdumpPath = g_Nepenthes->getConfig()->getValString("nepenthes.utilities.hexdump_path");
	if ( m_HexdumpPath.size() == 0 )
	{
		logCrit("Could not find hexdump_path in config file. \n");
		return false;
	}

	m_Events.set(EV_HEXDUMP);
	REG_EVENT_HANDLER(this);
	logInfo("LogHexdump::Init done.....\n");
	return true;
}


/**
 * unregister as EventHandler
 * 
 * @return returns true if everything was fine
 */
bool LogHexdump::Exit()
{
	return true;

}


/**
 * the handleEvent method is called whenever an event occurs 
 * the EventHandler wanted to have.
 * 
 * @param event  the Event
 * 
 * @return return 0
 */
uint32_t LogHexdump::handleEvent(Event *event)
{
	logPF();

	void *data;
	uint32_t size;

	logInfo("LogHexdump::handleEvent, event %d\n", event->getType());
	switch ( event->getType() )
	{
	case EV_HEXDUMP:
		data = ((HexdumpEvent *)event)->getData();
		size = ((HexdumpEvent *)event)->getSize();

		g_Nepenthes->getUtilities()->hexdump(l_debug, (byte *)data, size);
		break;

	default:
		logWarn("this should not happen\n");
	}
	return 0;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new LogHexdump(nepenthes);
		logInfo("loghexdump module is initialized....\n");
		return 1;
	}
	else
	{
		return 0;
	}
}

