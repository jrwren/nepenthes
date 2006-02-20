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


#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Buffer.hpp"

using namespace nepenthes;



/**
 * creates an emtpy shellbuffer.
 * 
 * @param initialSize
 *               the initial size of the buffer.
 */
Buffer::Buffer(uint32_t initialSize)
{
	reset();

	if( initialSize )
		resize(initialSize);
}


/**
 * creates an shell buffer containing data.
 * 
 * @param data   pointer to the data.
 * @param size   length of the data.
 */
Buffer::Buffer(void *data, uint32_t size)
{
	reset();
	add(data, size);
}


Buffer::~Buffer()
{
	clear();
}


/**
 * resets buffer vars.
 */
void Buffer::reset()
{
	m_allocSize = 0;
	m_offset = 0;
	m_data = 0;
}


/**
 * clears the buffer.
 */
void Buffer::clear()
{
	if( m_allocSize )
		free(m_data);

	reset();
}


/**
 * resizes the shellbuffer to newSize.
 * 
 * @param newSize the new size, padding to 256byte blocks is performed automatically.
 */
void Buffer::resize(uint32_t newSize)
{
	assert(newSize > m_allocSize);

	// pad to 256B blocks.
	if( newSize % 0x100 )
		newSize += 0x100 - (newSize % 0x100);

//	printf("SB resizing shell buffer from %d to %d\n", m_allocSize, newSize);

	m_data = realloc(m_data, newSize);
	m_allocSize = newSize;
}


/**
 * adds data to the buffer.
 * 
 * @param data   pointer containing data.
 * @param size   size of the data.
 */
void Buffer::add(void *data, uint32_t size)
{
	if( !size )
		return;

//	printf("SB adding %d bytes to buffer\n", size);

	// buffer is empty
	if( !m_allocSize )
	{
		resize(size);
		memcpy(m_data, data, size);
	}
	else
	{
		// realloc needed?
		if( m_offset + size > m_allocSize )
		{
			uint32_t newSize = m_allocSize;

			// keep doubling the buffer size until it fits.
			while( m_offset + size > newSize )
				newSize <<= 1;

			resize(newSize);
		}
		
		memcpy((void *)((intptr_t)m_data + m_offset), data, size);
	}

	m_offset += size;
}


/**
 * adds a zero-terminated string to the buffer.
 * 
 * @param str    zero-terminated string.
 */
void Buffer::addString(const char *str)
{
	printf("adding \"%s\"\n",str);
	add((void *)str, strlen(str));
}


/**
 * cuts size bytes from the begining or end (negative size)
 * 
 * @param size   number of bytes to cut, use negative values to cut from the back.
 */
void Buffer::cut(int32_t size)
{
	assert(size <= (int32_t)m_offset);

	if( size > 0 )
	{
		//memcpy(m_data, (void *)((int32_t)m_data + size), m_offset - size);
		memmove(m_data, (void *)((intptr_t)m_data + size), m_offset - size);
		m_offset -= size;
	}
}


/**
 * returns a pointer to the data.
 * 
 * @return pointer to the data.
 */
void *Buffer::getData()
{
	return m_data;
}


/**
 * returns the size of the buffer.
 * 
 * @return size of the buffer.
 */
uint32_t Buffer::getSize()
{
	return m_offset;
}


/**
 * print debug information.
 */
void Buffer::debug()
{
	printf("SB shellbuffer debug\n");
	printf("  > m_data = 0x%08x\n", (int)((intptr_t)m_data));
	printf("  > m_offset = %d\n", (int)m_offset);
	printf("  > m_allocSize = %d\n", (int)m_allocSize);
}

