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


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "Manager.hpp"
#include "GeoLocationManager.hpp"
#include "GeoLocationQuery.hpp"
#include "GeoLocationResult.hpp"
#include "GeoLocationCallback.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Config.hpp"

using namespace nepenthes;




/**
 * GeoLocationManager constructor
 * 
 * @param nepenthes the nepenthes Instance
 */
GeoLocationManager::GeoLocationManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
	m_GeoLocationHandler = NULL;
}

/**
 * GeoLocationManager destructor
 */
GeoLocationManager::~GeoLocationManager()
{
	logPF();
}

/**
 * ask the GeoLocationManager to perform a ip lookup
 * 
 * @param callback the callback who shall get the GeoLocationResult
 * @param ip       the ip to lookup
 * @param obj      additional data
 * 
 * @return returns true
 */
bool GeoLocationManager::addGeoLocation(GeoLocationCallback *callback,uint32_t ip, void *obj)
{
	logPF();
	string path = m_CachePath;
	
	string adir,bdir;

	char *address = inet_ntoa(*(in_addr *)&ip);
	char *addcpy = strdup(address);

	char *a = strtok(addcpy,".");
	char *b = strtok(NULL,".");
	

	path += a;
	path += "/";

    adir = path;
	
	path += a;
	path += ".";
	path += b;
	path += "/";

	bdir = path;

	path += address;


//	int stat(const char *file_name, struct stat *buf);

	// check full path

	struct stat s;
	if (stat(path.c_str(),&s) == 0)
	{ 
		// FIXME needs some work
		// float:lat float:len str:CountryLong str:CountryShort str:City
		
		logInfo("Using cached result for %s\n",address);
        FILE *f;
		if ( (f = fopen(path.c_str(),"rb")) != NULL )
		{

			float lat,len;
			char *city;
			char *shortcountry, *longcountry;

			uint32_t strl;

			fread(&lat,4,1,f);
			logSpam("CACHE READ LAT %f\n",lat);

			fread(&len,4,1,f);
			logSpam("CACHE READ LNG %f\n",lat);

			fread(&strl,4,1,f);
			logSpam("CACHE READ STRLEN %i\n",strl);
			longcountry = (char *)malloc(strl+1);
			memset(longcountry,0,strl+1);
			fread(longcountry,strl,1,f);
			logSpam("CACHE READ STR '%s'\n",longcountry);;

			fread(&strl,4,1,f);
			logSpam("CACHE READ STRLEN %i\n",strl);;
			shortcountry = (char *)malloc(strl+1);
			memset(shortcountry,0,strl+1);
			fread(shortcountry,strl,1,f);
			logSpam("CACHE READ STR '%s'\n",shortcountry);;

			fread(&strl,4,1,f);
			logSpam("CACHE READ STRLEN %i\n",strl);;
			city = (char *)malloc(strl+1);
			memset(city,0,strl+1);
			fread(city,strl,1,f);
			logSpam("CACHE READ STR '%s'\n",city);;

			fclose(f);

			GeoLocationResult result(ip,len,lat,longcountry, shortcountry,city,obj);

			callback->locationSuccess(&result);

			free(longcountry);
			free(shortcountry);
			free(city);

		}
	} else
	{
		logSpam("Adding %x %s %x to geolookup\n",callback,inet_ntoa(*(in_addr *)&ip),obj);
		GeoLocationQuery *query = new GeoLocationQuery(ip,callback,obj);
		m_GeoLocationHandler->geoLocate(query);
	}
	free(addcpy);
	return true;
}

/**
 * register a GeoLocationHandler
 * 
 * @param handler the handler to register
 * 
 * @return returns true if no other GeoLocationHandler was registerd before
 *         else false
 */
bool GeoLocationManager::registerGeolocationHandler(GeoLocationHandler *handler)
{
	if (m_GeoLocationHandler == NULL)
	{
		m_GeoLocationHandler = handler;
		return true;
	}else
	{
		logCrit("%s","GeoLocationManager already has a handler\n");
		return false;
	}
}


/**
 * cache a GeoLocationResult
 * 
 * @param result the result to cache
 * 
 * @return returns FIXME
 */
