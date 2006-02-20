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

#include <stdint.h>
#include <string>

using namespace std;

namespace nepenthes
{
	class DownloadUrl;
	class UploadCallback;

	/**
	 * the whole context a upload has is stored in the UploadQuery.
	 */
	class UploadQuery
	{
	public:
		UploadQuery(char *url, char *payload, uint32_t playloadlen, UploadCallback *callback=NULL, void *obj=NULL);
		virtual ~UploadQuery();

		virtual UploadCallback *getCallback();
		virtual void *getObject();

		virtual DownloadUrl *getUploadUrl();
		virtual char		*getBuffer();
		virtual uint32_t 	getSize();
		virtual string 		getUrl();

	protected:
		UploadCallback  *m_UploadCallback;
		void 			*m_Object;
		DownloadUrl 	*m_UploadUrl;

		char			*m_UploadBuffer;
		uint32_t 		m_UploadSize;
		string 			m_Url;
	};
/*
	class HTTPUploadQuery
	{
	public:
		HTTPUploadQuery(UploadCallback *callback, void *obj);
		~HTTPUploadQuery();

		bool addFormData(char *name, char *char *buffer, uint16_t len,


	};
*/	
}
