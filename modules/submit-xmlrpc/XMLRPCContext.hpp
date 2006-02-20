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

namespace nepenthes
{
	typedef enum
	{
		CS_INIT_SESSION,
		CS_OFFER_MALWARE,
		CS_SEND_MALWARE,
	} rpcctx_state;

	class GeoLocationResult;

	class XMLRPCContext
	{
	public:
		XMLRPCContext(string md5sum, string downloadurl, unsigned char *file, uint32_t filesize, uint32_t attackerip, rpcctx_state state);
		~XMLRPCContext();
		rpcctx_state getState();
		void setState(rpcctx_state state);
		string getRequest();
		void setSessionID(char *sessionid);

#ifdef HAVE_GEOLOCATION
		void setLocation(GeoLocationResult *result);
#endif

	protected:
		string m_MD5Sum;
		string m_DownloadURL;

		unsigned char *m_FileBuffer;
		uint32_t 	m_FileSize;
		uint32_t	m_AttackerIP;
		float 		m_AttackerLatitude;
		float 		m_AttackerLongitude;

#ifdef HAVE_GEOLOCATION
		string 		m_AttackerCity;
		string 		m_AttackerCountryShort;
		string 		m_AttackerCountryLong;
#endif

		rpcctx_state m_State;
		string m_SessionID;
	};
}
