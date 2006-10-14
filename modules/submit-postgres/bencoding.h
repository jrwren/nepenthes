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

#ifndef HAVE_BENCODING_H
#define HAVE_BENCODING_H

#include <stdint.h>
#include <stddef.h>

#define ERRLEN 0xff

typedef enum
{
	Bencoding_TypeInt,
	Bencoding_TypeString,
	Bencoding_TypeList,
	Bencoding_TypeDict
} Bencoding_ItemType;


typedef struct
{
	void *	m_data;
	size_t	m_len;
} Bencoding_String;

typedef int32_t Bencoding_Int;

struct Bencoding_Item_s;
typedef struct Bencoding_Item_s Bencoding_Item;

typedef struct
{
	uint16_t			m_size;
	uint16_t			m_capacity;
	Bencoding_Item *	m_vector;
} Bencoding_List;

typedef struct
{
	uint16_t			m_size;
	uint16_t			m_capacity;
	Bencoding_String *	m_keys;
	Bencoding_Item *	m_values;
} Bencoding_Dict;

struct Bencoding_Item_s
{
	Bencoding_ItemType	m_type;

	union
	{
		Bencoding_Int		m_int;
		Bencoding_String	m_string;
		Bencoding_List		m_list;
		Bencoding_Dict		m_dict;
	};
};

/* used for the top-level only. */
typedef struct
{
	uint16_t			m_size;
	uint16_t			m_capacity;
	uint16_t			m_iterator;
	Bencoding_Item *	m_vector;
} Bencoding_ItemVector;

typedef struct
{
	void *					m_buffer;
	size_t					m_len;

	uint8_t *				m_ptr;
	size_t					m_offset;

	Bencoding_ItemVector	m_items;

	char					m_errorMessage[ERRLEN];
} Bencoding_Context;



/* Create a new Bencoding context, return 0 on oom. */
extern Bencoding_Context *	Bencoding_createContext();

/* Destroy/free a Bencoding context. */
extern void					Bencoding_destroyContext(Bencoding_Context *c);

/* Decode a buffer with Bencoded data, returns -1 on error. */
extern int32_t				Bencoding_decodeBuffer(Bencoding_Context *c, const void *ptr, size_t len);

/* Return the next item from a (decoded) context, returns NULL if there are no more items. */
extern Bencoding_Item *		Bencoding_getNext(Bencoding_Context *c);

/* Get the last error message on a context. */
extern const char *			Bencoding_getErrorMessage(Bencoding_Context *c);


#endif
