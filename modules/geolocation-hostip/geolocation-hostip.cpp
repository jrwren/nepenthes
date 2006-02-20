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

#include "geolocation-hostip.hpp"
#include "GeoDialogue.hpp"

#include "SocketManager.hpp"
#include "TCPSocket.hpp"

#include "LogManager.hpp"

#include "Config.hpp"

#include "GeoLocationManager.hpp"
#include "GeoLocationQuery.hpp"

#include "GeoLocationResult.hpp"
#include "GeoLocationResult.cpp"

#include "GeoLocationCallback.hpp"

#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "DownloadManager.hpp"

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
 * creates a new GeoLocationHostIp Module, 
 * GeoLocationHostIp is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
GeoLocationHostIp::GeoLocationHostIp(Nepenthes *nepenthes)
{
	m_ModuleName        = "geolocation-hostip";
	m_ModuleDescription = "resolve ips to coordinates using hostip.info";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;
}

GeoLocationHostIp::~GeoLocationHostIp()
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
bool GeoLocationHostIp::Init()
{
	if ( g_Nepenthes->getGeoMgr()->registerGeolocationHandler(this) == false)
	{
		logCrit("%s","Could not register as GeolocationHandler\n");
		return false;
	}
	return true;
}

bool GeoLocationHostIp::Exit()
{
	return true;
}


bool GeoLocationHostIp::geoLocate(GeoLocationQuery *query)
{
	uint32_t ip = query->getAddress();
	logSpam("GeoLocationHostIp looking up info for %x %s %x\n",query->getCallback(),inet_ntoa(*(in_addr *)&ip),query->getObject());

	string url = "http://www.hostip.info/api/get.html?ip=";
	url += inet_ntoa(*(in_addr *)&ip);
	url += "&position=true";

	g_Nepenthes->getDownloadMgr()->downloadUrl(INADDR_ANY, (char *)url.c_str(),0,"internal usage",DF_INTERNAL_DOWNLOAD,this,query);

	return true;
}

void GeoLocationHostIp::downloadSuccess(Download *down)
{
	logPF();
	GeoLocationQuery *query = (GeoLocationQuery *)down->getObject();

//    string data(down->getDownloadBuffer()->getData(),down->getDownloadBuffer()->getSize());

//	logSpam("FOO\n%s\n",(char *)down->getDownloadBuffer()->getData());


	uint32_t size = down->getDownloadBuffer()->getSize();

	char *data = (char *)malloc(down->getDownloadBuffer()->getSize()+1);
	memset(data,0,size+1);
	memcpy(data,down->getDownloadBuffer()->getData(),size);

	uint32_t i=0;


	typedef struct 
	{
		char *m_Value;
		const char *m_Type;
	} mystruct;

	mystruct test[4]=
	{
		{ 	NULL, 	"Country: " },
		{ 	NULL, 	"City: " },
        { 	NULL, 	"Longitude: " },
		{ 	NULL, 	"Latitude: " }
	};

	uint32_t j;
	for (j=0;j<4;j++)
	{
		test[j].m_Value = strstr(data, test[j].m_Type);
		if (test[j].m_Value != NULL && test[j].m_Value + strlen(test[j].m_Type) < data +size)
		{
			test[j].m_Value += strlen(test[j].m_Type);
		}else
		{
			test[j].m_Value = NULL;
		}
	}

	for (i=0;i<size;i++)
	{
		if (data[i] == '\r' || data[i] == '\n' )
		{
			data[i] = '\0';
		}
	}


	for (j=0;j<4;j++)
	{
		if (test[j].m_Value != NULL && strlen(test[j].m_Value) == 0 )
		{
			test[j].m_Value = NULL;
		}
//		printf("%s -> '%s' \n",test[j].m_Type,test[j].m_Value);
	}

	float lat,len;
	uint32_t address = query->getAddress();
	char *city,*country;

	if (test[2].m_Value != NULL)
	{
		len = strtof(test[2].m_Value,NULL);
	}else
	{
		len = 0.0;
	}

	if (test[3].m_Value != NULL)
	{
		lat = strtof(test[3].m_Value,NULL);
	}else
	{
		lat = 0.0;
	}

	city = test[1].m_Value;
	country = test[0].m_Value;

	char *longcountry = NULL;
	char *shortcountry = NULL;

	

	longcountry = country;

	uint32_t strl = strlen(country);

	for (i=0; i<=strl;i++)
	{
		printf(".");
		if (country[i] == '(')
		{
			logSpam("FOUND ((( %i\n",i);
			country[i-1] = '\0';
			country[i] = '\0';
			shortcountry = country+i+1;
		}else
		if (country[i] == ')')
		{
			logSpam("FOUND ))) %i\n",i);
			country[i] = '\0';
		}
	}

	logSpam("CountryShort %s\n",shortcountry);
	logSpam("CountryLong %s\n",longcountry);

	GeoLocationResult *result = new GeoLocationResult(address,len,lat,longcountry,shortcountry,city,query->getObject());
	g_Nepenthes->getGeoMgr()->cacheGeoLocation(result);
	query->getCallback()->locationSuccess(result);
	delete result;
	delete query;

	free(data);
	
}

void GeoLocationHostIp::downloadFailure(Download *down)
{
	logPF();

}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new GeoLocationHostIp(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

#endif // HAVE_GEOLOCATION
