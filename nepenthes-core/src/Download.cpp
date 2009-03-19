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

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "EventManager.hpp"
#include "LogManager.hpp"
#include "Nepenthes.hpp"
#include "SubmitEvent.hpp"

#include <cstring>

using namespace nepenthes;

/**
 * constructor for Download
 * 
 * @param url      the url to download
 * @param address  the attackers ip address
 * @param triggerline
 *                 the triggerline for the download
 * @param callback the DownloadCallback (if used)
 * @param obj      the additional data (if used)
 */
Download::Download(uint32_t localhost, char *url,uint32_t address,const char *triggerline,DownloadCallback *callback, void *obj)
{
	m_Url 			= url;
	m_TriggerLine 	= triggerline;
	m_DownloadUrl 	= new DownloadUrl(url);
	m_DownloadBuffer= new DownloadBuffer();
	m_RemoteHost	= address;
	m_LocalHost = localhost;
	m_FileType = "";
	m_DownloadFlags = 0;

	m_DownloadCallback = callback;
	m_Object = obj;

	SubmitEvent se(EV_DOWNLOAD,this);
	g_Nepenthes->getEventMgr()->handleEvent(&se);
}

/**
 * destructor for Download
 */
Download::~Download()
{
	logPF();

	SubmitEvent se(EV_DOWNLOAD_DESTROYED, this);
	g_Nepenthes->getEventMgr()->handleEvent(&se);

	delete m_DownloadUrl;
	delete m_DownloadBuffer;
}

/**
 * set the DownloadUrl
 * 
 * @param url    the new url
 */
void Download::setUrl(string *url)
{
	m_Url = *url;
}

/**
 * get the Url to download 
 * 
 * @return returns the url to download as string
 */
string Download::getUrl()
{
	return m_Url;
}

/**
 * get the downloads triggerline
 * 
 * @return returns the downloads triggerline as string
 */
string Download::getTriggerLine()
{
	return m_TriggerLine;
}

/**
 * set the DownloadBuffer 's md5sum
 * 
 * @param s      the md5hash
 */
void Download::setMD5Sum(string *s)
{
	m_MD5Sum = *s;
}

/**
 * get the DownloadBuffers md5sum
 * 
 * @return the md5hash in hex as string
 */
string Download::getMD5Sum()
{
	return m_MD5Sum;
}

/**
 * get the attackers ip
 * 
 * @return the attackers ip
 */
uint32_t Download::getRemoteHost()
{
	return m_RemoteHost;
}


/**
 * get the local ip for the download
 * 
 * @return the attackers ip
 */
uint32_t Download::getLocalHost()
{
	return m_LocalHost;
}


/**
 * get the DownloadUrl
 * 
 * @return returns pointer to the DownloadUrl
 */
DownloadUrl *Download::getDownloadUrl()
{
	return m_DownloadUrl;
}

DownloadBuffer *Download::getDownloadBuffer()
{
	return m_DownloadBuffer;
}

/**
 * set DownloadBuffer 's filetype
 * 
 * @param type   the filetype
 */
void Download::setFileType(char *type)
{
	m_FileType = type;
}

/**
 * get the DownloadBuffers Filetype
 * 
 * @return returns the filetype as string
 */
string Download::getFileType()
{
	return m_FileType;
}

/**
 * set the DownloadBuffers sha512 hash 
 * 
 * @param hash   the sh512 hash
 */
void  Download::setSHA512(unsigned char *hash)
{
	memcpy(m_SHA512Sum,hash,64);
}

/**
 * get the DownloadBuffer 's sha512 hash as binary data
 * 
 * @return pointer to 64 byte binary data sha512 hash
 */
unsigned char * Download::getSHA512()
{
	return m_SHA512Sum;
}

/**
 * get the DownloadBuffer 's sha512 hash as string
 * 
 * @return returns the DownloadBuffer 's sha512 hash as string
 */
string  Download::getSHA512Sum()
{
	string s;
	string SHA512Sum ="";

	for(uint32_t i = 0; i < 64; ++i)
	{
		SHA512Sum += ((m_SHA512Sum[i] >> 4) < 10 ? (m_SHA512Sum[i] >> 4) + '0' : (m_SHA512Sum[i] >> 4) + ('a' - 10));
		SHA512Sum += ((m_SHA512Sum[i] & 0xF) < 10 ? (m_SHA512Sum[i] & 0xF) + '0' : (m_SHA512Sum[i] & 0xF) + ('a' - 10));
	}
	return SHA512Sum;
}



/**
 * add DownloadFlags to the download
 * 
 * @param flag   the DownloadFlags
 */
void Download::addDownloadFlags(uint8_t flag)
{
	m_DownloadFlags |= flag;
}

/**
 * get the Download 's DownloadFlags
 * 
 * @return returns Download 's DownloadFlags
 */
uint8_t Download::getDownloadFlags()
{
	return m_DownloadFlags;
}

/**
 * get the Download 's DownloadCallback
 * 
 * @return returns the DownloadCallback
 */
DownloadCallback *Download::getCallback()
{
	return m_DownloadCallback;
}

/**
 * get the additional data
 * 
 * @return returns pointer to the additional data
 */
void *Download::getObject()
{
	return m_Object;
}
