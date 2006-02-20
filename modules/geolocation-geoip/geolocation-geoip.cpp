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

#include "geolocation-geoip.hpp"

#include "SocketManager.hpp"
#include "TCPSocket.hpp"

#include "LogManager.hpp"

#include "Config.hpp"

#include "GeoLocationManager.hpp"
#include "GeoLocationQuery.hpp"

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
 * creates a new GeoLocationGeoIp Module, 
 * GeoLocationGeoIp is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
GeoLocationGeoIp::GeoLocationGeoIp(Nepenthes *nepenthes)
{
	m_ModuleName        = "geolocation-geoip";
	m_ModuleDescription = "resolve ips to coordinates using maxdata geoip city";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;
}

GeoLocationGeoIp::~GeoLocationGeoIp()
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
bool GeoLocationGeoIp::Init()
{
#ifdef HAVE_LIBGEOIP
	m_GeoIP = GeoIP_open("/opt/geoip/share/GeoIP/GeoIPCity.dat", GEOIP_INDEX_CACHE);

	if ( g_Nepenthes->getGeoMgr()->registerGeolocationHandler(this) == false)
	{
		logCrit("%s","Could not register as GeolocationHandler\n");
		return false;
	}
	return true;
#else 
	logCrit("%s","Module compiled without libgeoip installed, wont work\n");
	return false;
#endif

}

bool GeoLocationGeoIp::Exit()
{
	return true;
}


bool GeoLocationGeoIp::geoLocate(GeoLocationQuery *query)
{
#ifdef HAVE_LIBGEOIP
	uint32_t ip = query->getAddress();

	char *host = inet_ntoa(*(in_addr *)&ip);
	GeoIPRecord * gir;
	if ( (gir = GeoIP_record_by_name (m_GeoIP, (const char *)host)) != NULL)
	{
		GeoLocationResult *geo = new GeoLocationResult(ip,gir->longitude,gir->latitude,gir->country_code, gir->country_name, gir->city,query->getObject());
		query->getCallback()->locationSuccess(geo);
	}
	return true;
#else
	return false;
#endif
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new GeoLocationGeoIp(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

#endif // HAVE_GEOLOCATION
