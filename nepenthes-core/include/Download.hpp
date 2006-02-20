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
#ifndef HAVE_DOWNLOAD_HPP
#define HAVE_DOWNLOAD_HPP

#include <string>

using namespace std;

namespace nepenthes
{
	class DownloadUrl;
	class DownloadBuffer;

	class Download
	{
	public:
		Download(char *pszUri, unsigned long ulAddress, char *triggerline);
		virtual ~Download();
		virtual void setUrl(string *url);
		virtual string getUrl();
		virtual string getTriggerLine();
		virtual void   setMD5Sum(string *s);
		virtual string getMD5Sum();
		virtual unsigned long getAddress();
		virtual DownloadUrl *getDownloadUrl();
		virtual DownloadBuffer *getDownloadBuffer();
		virtual void setFileType(char *type);
		virtual string getFileType();
	protected:
		string  m_Url;
		string  m_TriggerLine;
		string  m_MD5Sum;
		string m_FileType;

		unsigned long m_Address;
		DownloadUrl *m_DownloadUrl;
		DownloadBuffer  *m_DownloadBuffer;
	};
}

#endif
