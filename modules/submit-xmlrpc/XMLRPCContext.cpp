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

#include "submit-xmlrpc.hpp"
#include "XMLRPCContext.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"
#include "GeoLocationResult.hpp"

using namespace std;

using namespace nepenthes;


XMLRPCContext::XMLRPCContext(string md5sum, string downloadurl, unsigned char *file, uint32_t filesize, uint32_t attackerip, rpcctx_state state)
{
	m_MD5Sum = md5sum;
	m_DownloadURL = downloadurl;
	m_FileBuffer = g_Nepenthes->getUtilities()->b64encode_alloc(file,filesize);;
	m_FileSize = strlen("FOOOOOOO");
	m_AttackerIP = attackerip;
//	m_LocalIP = localip;
	m_State = state;
	printf("State is %i\n",m_State);
}

XMLRPCContext::~XMLRPCContext()
{
	free(m_FileBuffer);
}

rpcctx_state XMLRPCContext::getState()
{
	return m_State;
}

void XMLRPCContext::setState(rpcctx_state state)
{
	m_State = state;
}

string XMLRPCContext::getRequest()
{
	
	string req = "";
	string content = "";


#ifdef HAVE_GEOLOCATION
	char *location;// ="";
	
	string city, shortcountry, longcountry;

	city 			= g_Nepenthes->getUtilities()->escapeXMLString((char *)m_AttackerCity.c_str());
	shortcountry 	= g_Nepenthes->getUtilities()->escapeXMLString((char *)m_AttackerCountryShort.c_str());
	longcountry 	= g_Nepenthes->getUtilities()->escapeXMLString((char *)m_AttackerCountryLong.c_str());


	asprintf(&location,
	
		"<member>"
			"<name>"
				"AttackerLongitude"
			"</name>"
			"<value>"
				"<double>"
					"%f"
				"</double>"
			"</value>"
		"</member>"
		"<member>"
			"<name>"
				"AttackerLatitude"
			"</name>"
			"<value>"
				"<double>"
					"%f"
				"</double>"
			"</value>"
		"</member>"
		 "<member>"
			 "<name>"
				 "AttackerCountry"
			 "</name>"
			 "<value>"
				 "<string>"
					 "%s"
				 "</string>"
			 "</value>"
		 "</member>"
		 "<member>"
			 "<name>"
				 "AttackerCountryShort"
			 "</name>"
			 "<value>"
				 "<string>"
					 "%s"
				 "</string>"
			 "</value>"
		 "</member>"
		 "<member>"
			 "<name>"
				 "AttackerCity"
			 "</name>"
			 "<value>"
				 "<string>"
					 "%s"
				 "</string>"
			 "</value>"
		 "</member>"

			 ,
    		 m_AttackerLongitude,
			 m_AttackerLatitude,
			 longcountry.c_str(),
			 shortcountry.c_str(),
			 city.c_str());
#endif


	switch ( m_State )
	{
	
	// call at startup
	case CS_INIT_SESSION:
		content = 
		"<methodCall>"
			"<methodName>"
				"init_session"
			"</methodName>"
			"<params>"
				"<param>"
					"<value>"
						"<string>"
							"md"				// user
						"</string>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<string>"
							"test"				// sensor-id (e.g. hostname)
						"</string>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<dateTime.iso8601>"
							"20050816T05:22:17"	// now()
						"</dateTime.iso8601>"
					"</value>"
				"</param>"
				"<struct>"
					"<member>"
						"<name>"
							"clientsoftware"
						"</name>"
						"<value>"
							"<string>"
								"Nepenthes submit-xmlrpc $Rev$"
							"</string>"
						"</value>"
					"</member>"
				"</struct>"
			"</params>"
		"</methodCall>";
		break;

	// call when reading malware from disk at startup ? (comandline switch to request that???)
	case CS_OFFER_MALWARE:
		content = 
		"<methodCall>"
			"<methodName>"
				"offer_malware"
			"</methodName>"
			"<params>"
				"<param>"
					"<value>"
						"<string>";
		content += 				m_SessionID;			// sessionid
		content += 		"</string>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<string>";
		content += 				m_MD5Sum;				// md5hash
		content += 		"</string>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<dateTime.iso8601>";
		content += 			"20050816T05:22:17";		// time: first_seen, (can also be now())
		content += 		"</dateTime.iso8601>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<struct>"


							"<member>"
								"<name>"
									"AttackerIP"
								"</name>"
								"<value>"
									"<string>";
				content += 				inet_ntoa(*(in_addr *)&m_AttackerIP);		// attacker ip
				content +=			"</string>"
								"</value>"
							"</member>";
#ifdef HAVE_GEOLOCATION
				content += location;
#endif

				content +=	"<member>"
								"<name>"
									"Url"
								"</name>"
								"<value>"
									"<string>";
				content += 				g_Nepenthes->getUtilities()->escapeXMLString((char *)m_DownloadURL.c_str());
				content +=			"</string>"
								"</value>"
							"</member>"

						"</struct>"
					"</value>"
				"</param>"
			"</params>"
		"</methodCall>";
		break;

	// call when offer_malware() or log download_success() returned True
	case CS_SEND_MALWARE:
		content = 
		"<methodCall>"
			"<methodName>"
					"send_malware"
			"</methodName>"
			"<params>"
				"<param>"
					"<value>"
						"<string>"
							"XXXsessionIDXXX"
						"</string>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<string>";
		content +=	 		m_MD5Sum;	// md5hash
        content +=		"</string>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<base64>";
		content +=			(char *)m_FileBuffer; // file as base64 dump
		content +=		"</base64>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<struct>"

							"<member>"
								"<name>"
									"AttackerIP"
								"</name>"
								"<value>"
									"<string>";
				content += 				inet_ntoa(*(in_addr *)&m_AttackerIP);		// attacker ip
				content +=			"</string>"
								"</value>"
							"</member>";
#ifdef HAVE_GEOLOCATION
				content += location;
#endif
				content +=	"<member>"
								"<name>"
									"Url"
								"</name>"
								"<value>"
									"<string>";
				content += 				g_Nepenthes->getUtilities()->escapeXMLString((char *)m_DownloadURL.c_str());
				content +=			"</string>"
								"</value>"
							"</member>"


						"</struct>"
					"</value>"
				"</param>"
			"</params>"
		"</methodCall>";
		break;
	}

#ifdef HAVE_GEOLOCATION
	free(location);
#endif

	return content;
}

void XMLRPCContext::setSessionID(char *sessionid)
{
	m_SessionID = sessionid;
}

#ifdef HAVE_GEOLOCATION
void XMLRPCContext::setLocation(GeoLocationResult *result)
{

	if (result != NULL)
	{
    	m_AttackerLatitude 	= result->getLatitude();
		m_AttackerLongitude = result->getLongitude();
		m_AttackerCity 		= result->getCity();
		m_AttackerCountryLong 	= result->getCountry();
		m_AttackerCountryShort 	= result->getCountryShort();
	}else
	{
		m_AttackerLatitude 	= 0.0;
		m_AttackerLongitude = 0.0;
		m_AttackerCity 		= "";
		m_AttackerCountryLong 	= "";
		m_AttackerCountryShort 	= "";
	}
}
#endif
