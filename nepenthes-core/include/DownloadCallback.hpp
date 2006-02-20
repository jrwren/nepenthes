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

#ifndef HAVE_DOWNLOADCALLBACK_HPP
#define HAVE_DOWNLOADCALLBACK_HPP

#include <string>

using namespace std;

namespace nepenthes
{
	class Download;


	/**
	 * if we download something and have to use the download internal, 
	 * we assign a downloadcallback
	 * the downloadhandler has to take care of calling the callback 
	 * when the download is done
	 * 
	 * if a download is successfully, and a downloadcallback we call is assigned
	 * we call
	 * downloadSuccess(Download *down)
	 * if it broke
	 * we call 
	 * downloadFailure(Download *down)
	 * 
	 * if no downloadcallback is provided, give it to the submitmanager
	 */
	class DownloadCallback
	{
		public:
		virtual ~DownloadCallback(){};
        virtual void downloadSuccess(Download *down)=0;
		virtual void downloadFailure(Download *down)=0;
	};
}

#endif
