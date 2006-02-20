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

#include "EventManager.hpp"
#include "EventHandler.hpp"
#include "Nepenthes.hpp"
#include "SocketEvent.hpp"

#include "LogManager.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_ev | l_mgr

/**
 * EventManager constructor
 * 
 * @param nepenthes the nepenthes instance
 */
EventManager::EventManager(Nepenthes *nepenthes)
{
}

/**
 * EventManager destructor
 */
EventManager::~EventManager()
{
}

/**
 * Inits the EventManager
 * 
 * registers all internal Events
 * 
 * @return true
 */
bool  EventManager::Init()
{
	// FIXME use a struct
	
	registerInternalEvent("EV_TIMEOUT",EV_TIMEOUT);

	registerInternalEvent("EV_SOCK_TCP_BIND",EV_SOCK_TCP_BIND);
	registerInternalEvent("EV_SOCK_TCP_ACCEPT",EV_SOCK_TCP_ACCEPT);
	registerInternalEvent("EV_SOCK_TCP_CONNECT",EV_SOCK_TCP_CONNECT);
	registerInternalEvent("EV_SOCK_TCP_CLOSE",EV_SOCK_TCP_CLOSE);
	registerInternalEvent("EV_SOCK_TCP_RX",EV_SOCK_TCP_RX);
	registerInternalEvent("EV_SOCK_TCP_TX",EV_SOCK_TCP_TX);

	registerInternalEvent("EV_SOCK_UDP_BIND",EV_SOCK_UDP_BIND);
	registerInternalEvent("EV_SOCK_UDP_ACCEPT",EV_SOCK_UDP_ACCEPT);
	registerInternalEvent("EV_SOCK_UDP_CONNECT",EV_SOCK_UDP_CONNECT);
	registerInternalEvent("EV_SOCK_UDP_CLOSE",EV_SOCK_UDP_CLOSE);
	
	registerInternalEvent("EV_SOCK_UDS_BIND",EV_SOCK_UDS_BIND);
	registerInternalEvent("EV_SOCK_UDS_ACCEPT",EV_SOCK_UDS_ACCEPT);
	registerInternalEvent("EV_SOCK_UDS_CONNECT",EV_SOCK_UDS_CONNECT);
	registerInternalEvent("EV_SOCK_UDS_CLOSE",EV_SOCK_UDS_CLOSE);

	registerInternalEvent("EV_SOCK_RAW_BIND",EV_SOCK_RAW_BIND);
	registerInternalEvent("EV_SOCK_RAW_ACCEPT",EV_SOCK_RAW_ACCEPT);
	registerInternalEvent("EV_SOCK_RAW_CONNECT",EV_SOCK_RAW_CONNECT);
	registerInternalEvent("EV_SOCK_RAW_CLOSE",EV_SOCK_RAW_CLOSE);

	registerInternalEvent("EV_DOWNLOAD",EV_DOWNLOAD);

	registerInternalEvent("EV_SUBMISSION",EV_SUBMISSION);
	registerInternalEvent("EV_SUBMISSION_UNIQ",EV_SUBMISSION_UNIQ);
	registerInternalEvent("EV_SUBMISSION_HIT",EV_SUBMISSION_HIT);

	registerInternalEvent("EV_DIALOGUE_ASSIGN_AND_DONE",EV_DIALOGUE_ASSIGN_AND_DONE);

	return true;
}
/**
 * Exits the EventManager
 * 
 * @return true
 */
bool  EventManager::Exit()
{
	return true;
}

/**
 * lists all EventHandlers
 */
void EventManager::doList()
{
	list <EventHandler *>::iterator ehandler;
	logInfo("=--- %-69s ---=\n","EventManager");
	int32_t i=0;
	for(ehandler = m_EventHandlers.begin();ehandler != m_EventHandlers.end();ehandler++,i++)
	{
		logInfo("  %i) %-8s %s\n",i,(*ehandler)->getEventHandlerName().c_str(), (*ehandler)->getEventHandlerDescription().c_str());
	}
    logInfo("=--- %2i %-66s ---=\n\n",i, "EventHandlers registerd");
}

/**
 * accepts Events, gives them to all EventHandler 's who want that Event
 * 
 * @param event  the Event
 * 
 * @return returns 0
 */
uint32_t EventManager::handleEvent(Event *event)
{
	logPF();
	list <EventHandler *>::iterator handler;
	for(handler = m_EventHandlers.begin();handler != m_EventHandlers.end();handler++)
	{
		if ( (*handler)->testEvent(event) )
			(*handler)->handleEvent(event);
	}
	return 0;
}

/**
 * register a EventHandler
 * 
 * @param handler the EventHandler to register
 */
void EventManager::registerEventHandler(EventHandler *handler)
{
	m_EventHandlers.push_back(handler);
	return;
}
/**
 * unregister an EventHandler
 * 
 * @param handler the EventHandler to unregister
 * 
 * @return true on success, else false
 */
bool EventManager::unregisterEventHandler(EventHandler *handler)
{
	list <EventHandler *>::iterator it;
	for(it = m_EventHandlers.begin();it != m_EventHandlers.end();it++)
	{
		if (*it == handler)
		{
			m_EventHandlers.erase(it);
			return true;
		}
	}

	return false;
}



/**
 * check all EventHandler for timeout
 * 
 * @return returns true
 */
bool EventManager::doTimeoutLoop()
{
	list <EventHandler *>::iterator handler;
	SocketEvent ev(NULL,EV_TIMEOUT);
	for(handler = m_EventHandlers.begin();handler != m_EventHandlers.end();handler++)
	{
		if ( (*handler)->isTimeout() )
			(*handler)->handleEvent(&ev);
	}
	return true;
}		

/**
 * register a internal event
 * 
 * @param name   the events name
 * @param number the events number
 * 
 * @return true on success ( no collisions in number & name)
 *         else false
 */
bool EventManager::registerInternalEvent(char *name, uint16_t number)
{
// check name and number are uniq
	list<EventRegistration *>::iterator it;
	for(it = m_EventRegistrations.begin();it != m_EventRegistrations.end();it++)
	{
		if ((*it)->m_EventName == name || (*it)->m_EventNumber == number )
		{
			logCrit("EVENT %s(%u) collides EVENT %s (%u) \n",(*it)->m_EventName.c_str(),(*it)->m_EventNumber,name,number);
			return false;
		}
	}

	EventRegistration *reg = new EventRegistration;
	reg->m_EventName = name;
	reg->m_EventNumber = number;

	m_EventRegistrations.push_back(reg);

	return true;
}


/**
 * register a external Event
 * 
 * @param name   the Events Name
 * 
 * @return returns the Events Number
 */
uint16_t EventManager::registerEvent(char *name)
{ // FIXME
	int32_t retval = rand()%EVENT_HANDLER_BITSET_SIZE;
	while(registerInternalEvent(name,retval) == false)
		retval = rand();
	return retval;
}
