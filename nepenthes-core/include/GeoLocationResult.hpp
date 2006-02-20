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

#ifndef HAVE_GEOLOCATIONRESULT_HPP
#define HAVE_GEOLOCATIONRESULT_HPP

#include <stdint.h>
#include <string>

using namespace std;

namespace nepenthes
{
	class GeoLocationResult
	{
	public:
		GeoLocationResult(uint32_t address, float lng, float lat, char *country, char *countrylong, char *city, void *obj);
		virtual ~GeoLocationResult();


		virtual uint32_t 	getAddress();

		virtual float 		getLatitude();
		virtual float 		getLongitude();

		virtual string 		getCity();
        virtual string 		getCountry();
		virtual string 		getCountryShort();

		virtual void 		*getObject();

	private:
		uint32_t 	m_Address;

		float 		m_Lng;
		float 		m_Lat;

		string 		m_Country;
		string 		m_CountryShort;
		string		m_City;

		void 		*m_Object;
	};
};

#endif

#endif // HAVE_GEOLOCATION
