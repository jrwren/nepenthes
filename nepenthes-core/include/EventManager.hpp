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
#include <string>
#include <stdint.h>

#include "Manager.hpp"

#define REG_EVENT_HANDLER(handler) g_Nepenthes->getEventMgr()->registerEventHandler(handler)
#define UNREG_EVENT_HANDLER(handler) g_Nepenthes->getEventMgr()->unregisterEventHandler(handler)

using namespace std;

namespace nepenthes
{
	class Nepenthes;
	class Event;
	class EventHandler;

	struct EventRegistration
	{
		string 		m_EventName;
		uint32_t 	m_EventNumber;
	};

	/**
	 * if something throw a Event , the EventManager will pass 
	 * it to all EventHandler who are interested in it
	 */
    class EventManager: public Manager
    {
    public:
        EventManager(Nepenthes *nepenthes);
        virtual ~EventManager();
        virtual uint32_t handleEvent(Event *event);
        virtual void registerEventHandler(EventHandler *handler);
        virtual bool unregisterEventHandler(EventHandler *handler);

		bool doTimeoutLoop();
		void doList();
		bool Init();
		bool Exit();

		virtual uint16_t registerEvent(char *name);
//		virtual uint16_t registerEvent(const char *name);
//		virtual int32_t	 getEventbyName(char *name);
    private:

		bool registerInternalEvent(const char *name, uint16_t number);


        list <EventHandler *> m_EventHandlers;

		list <EventRegistration *> m_EventRegistrations;	// list sucks, as m_EventNumber has to be uniq, 
														// but map sucks also, as EventName has to be uniq too.
    };

}
 
#endif
