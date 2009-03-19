/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 * Copyright (C) 2008  Jason V. Miller
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
 *******************************************************************************/

#ifndef HAVE_DNS_EVENT_HPP
#define HAVE_DNS_EVENT_HPP

#include <string>

#include "DNSQuery.hpp"
#include "DNSResult.hpp"
#include "Event.hpp"

using namespace std;

namespace nepenthes
{
	class DNSEvent : public Event
	{
	public:
		DNSEvent ( uint32_t id, DNSQuery *query, DNSResult *result = NULL )
		{
			m_EventType = id;
			m_Query = query;
			m_Result = result;
		}
		DNSQuery *getQuery ( void )
		{
			return m_Query;
		}
		DNSResult *getResult ( void )
		{
			return m_Result;
		}

	protected:
		DNSQuery *m_Query;
		DNSResult *m_Result;
	};
}

#endif /* HAVE_DNS_EVENT_HPP */

