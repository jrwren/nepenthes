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

#ifndef HAVE_EVENTMANAGER_HPP
#define HAVE_EVENTMANAGER_HPP

#include <list>

#include "Manager.hpp"

#define REG_EVENT_HANDLER(handler) g_Nepenthes->getEventMgr()->registerEventHandler(handler)

using namespace std;

namespace nepenthes
{
	class Nepenthes;
	class Event;
	class EventHandler;


    class EventManager: public Manager
    {
    public:
        EventManager(Nepenthes *nepenthes);
        virtual ~EventManager();
        unsigned int handleEvent(Event *event);
        virtual void registerEventHandler(EventHandler *handler);
        virtual bool unregisterEventHandler(EventHandler *handler);

		bool doTimeoutLoop();
		void doList();
		bool Init();
		bool Exit();

    private:
        list <EventHandler *> m_EventHandlers;
    };

}
 
#endif
