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
#ifndef HAVE_BUFFER_HPP
#define HAVE_BUFFER_HPP

namespace nepenthes
{
	/**
	 * generic buffer, holding strings or binary data.
	 */
	class Buffer
	{
	public:
		Buffer(unsigned int intialSize = 0);
		Buffer(void *data, unsigned int size);
		virtual         ~Buffer();

		virtual void            clear();
		virtual void            add(void *data, unsigned int size);
		virtual void            addString(const char *str);
		virtual unsigned int    getSize();
		virtual void            *getData();
		virtual void            cut(int size);

		virtual void            debug();

	private:
		virtual void            resize(unsigned int newSize);
		virtual void            reset();

		void            *m_data;
		unsigned int    m_offset;
		unsigned int    m_allocSize;
	};
}

#endif
