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

#ifndef HAVE_UPLOADCALLBACK_HPP
#define HAVE_UPLOADCALLBACK_HPP

#include <string>

using namespace std;

namespace nepenthes
{
	class UploadResult;


	/**
	 * whenever a Upload is done, 
	 * the UploadHandler will call the UploadCallback 
	 * to inform the issuer his task is completed
	 */
	class UploadCallback
	{
		public:
		virtual ~UploadCallback(){};
        virtual void uploadSuccess(UploadResult *up)=0;
		virtual void uploadFailure(UploadResult *up)=0;
	};
}

#endif
