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

#ifdef HAVE_LIBIP2LOCATION_H
#include <Ip2Location.h>
#endif

extern "C"
{
    #include "Ip2Location.h"
}

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

#include "GeoLocationHandler.hpp"
#include "SubmitHandler.hpp"

using namespace std;

namespace nepenthes
{

	class GeoLocationIp2Location : public Module , public GeoLocationHandler
	{
	public:
		GeoLocationIp2Location(Nepenthes *);
		~GeoLocationIp2Location();
		bool Init();
		bool Exit();

		bool geoLocate(GeoLocationQuery *query);

	private:
//#ifdef HAVE_LIBIP2LOCATION_H
		Ip2Location *m_Ip2Location;
//#endif

	};
}
extern nepenthes::Nepenthes *g_Nepenthes;

#endif // HAVE_GEOLOCATION
