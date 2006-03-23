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

#ifndef HAVE_UPLOADHANDLER_HPP
#define HAVE_UPLOADHANDLER_HPP

#include <string>
using namespace std;

namespace nepenthes
{
	class UploadQuery;

	/**
	 * intrested in uploading?
	 * implement a protocol and register as a UploadHandler on your UploadManager
	 * he will give you UploadQuery 's for your protocoll and you will inform the UploadCallback with the UploadResult
	 * on success
	 */
    class UploadHandler
    {
    public:
        virtual ~UploadHandler(){};
        virtual bool Init()=0;
        virtual bool Exit()=0;
        virtual bool upload(UploadQuery *up)=0;


		virtual string getUploadHandlerName()
		{
			return m_UploadHandlerName;
		}

		virtual string getUploadHandlerDescription()
		{
			return m_UploadHandlerDescription;
		}

	protected:
		string m_UploadHandlerName;
		string m_UploadHandlerDescription;
    };
}

#endif
