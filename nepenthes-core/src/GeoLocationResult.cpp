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


#include "GeoLocationResult.hpp"

using namespace nepenthes;

/**
 * GeoLocationResult constructor
 * 
 * @param address the address we looked up
 * @param lng     position longitude
 * @param lat     position latitude
 * @param country country
 * @param city    city
 * @param obj     additional data
 */
GeoLocationResult::GeoLocationResult(uint32_t address, float lng, float lat, char *country, char *countryshort, char *city, void *obj)
{
	m_Address = address;
	m_Lat = lat;
	m_Lng = lng;

	if (country != NULL)
	{
    	m_Country = country;
	}else
	{
		m_Country = "";
	}

	if (countryshort != NULL)
	{
		m_CountryShort = countryshort;
	}else
	{
		m_CountryShort = "";
	}


	if (city != NULL)
	{
    	m_City = city;
	}else
	{
		m_City = "";
	}

	m_Object = obj;
}

/**
 * GeoLocationQuery destructor
 */
GeoLocationResult::~GeoLocationResult()
{
}


/**
 * get the lookup address
 * 
 * @return returns the looked up address as unsigned int
 */
uint32_t    GeoLocationResult::getAddress()
{
	return m_Address;
}

/**
 * get the positions latitude
 * 
 * @return returns the latitude
 */
float       GeoLocationResult::getLatitude()
{
	return m_Lat;
}

/**
 * get the positions longitude
 * 
 * @return returns the positions longitude
 */
float       GeoLocationResult::getLongitude()
{
	return m_Lng;
}

/**
 * get the City
 * 
 * @return returns the city as string
 */
string  GeoLocationResult::getCity()
{
	return m_City;
}

/**
 * get the Country
 * 
 * @return returns the country as string
 */
string  GeoLocationResult::getCountry()
{
	return m_Country;
}

/**
 * get the Country short
 * 
 * @return returns the country as string
 */
string  GeoLocationResult::getCountryShort()
{
	return m_CountryShort;
}



/**
 * get the additional data
 * 
 * @return returns pointer to the additional data
 */
void    *GeoLocationResult::getObject()
{
	return m_Object;
}


#endif // HAVE_GEOLOCATION
