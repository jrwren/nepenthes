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
#ifndef HAVE_DOWNLOADBUFFER_HPP
#define HAVE_DOWNLOADBUFFER_HPP

#include <string>
#include <stdint.h>

using namespace std;

namespace nepenthes
{
	/**
	 * if we download something, we use this buffer class to store the result
	 */
	class DownloadBuffer
	{
	public:
		DownloadBuffer();
		virtual bool Init(uint32_t i);
		virtual ~DownloadBuffer();
		virtual bool addData(char *pszData, uint32_t iDataLen);
		virtual char *getData();
		virtual uint32_t getSize();
		virtual bool cutFront(uint32_t len);
	private:
		char 		*m_Buffer;
		uint32_t   m_BufferSize;
		uint32_t   m_BufferOffset;

	};
}

#endif
