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

#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "geolocation-ip2location.hpp"

#include "SocketManager.hpp"
#include "TCPSocket.hpp"

#include "LogManager.hpp"

#include "Config.hpp"

#include "GeoLocationManager.hpp"

#include "GeoLocationQuery.hpp"
#include "GeoLocationQuery.cpp"

#include "GeoLocationResult.hpp"
#include "GeoLocationResult.cpp"

#include "GeoLocationCallback.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * The Constructor
 * creates a new GeoLocationIp2Location Module, 
 * GeoLocationIp2Location is an example for binding a socket & setting up the Dialogue & DialogueFactory
 * 
 * 
 * it can be used as a shell emu to allow trigger commands 
 * 
 * 
 * sets the following values:
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
GeoLocationIp2Location::GeoLocationIp2Location(Nepenthes *nepenthes)
{
	m_ModuleName        = "geolocation-ip2location";
	m_ModuleDescription = "resolve ips to coordinates using p2location DB5";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;
}

GeoLocationIp2Location::~GeoLocationIp2Location()
{

}


/**
 * Module::Init()
 * 
 * binds the port, adds the DialogueFactory to the Socket
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool GeoLocationIp2Location::Init()
{
//#ifdef HAVE_LIBIP2LOCATION_H

	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	string path;
	try
	{
		path = m_Config->getValString("geolocation-ip2location.path");
	} catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	if ( (m_Ip2Location = ip2location_open((char *)path.c_str())) == NULL)
	{
		logCrit("Could not open ip2Location Database in Path %s\n",path.c_str());
	}
	ip2location_initialize(m_Ip2Location);

	if ( g_Nepenthes->getGeoMgr()->registerGeolocationHandler(this) == false)
	{
		logCrit("%s","Could not register as GeolocationHandler\n");
		return false;
	}
	return true;
/*#else 
	logCrit("%s","Module compiled without libgeoip installed, wont work\n");
	return false;
#endif
*/
}

bool GeoLocationIp2Location::Exit()
{
	return true;
}


bool GeoLocationIp2Location::geoLocate(GeoLocationQuery *query)
{
//#ifdef HAVE_LIBIP2LOCATION
	uint32_t ip = query->getAddress();

	char *host = inet_ntoa(*(in_addr *)&ip);
	Ip2LocationRecord *record;
	if ( (record = get_record(m_Ip2Location, (char *)host,ALL)) != NULL)
	{
		GeoLocationResult *geo = new GeoLocationResult(ip,record->longitude,record->latitude,record->country_long,record->country_short,record->city,query->getObject());
		query->getCallback()->locationSuccess(geo);
		free_record(record);
		delete geo;
	}

	delete query;
	return true;
//#else
//	return false;
//#endif
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new GeoLocationIp2Location(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

#endif // HAVE_GEOLOCATION
