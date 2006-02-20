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


#include "UploadQuery.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "DownloadUrl.hpp"

using namespace std;
using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_mgr


/**
 * UploadQuery constructor
 * 
 * @param url      the url where to upload
 * @param payload  the payload to upload
 * @param size     the payloads size
 * @param callback the UploadCallback to call when the UploadQuery is done
 * @param obj      the additional data
 */
UploadQuery::UploadQuery(char *url, char *payload, uint32_t size, UploadCallback *callback, void *obj)
{
	m_Url 			= url;
	m_UploadUrl 	= new DownloadUrl(url);
	m_UploadBuffer = (char *)malloc(size);
	memcpy(m_UploadBuffer,payload,size);
	m_UploadSize = size;

	m_UploadCallback = callback;
	m_Object = obj;
}

UploadQuery::~UploadQuery()
{
	free(m_UploadBuffer);
	delete m_UploadUrl;
}

/**
 * get the UploadCallback
 * 
 * @return returns the UploadCallback
 */
UploadCallback *UploadQuery::getCallback()
{
	return m_UploadCallback;
}

/**
 * get the additional data
 * 
 * @return returns pointer to the additional data
 */
void *UploadQuery::getObject()
{
	return m_Object;
}

/**
 * get the DownloadUrl 
 * 
 * @return returns the DownloadUrl for the Upload
 */
DownloadUrl *UploadQuery::getUploadUrl()
{
	return m_UploadUrl;
}

/**
 * get the buffer to upload
 * 
 * @return returns the buffer
 */
char *UploadQuery::getBuffer()
{
	return m_UploadBuffer;
}

/**
 * get the upload buffers size
 * 
 * @return returns the upload buffers size
 */
uint32_t UploadQuery::getSize()
{
	return m_UploadSize;
}

/**
 * get the uploads url
 * 
 * @return returns the uploads url as string
 */
string UploadQuery::getUrl()
{
	return m_Url;
}
