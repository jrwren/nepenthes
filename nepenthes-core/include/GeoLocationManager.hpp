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


#include <stdint.h>

#include "Manager.hpp"
#include "GeoLocationHandler.hpp"

namespace nepenthes
{

	class GeoLocationCallback;
	class GeoLocationResult;

	/**
	 * if you want to know where a ip is located, ask the geolocationmanager.
	 */
	class GeoLocationManager : public Manager
	{
	public:
		GeoLocationManager(Nepenthes *nepenthes);
		~GeoLocationManager();

		virtual bool addGeoLocation(GeoLocationCallback *callback,uint32_t ip, void *obj);
		virtual bool registerGeolocationHandler(GeoLocationHandler *handler);
		virtual bool cacheGeoLocation(GeoLocationResult *result);

		void doList();
		bool Init();
		bool Exit();

	protected:
		string cachePathFromAddress(uint32_t ip);

		GeoLocationHandler	*m_GeoLocationHandler;
		string m_CachePath;
	};
};

#endif // HAVE_GEOLOCATION
