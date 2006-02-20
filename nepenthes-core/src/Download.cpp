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
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

Download::Download(char *url,uint32_t address,char *triggerline)
{
	m_Url 			= url;
	m_TriggerLine 	= triggerline;
	m_DownloadUrl 	= new DownloadUrl(url);
	m_DownloadBuffer= new DownloadBuffer();
	m_Address 		= address;
	m_FileType = "";
}

Download::~Download()
{
	logPF();
	delete m_DownloadUrl;
	delete m_DownloadBuffer;
}

void Download::setUrl(string *url)
{
	m_Url = *url;
}

string Download::getUrl()
{
	return m_Url;
}

string Download::getTriggerLine()
{
	return m_TriggerLine;
}

void Download::setMD5Sum(string *s)
{
	m_MD5Sum = *s;
}

string Download::getMD5Sum()
{
	return m_MD5Sum;
}

uint32_t Download::getAddress()
{
	return m_Address;
}

DownloadUrl *Download::getDownloadUrl()
{
	return m_DownloadUrl;
}

DownloadBuffer *Download::getDownloadBuffer()
{
	return m_DownloadBuffer;
}

void Download::setFileType(char *type)
{
	m_FileType = type;
}

string Download::getFileType()
{
	return m_FileType;
}

void  Download::setSHA512(unsigned char *hash)
{
	memcpy(m_SHA512Sum,hash,64);
}

unsigned char * Download::getSHA512()
{
	return m_SHA512Sum;
}

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
