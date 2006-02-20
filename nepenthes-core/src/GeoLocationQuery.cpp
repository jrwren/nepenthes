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


#include "GeoLocationQuery.hpp"

using namespace nepenthes;

/**
 * GeoLocationQuery constructor
 * 
 * @param address  the address to geolocate
 * @param callback the GeoLocationCallback
 * @param obj      additional data to attach
 */
GeoLocationQuery::GeoLocationQuery(uint32_t address, GeoLocationCallback *callback, void *obj)
{
	m_Address = address;
	m_Callback = callback;
	m_Object = obj;
}

/**
 * GeoLocationQuery destructor
 */
GeoLocationQuery::~GeoLocationQuery()
{

}


/**
 * get the ip address to lookup
 * 
 * @return returns the ip address
 */
uint32_t    GeoLocationQuery::getAddress()
{
	return m_Address;
}

/**
 * get the additional data
 * 
 * @return returns pointer to the additional data
 */
void        *GeoLocationQuery::getObject()
{
	return m_Object;
}

/**
 * get the GeoLocationCallback 
 * 
 * @return returns the GeoLocationCallback
 */
GeoLocationCallback *GeoLocationQuery::getCallback()
{
	return m_Callback;
}

#endif // HAVE_GEOLOCATION
