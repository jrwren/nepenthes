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

#ifndef HAVE_EVENTHANDLER_HPP
#define HAVE_EVENTHANDLER_HPP

#ifdef EVENT_HANDLER_BITSET_SIZE
#undef EVENT_HANDLER_BITSET_SIZE
#endif 

#define EVENT_HANDLER_BITSET_SIZE 256

#include <bitset>
#include <list>
#include <string>
#include <stdint.h>

#ifdef WIN32
#include <time.h>
#include <sys/timeb.h>
#endif

//#include "Event.hpp"

using namespace std;

namespace nepenthes
{
	class EventManager;
	class Event;	

    class EventHandler
    {
    public:
        virtual ~EventHandler(){};
        virtual uint32_t handleEvent(Event *event)=0;
        virtual bool testEvent(Event *event);
		virtual string getEventHandlerDescription();
		virtual string getEventHandlerName();
		virtual bool isTimeout();

    protected:
        bitset<EVENT_HANDLER_BITSET_SIZE> m_Events;
        string m_EventHandlerName;
        string m_EventHandlerDescription;
		time_t m_Timeout;
		time_t m_TimeoutIntervall;


    };

}

#endif
