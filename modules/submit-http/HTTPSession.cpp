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


#include <curl/curl.h>
#include <curl/types.h> /* new for v7 */
#include <curl/easy.h> /* new for v7 */
#include <sstream>
#include <netinet/in.h>

#include "HTTPSession.hpp"
#include "submit-http.hpp"
#include "DownloadBuffer.hpp"
#include "DownloadUrl.hpp"

using namespace nepenthes;

HTTPSession::HTTPSession(string &url, string &email, string &user, string &password, Download* down)
{
	state = S_ERROR;
	postInfo = NULL;
	postFile = NULL;
	curlInfoHandle = NULL;
	curlFileHandle = NULL;

	submitURL = url;

	if ( user.length() > 0 && password.length() > 0 )
		submitAuthStr = user + ":" + password;

	md5 = down->getMD5Sum();
	sha512 = down->getSHA512Sum();
	fileSize = down->getDownloadBuffer()->getSize();
	fileName = down->getDownloadUrl()->getFile();
	fileSourceURL = down->getUrl();

	fileBuffer = new uint8_t[fileSize];
	fileBuffer = (uint8_t*)memcpy(fileBuffer, down->getDownloadBuffer()->getData(), fileSize);

	curlInfoHandle = curl_easy_init();
	if ( curlInfoHandle )
	{
		struct curl_httppost* last = NULL;

		if ( email.length() > 0 )
			curl_formadd(&postInfo, &last, CURLFORM_COPYNAME, "email", CURLFORM_COPYCONTENTS, email.c_str(), CURLFORM_END);

		stringstream sSourceHost; sSourceHost << htonl(down->getRemoteHost());
		stringstream sTargetHost; sTargetHost << htonl(down->getLocalHost());

		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "url", CURLFORM_COPYCONTENTS, fileSourceURL.c_str(), CURLFORM_END);
		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "trigger", CURLFORM_COPYCONTENTS, down->getTriggerLine().c_str(), CURLFORM_END);
		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "md5", CURLFORM_COPYCONTENTS, md5.c_str(), CURLFORM_END);
		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "sha512", CURLFORM_COPYCONTENTS, sha512.c_str(), CURLFORM_END);
		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "filetype", CURLFORM_COPYCONTENTS, down->getFileType().c_str(), CURLFORM_END);
		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "source_host", CURLFORM_COPYCONTENTS, sSourceHost.str().c_str(), CURLFORM_END);
		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "target_host", CURLFORM_COPYCONTENTS, sTargetHost.str().c_str(), CURLFORM_END);
		curl_formadd(&postInfo, &last, CURLFORM_PTRNAME, "filename", CURLFORM_COPYCONTENTS, down->getDownloadUrl()->getFile().c_str(), CURLFORM_END);

		setCURLOpts(curlInfoHandle, postInfo);
	}
}

HTTPSession::~HTTPSession()
{
	delete [] fileBuffer;
	curl_formfree(postInfo);
	if ( postFile )
		curl_formfree(postFile);
	curl_easy_cleanup(curlInfoHandle);
	if ( curlFileHandle )
		curl_easy_cleanup(curlFileHandle);
}

CURL* HTTPSession::getSubmitInfoHandle()
{
	return curlInfoHandle;
}               

CURL* HTTPSession::getSubmitFileHandle()
{
	curlFileHandle = curl_easy_init();
	if ( curlFileHandle )
	{
		postFile = NULL;
		struct curl_httppost* last = NULL;

		curl_formadd(&postFile, &last, CURLFORM_PTRNAME, "md5", CURLFORM_COPYCONTENTS, md5.c_str(), CURLFORM_END);
		curl_formadd(&postFile, &last, CURLFORM_PTRNAME, "sha512", CURLFORM_COPYCONTENTS, sha512.c_str(), CURLFORM_END);

		curl_formadd(&postFile, &last,
					 CURLFORM_COPYNAME, "file",
					 CURLFORM_BUFFER, fileName.c_str(),
					 CURLFORM_BUFFERPTR, fileBuffer,
					 CURLFORM_BUFFERLENGTH, fileSize,
					 CURLFORM_END);

		setCURLOpts(curlFileHandle, postFile);
	}
	return curlFileHandle;
}               

string HTTPSession::getMD5()
{
	return md5;
}               

string HTTPSession::getSHA512()
{
	return sha512;
}               

void HTTPSession::setCURLOpts(CURL* c, curl_httppost* post)
{
	curl_easy_setopt(c, CURLOPT_HTTPPOST, post);
	curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, false);
	curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(c, CURLOPT_URL, submitURL.c_str());
	curl_easy_setopt(c, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; nepenthes; Linux)");
	curl_easy_setopt(c, CURLOPT_PRIVATE, (char*) this);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, HTTPSession::WriteCallback);

	if ( submitAuthStr.length() > 0 )
		curl_easy_setopt(c, CURLOPT_USERPWD, submitAuthStr.c_str());
}

size_t HTTPSession::WriteCallback(char *buffer, size_t size, size_t nitems, void *p)
{
	HTTPSession* s = (HTTPSession*)p;
	int32_t iSize = size * nitems;
	
	string res(buffer, iSize);
	if ( res.find("S_FILEREQUEST") != string::npos )
		s->setState(S_FILEREQUEST);
	else
		if ( res.find("S_FILEKNOWN") != string::npos )
		s->setState(S_FILEKNOWN);
	else
		if ( res.find("S_FILEOK") != string::npos )
		s->setState(S_FILEOK);
	else
		s->setState(S_ERROR);

//	delete(strBuf);
	return iSize;
}

uint8_t HTTPSession::getState()
{
	return state;
}

void HTTPSession::setState(uint8_t s)
{
	this->state = s;
}

string HTTPSession::getFileSourceURL()
{
	return fileSourceURL;
}
