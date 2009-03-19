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
#include <stdint.h>

//#include "EvCID.hpp"
using namespace std;



#define DF_TYPE_BINARY			0x0000001
#define DF_INTERNAL_DOWNLOAD 	0x0000002

namespace nepenthes
{
	class DownloadUrl;
	class DownloadBuffer;
	class DownloadCallback;

	/**
	 * whenever we download something, we have to store the information where to download,
	 * and have to store the stuff we downloaded.
	 * 
	 * the downloads address is saved in DownloadUrl
	 * the downloads result is stored in DownloadBuffer
	 */
	class Download //: public EvCID
	{
	public:
		Download(uint32_t localhost, char *pszUri, uint32_t ulAddress, const char *triggerline, DownloadCallback *callback=NULL, void *obj=NULL);
		virtual ~Download();
		virtual void 			setUrl(string *url);
		virtual string 			getUrl();
		virtual string 			getTriggerLine();

		virtual void   			setMD5Sum(string *s);
		virtual string 			getMD5Sum();

		virtual void 			setSHA512(unsigned char *hash);
		virtual unsigned char 	*getSHA512();
		virtual string 			getSHA512Sum();

		virtual uint32_t 		getRemoteHost();
		virtual uint32_t 		getLocalHost();

		virtual DownloadUrl 	*getDownloadUrl();
		virtual DownloadBuffer 	*getDownloadBuffer();
		virtual void 			setFileType(char *type);
		virtual string 			getFileType();

		uint8_t					getDownloadFlags();
		void					addDownloadFlags(uint8_t flags);

		virtual DownloadCallback *getCallback();
		virtual void 			*getObject();

	protected:
		string  			m_Url;
		string  			m_TriggerLine;


		string  			m_MD5Sum;
		unsigned char 		m_SHA512Sum[64];

		string 				m_FileType;

		uint32_t 			m_RemoteHost;
		uint32_t 			m_LocalHost;

		DownloadUrl 		*m_DownloadUrl;
		DownloadBuffer  	*m_DownloadBuffer;

		uint8_t				m_DownloadFlags;

		DownloadCallback 	*m_DownloadCallback;
		void 				*m_Object;
	};
}

#endif