bool GeoLocationManager::cacheGeoLocation(GeoLocationResult *result)
{

	string path = m_CachePath;
	string adir,bdir;

	uint32_t ip = result->getAddress();


	char *address = inet_ntoa(*(in_addr *)&ip);
	char *addcpy = strdup(address);

	char *a = strtok(addcpy,".");
	char *b = strtok(NULL,".");
	

	path += a;
	path += "/";

    adir = path;
	
	path += a;
	path += ".";
	path += b;
	path += "/";

	bdir = path;

	path += address;


//	int stat(const char *file_name, struct stat *buf);

	// check full path

	struct stat s;
	if (stat(path.c_str(),&s) == -1)
	{
		logSpam("Path %s\n",strerror(errno));
		if (stat(bdir.c_str(),&s) == -1)
		{
			logSpam("B Dir %s\n",strerror(errno));
			if (stat(adir.c_str(),&s) == -1)
			{
				logSpam("A Dir %s\n",strerror(errno));
				if (mkdir(adir.c_str(),0755) == -1)
				{
					logCrit("Could not create %s (%s)\n",adir.c_str(),strerror(errno));
				}
			}
			if (mkdir(bdir.c_str(),0755) == -1)
			{
				logCrit("Could not create %s (%s)\n",bdir.c_str(),strerror(errno));
			}
		}
		logSpam("CACHING %s\n",address);

		// FIXME needs some work
		
		FILE *f;

		if ( (f = fopen(path.c_str(),"wb")) != NULL )
		{
			float lat,len;
			char *city;
			char *shortcountry, *longcountry;

			lat = result->getLatitude();
			len = result->getLongitude();

			city = strdup(result->getCity().c_str());

			longcountry = strdup(result->getCountry().c_str());

			shortcountry = strdup(result->getCountryShort().c_str());

			uint32_t strl;


			logSpam("CACHE WRITE LAT %f\n",lat);
			fwrite(&lat,1,4,f);

			logSpam("CACHE WRITE LNG %f\n",lat);
			fwrite(&len,1,4,f);

			strl = strlen(longcountry);
			logSpam("CACHE WRITE STRLEN %i\n",strl);;
			fwrite(&strl,1,4,f);
			logSpam("CACHE WRITE STR '%s'\n",longcountry);;
			fwrite(longcountry,1,strlen(longcountry),f);

			strl = strlen(shortcountry);
			logSpam("CACHE WRITE STRLEN %i\n",strl);;
			fwrite(&strl,1,4,f);
			logSpam("CACHE WRITE STR '%s'\n",shortcountry);;
			fwrite(shortcountry,1,strlen(shortcountry),f);

			strl = strlen(city);
			logSpam("CACHE WRITE STRLEN %i\n",strl);;
			fwrite(&strl,1,4,f);
			logSpam("CACHE WRITE STR '%s'\n",city);;
			fwrite(city,1,strlen(city),f);

			fclose(f);

			free(longcountry);
			free(shortcountry);
			free(city);

		}
	}

	logSpam("%s A: '%s' B: '%s' PATH '%s'\n",address,a,b,path.c_str());
	free(addcpy);
	return true;
}


/**
 * list the attached GeoLocationHandler
 */
void GeoLocationManager::doList()
{

}

/**
 * Init the GeoLocationManager
 * 
 * @return returns true if a GeoLocationHandler is attached
 *         else false
 */
bool GeoLocationManager::Init()
{
	logPF();
	if (m_GeoLocationHandler == NULL)
	{
		logCrit("%s","NO GeoLocationHandler registerd, load a module\n");
		return false;
	}

	try
	{
		m_CachePath = g_Nepenthes->getConfig()->getValString("nepenthes.geolocationmanager.cache_path");
		logInfo("Cache Path is '%s'\n",m_CachePath.c_str());
	}catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}
	return true;
}

/**
 * Exits the GeoLocationManager
 * 
 * @return returns true
 */
bool GeoLocationManager::Exit()
{
	return true;
}


#endif // HAVE_GEOLOCATION
