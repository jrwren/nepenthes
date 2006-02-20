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

EventManager::EventManager(Nepenthes *nepenthes)
{
}
EventManager::~EventManager()
{
}

bool  EventManager::Init()
{
	return true;
}
bool  EventManager::Exit()
{
	return true;
}

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

void EventManager::registerEventHandler(EventHandler *handler)
{
	m_EventHandlers.push_back(handler);
	return;
}
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
