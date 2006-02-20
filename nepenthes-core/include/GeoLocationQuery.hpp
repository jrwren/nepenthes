
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

#ifdef HAVE_GEOLOCATION

#ifndef HAVE_GEOLOCATIONQUERY_HPP
#define HAVE_GEOLOCATIONQUERY_HPP

#include <string>
#include <stdint.h>

using namespace std;

namespace nepenthes
{
	class GeoLocationCallback;

	/**
	 * the GeoLocationManager will create a GeoLocationQuery containing the provided information, 
	 * and pass it to the GeoLocationHandler
	 */
	class GeoLocationQuery
	{
	public:
		GeoLocationQuery(uint32_t address, GeoLocationCallback *callback, void *obj);
		virtual ~GeoLocationQuery();


		virtual uint32_t 	getAddress();
		virtual void 		*getObject();
		virtual GeoLocationCallback *getCallback();

	private:
		uint32_t 	m_Address;
		void 		*m_Object;

		GeoLocationCallback *m_Callback;
	};
};

#endif

#endif // HAVE_GEOLOCATION
