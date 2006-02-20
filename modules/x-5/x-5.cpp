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

#include "x-5.hpp"
#include "LogManager.hpp"
#include "EventManager.hpp"
#include "SocketEvent.hpp"
#include "Socket.hpp"

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
 * creates a new X5 Module, where x% is public Module, public EventHandler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the EventHandlerName
 * - sets the EventHandlerDescription
 * - sets the EventHandlers Timeout
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
X5::X5(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-5";
	m_ModuleDescription = "eXample Module 5 -eventhandler example-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = "X5EventHandler";
	m_EventHandlerDescription = "printf some events to console if they get fired";

	m_Timeout = time(NULL) + rand()%23;

	g_Nepenthes = nepenthes;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
X5::~X5()
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
bool X5::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
    m_Events.set(EV_SOCK_TCP_ACCEPT);
	m_Events.set(EV_TIMEOUT);
	REG_EVENT_HANDLER(this);
	return true;
}


/**
 * unregister as EventHandler
 * 
 * @return returns true if everything was fine
 */
bool X5::Exit()
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
unsigned int X5::handleEvent(Event *event)
{
	logPF();
	logInfo("Event %i\n",event->getType());
	switch(event->getType())
	{
	case EV_SOCK_TCP_ACCEPT:
		logInfo("X5 EVENT Connection accepted \n\t %s\n",((SocketEvent *)event)->getSocket()->getDescription().c_str());
		break;

	case EV_TIMEOUT:
		m_Timeout = time(NULL) + rand()%23;
		logInfo("X5 EVENT Timeout %i\n",(int)time(NULL));
		break;

	default:
		logWarn("%s","this should not happen\n");
	}
	return 0;
}


extern "C" int module_init(int version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new X5(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
