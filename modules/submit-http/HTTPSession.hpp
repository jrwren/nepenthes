/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2006 Niklas Schiffler <nick@digitician.eu> 
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
 
#include <curl/curl.h>
#include <curl/types.h>

#include "Download.hpp"

using namespace std;

namespace nepenthes
{


	class HTTPSession
	{
	public:
		static const uint8_t S_FILEKNOWN   = 0;
		static const uint8_t S_FILEREQUEST = 1;
		static const uint8_t S_FILEOK      = 2;
		static const uint8_t S_FILEPENDING = 3;
		static const uint8_t S_ERROR       = 4;

		HTTPSession(string &url, string &email, string &user, string &password, Download* down);
		~HTTPSession();
		CURL* getSubmitInfoHandle();
		CURL* getSubmitFileHandle();
		string getMD5();
		string getSHA512();
		void setCURLOpts(CURL* c, curl_httppost* post);
		uint8_t getState();
		void setState(uint8_t s);
		string getFileSourceURL();

		static size_t WriteCallback(char *buffer, size_t size, size_t nitems, void *userp);

	protected:
		CURL* curlInfoHandle;
		CURL* curlFileHandle;
		uint8_t* fileBuffer;
		size_t fileSize;        
		struct curl_httppost* postInfo;
		struct curl_httppost* postFile;
		string fileName;
		string fileSourceURL;
		string md5;
		string sha512;
		string submitURL;
		string submitAuthStr;
		uint8_t state;

	};

}

