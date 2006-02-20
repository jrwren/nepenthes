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

#ifndef HAVE_DOWNLOADHANDLER_HPP
#define HAVE_DOWNLOADHANDLER_HPP

#include <string>
using namespace std;

namespace nepenthes
{
	class Download;

	/**
	 * if you register to the DownloadManager 
	 * and provide capabilities to download a specific protocoll, 
	 * you are a DownloadHandler
	 */
    class DownloadHandler
    {
    public:
        virtual ~DownloadHandler(){};
        virtual bool Init()=0;
        virtual bool Exit()=0;
		/**
		 * the DownloadManager will call this to ask you to download something
		 * the information where to download is stored in the DownloadUrl in Download
		 * 
		 * @param down   the Download information
		 * 
		 * @return 
		 */
        virtual bool download(Download *down)=0;


		virtual string getDownloadHandlerName()
		{
			return m_DownloadHandlerName;
		}

		virtual string DownloadHandler::getDownloadHandlerDescription()
		{
			return m_DownloadHandlerDescription;
		}

	protected:
		string m_DownloadHandlerName;
		string m_DownloadHandlerDescription;
    };
}

#endif
