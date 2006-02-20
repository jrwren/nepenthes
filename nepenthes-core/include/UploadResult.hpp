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
#ifndef HAVE_UPLOADRESULT_HPP
#define HAVE_UPLOADRESULT_HPP

#include <stdint.h>


namespace nepenthes
{
	/**
	 * the UploadResult will inform the UploadCallback what we got in exchange for our upload.
	 */
	class UploadResult
	{
	public:
		UploadResult(char *result, uint32_t size, void *obj);
		virtual ~UploadResult();
		virtual void 		*getObject();
		virtual char 		*getData();
		virtual uint32_t 	getSize();

	protected:
		void 		*m_Object;
		uint32_t 	m_Size;
		char 		*m_Data;
	};
}

#endif 

