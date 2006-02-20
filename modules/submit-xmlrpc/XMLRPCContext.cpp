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

string XMLRPCContext::getRequest(bool pipeline)
{
	string req = "";
	string content = "";
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
							// empty for now
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
							// so far empty
						"</struct>"
					"</value>"
				"</param>"
			"</params>"
		"</methodCall>";
		break;
		
	// call when writing to logged_submissions
	case CS_LOG_DOWNLOAD_SUCCESS:
		break;

	// call when writing to logged_downloads
	case CS_LOG_DOWNLOAD_ATTEMPT:
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
						"<dateTime.iso8601>"
							"20050816T05:22:17"			// now()
						"</dateTime.iso8601>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<string>";
		content += 				"url://blabla";			// download url
		content += 		"</string>"
					"</value>"
				"</param>"
				"<param>"
					"<value>"
						"<dateTime.iso8601>";
		content += 			"20050816T05:22:17";		// now()
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
		content += 						inet_ntoa(*(in_addr *)&m_AttackerIP);		// attacker ip
		content +=					"</string>"
								"</value>"
							"</member>"
							"<member>"
								"<name>"
									"TargetIP"
								"</name>"
								"<value>"
									"<string>";
		content += 						inet_ntoa(*(in_addr *)&m_AttackerIP);		// FIXME
		content +=					"</string>"
								"</value>"
							"</member>"
							// other stuff to log: module triggered, ports, exploit used, shellcode used -md
							// this information is already lost at this point of time, no way to revert it -common
							
						"</struct>"
					"</value>"
				"</param>"
			"</params>"
		"</methodCall>";
		break;

	}

	req = "POST /";
	req += g_SubmitXMLRPC->getXMLRPCPath();
	req += " HTTP/1.1\r\n";
	req += "Accept: */*\r\n";
	req += "Accept-Encoding: deflate\r\n";
	req += "User-Agent: Nepenthes SubmitXMLRPC \r\n";
	req += "Host: ";
	req += g_SubmitXMLRPC->getXMLRPCHost();
	req += "\r\n";

	req += "Content-Type: text/xml\r\n";

	if ( pipeline == true )
	{
		req += "Connection: Keep-Alive\r\n";
	}



	char contentlen[9];
	memset(contentlen,0,9);
	snprintf(contentlen,8,"%u",(uint32_t)content.size());
	req += "Content-Length: ";
	req += contentlen;
	req += "\r\n";

	req += "\r\n";
	req += content;

//	printf("%s\n",req.c_str());
	return req;
}

void XMLRPCContext::setSessionID(char *sessionid)
{
	m_SessionID = sessionid;
}


