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

#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "bencoding.h"

#define BENCODING_OOM "Out of memory!"

static int32_t parseBuffer(Bencoding_Context *c, Bencoding_Item *item);
void debugItem(Bencoding_Item *item, uint32_t indent);
void freeItem(Bencoding_Item *item);
static inline uint32_t isEof(Bencoding_Context *c);

Bencoding_Context *Bencoding_createContext()
{
	Bencoding_Context *c;

	c = (Bencoding_Context *)malloc(sizeof(Bencoding_Context));

	if ( c != NULL )
		memset(c, 0, sizeof(Bencoding_Context));

	return c;
};

void Bencoding_destroyContext(Bencoding_Context *c)
{
	uint32_t i;

	if ( c->m_items.m_vector )
	{
		for ( i = 0; i < c->m_items.m_size; i++ )
			freeItem(&(c->m_items.m_vector[i]));

		free(c->m_items.m_vector);
	}

	free(c->m_buffer);
	free(c);
};

const char *Bencoding_getErrorMessage(Bencoding_Context *c)
{
	return c->m_errorMessage;
}

int32_t Bencoding_decodeBuffer(Bencoding_Context *c, const void *ptr, size_t len)
{
	c->m_items.m_vector = 0;
	c->m_len = len;
	c->m_buffer = malloc(len);

	if ( c->m_buffer == NULL )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "%s", BENCODING_OOM);
		return -1;
	}

	memcpy(c->m_buffer, ptr, len);

	c->m_ptr = (uint8_t *)c->m_buffer;
	c->m_offset = 0;

	c->m_items.m_size = 0;
	c->m_items.m_iterator = 0;
	c->m_items.m_capacity = 4;
	c->m_items.m_vector = (Bencoding_Item *)malloc(sizeof(Bencoding_Item) * c->m_items.m_capacity);
	if ( c->m_items.m_vector == NULL )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "%s", BENCODING_OOM);
		return -1;
	}

	while ( !isEof(c) )
	{
		if ( c->m_items.m_size == c->m_items.m_capacity )
		{
			c->m_items.m_capacity *= 2;
			Bencoding_Item *newvector = (Bencoding_Item *)realloc((void *)c->m_items.m_vector,
																  sizeof(Bencoding_Item) * c->m_items.m_capacity);

			if ( newvector == NULL )
			{
				snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "%s", BENCODING_OOM);
				return -1;
			}

			c->m_items.m_vector = newvector;
		}

		if ( parseBuffer(c, &(c->m_items.m_vector[c->m_items.m_size])) == -1 )
			return -1;

		c->m_items.m_size++;
	}

	return 0;
}

Bencoding_Item *Bencoding_getNext(Bencoding_Context *c)
{
	if ( !(c->m_items.m_iterator < c->m_items.m_size) )
		return NULL;

	return &(c->m_items.m_vector[c->m_items.m_iterator++]);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static inline void advance(Bencoding_Context *c)
{
	c->m_ptr++;
	c->m_offset++;
}

static inline uint32_t isEof(Bencoding_Context *c)
{
	return !(c->m_offset < c->m_len);
}

/* look at the next char, don't advance, -1 on error. */
static inline int32_t peekChar(Bencoding_Context *c, uint8_t *ch)
{
	if ( isEof(c) )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage),
				 "Got premature end of data at position %d",
				 c->m_offset);
		return -1;
	}

	*ch = *(c->m_ptr);
	return 0;
}

/* read one char from the stream, -1 on error.*/
static inline int32_t readChar(Bencoding_Context *c, uint8_t *ch)
{
	if ( isEof(c) )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage),
				 "Got premature end of data at position %d",
				 c->m_offset);
		return -1;
	}

	*ch = *(c->m_ptr);
	advance(c);
	return 0;
}

/* require the presence of a specific char on the stream, -1 on error. */
inline int32_t consumeChar(Bencoding_Context *c, uint8_t ch)
{
	uint8_t chr;

	if ( readChar(c, &chr) == -1 )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage),
				 "Expected 0x%02x (`%c'), but got premature end of data at position %d",
				 ch, isprint(ch) ? ch : '.', c->m_offset);
		return -1;
	}

	if ( chr != ch )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage),
				 "Expected 0x%02x (`%c'), but got 0x%02x (`%c') at position %d",
				 ch, isprint(ch) ? ch : '.', chr, isprint(chr) ? chr : '.',
				 c->m_offset);
		return -1;
	}

	return 0;
}

int32_t readInt(Bencoding_Context *c, int32_t *value)
{
	uint8_t premature = 1, neg = 0;
	*value = 0;

	if ( !isEof(c) && *(c->m_ptr) == '-' )
	{
		neg = 1;
		advance(c);
	}
	if ( !isEof(c) && *(c->m_ptr) == '0' )
	{
		advance(c);
		return 0;
	}

	if ( !isdigit(*(c->m_ptr)) )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage),
				 "Got non digit character 0x%02x (`%c') for integer value at position %d",
				 *(c->m_ptr), isprint(*(c->m_ptr)) ? *(c->m_ptr) : '.', c->m_offset);
		return -1;
	}

	while ( !isEof(c) && isdigit(*(c->m_ptr)) )
	{
		premature = 0;
		*value = *value * 10 + (*(c->m_ptr) - '0');
		advance(c);
	}

	if ( neg )
		*value *= -1;

	if ( premature )
	{
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage),
				 "Expected digit, but got premature end of data at position %d",
				 c->m_offset);
		return -1;
	}

	return 0;
}

int32_t readString(Bencoding_Context *c, Bencoding_String *str)
{
	uint32_t len, i;

	if ( readInt(c, (int32_t *)&len) == -1 )
		return -1;

	if ( consumeChar(c, ':') == -1 )
		return -1;

	str->m_data = (void *)c->m_ptr;
	str->m_len = len;

	for ( i = 0; i < len; i++ )
	{
		if ( isEof(c) )
		{
			snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "Premature end of encoded string at position %d",
					 c->m_offset);
			return -1;
		}
		advance(c);
	}

	return 0;
}

int32_t parseBuffer(Bencoding_Context *c, Bencoding_Item *item)
{
	assert(!isEof(c));

	uint32_t i;
	int32_t error;
	uint8_t ch;

	switch ( *(c->m_ptr) )
	{
	case 'i': /* integer */
		advance(c);	/* l */
//			if( consumeChar(c, ':') == -1 )
//				return -1;

		if ( readInt(c, (int32_t *)&i) == -1 )
			return -1;

		if ( consumeChar(c, 'e') == -1 )
			return -1;

		item->m_type = Bencoding_TypeInt;
		item->m_int = i;
		printf("read int\n");
		break;

	case 'l': /* list */
		advance(c);	/* l */
		printf("found list\n");
		item->m_type = Bencoding_TypeList;

		item->m_list.m_size = 0;
		item->m_list.m_capacity = 4;
		item->m_list.m_vector = (Bencoding_Item *)malloc(sizeof(Bencoding_Item) * item->m_list.m_capacity);
		if ( item->m_list.m_vector == NULL )
		{
			snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "%s", BENCODING_OOM);
			return -1;
		}

		while ( (error = peekChar(c, &ch)) != -1 && ch != 'e' )
		{
			if ( item->m_list.m_size == item->m_list.m_capacity )
			{
				item->m_list.m_capacity *= 2;
				Bencoding_Item *newvec = (Bencoding_Item *)realloc((void *)item->m_list.m_vector,
																   sizeof(Bencoding_Item) * item->m_list.m_capacity);
				if ( newvec == NULL )
				{
					snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "%s", BENCODING_OOM);
					return -1;
				}

				item->m_list.m_vector = newvec;
			}

			if ( parseBuffer(c, &(item->m_list.m_vector[item->m_list.m_size])) == -1 )
			{
				freeItem(item);
				return -1;
			}

			item->m_list.m_size++;
		}

		if ( error == -1 )
		{
			freeItem(item);
			return -1;
		}
		else
			advance(c);	/* e */

		printf("read list\n");
		break;

	case 'd': /* dictionary */
		advance(c);	/* d */
		printf("found dict\n");
		item->m_type = Bencoding_TypeDict;

		item->m_dict.m_size = 0;
		item->m_dict.m_capacity = 2;
		item->m_dict.m_keys = (Bencoding_String *)malloc(sizeof(Bencoding_String) * item->m_dict.m_capacity);
		item->m_dict.m_values = (Bencoding_Item *)malloc(sizeof(Bencoding_Item) * item->m_dict.m_capacity);

		if ( item->m_dict.m_keys == NULL || item->m_dict.m_values == NULL )
		{
			snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "%s", BENCODING_OOM);
			return -1;
		}

		while ( (error = peekChar(c, &ch)) != -1 && ch != 'e' )
		{
			if ( item->m_dict.m_size == item->m_dict.m_capacity )
			{
				item->m_dict.m_capacity *= 2;
				Bencoding_String *newkeys = (Bencoding_String *)realloc((void *)item->m_dict.m_keys,
																		sizeof(Bencoding_String) * item->m_dict.m_capacity);
				Bencoding_Item *newvalues = (Bencoding_Item *)realloc((void *)item->m_dict.m_values,
																	  sizeof(Bencoding_Item) * item->m_dict.m_capacity);

				if ( newkeys == NULL || newvalues == NULL )
				{
					snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "%s", BENCODING_OOM);
					return -1;
				}

				item->m_dict.m_keys = newkeys;
				item->m_dict.m_values = newvalues;
			}

			/* read key */
			if ( readString(c, &(item->m_dict.m_keys[item->m_dict.m_size])) == -1 )
			{
				freeItem(item);
				return -1;
			}

			/* val */
			if ( parseBuffer(c, &(item->m_dict.m_values[item->m_dict.m_size])) == -1 )
			{
				freeItem(item);
				return -1;
			}

			item->m_dict.m_size++;
		}

		if ( error == -1 )
		{
			freeItem(item);
			return -1;
		}
		else
			advance(c);	/* e */
		printf("read dict\n");
		break;

	case '0': /* string (length-prefixed) */
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		item->m_type = Bencoding_TypeString;

		if ( readString(c, &(item->m_string)) == -1 )
			return -1;

		printf("read string\n");
		break;

	default:
		snprintf(c->m_errorMessage, sizeof(c->m_errorMessage), "Invalid character 0x%02x (`%c') at position %d",
				 *(c->m_ptr), isprint(*(c->m_ptr)) ? *(c->m_ptr) : '.', c->m_offset);
		return -1;
		break;
	}

	return 0;
}


inline void printIndent(uint32_t indent)
{
	while ( indent-- )
		printf("  ");
}

void debugItem(Bencoding_Item *item, uint32_t indent)
{
	uint32_t i, j;
	switch ( item->m_type )
	{
	case Bencoding_TypeInt:
		printIndent(indent); printf("(int) %d\n", item->m_int);
		break;

	case Bencoding_TypeList:
		printIndent(indent); printf("(list)\n");

		for ( i = 0; i < item->m_list.m_size; i++ )
			debugItem(&(item->m_list.m_vector[i]), indent + 1); 
		break;

	case Bencoding_TypeString:
		printIndent(indent); printf("(string) (%i bytes)\n", item->m_string.m_len);
		printIndent(indent); 
		for ( i = 1; i <= item->m_string.m_len; i++ )
		{
			if ( isprint( ( (uint8_t *)item->m_string.m_data )[i-1] ) )
				printf("%c", ((uint8_t *)item->m_string.m_data)[i-1]);
			else

				printf("%02x", ((uint8_t *)item->m_string.m_data)[i-1]);
			if ( i % 20 == 0 && 0 )
			{
				printf("\n");
				printIndent(indent);
			}
		}
		printf("\n");
		break;

	case Bencoding_TypeDict:
		printIndent(indent); printf("(dict)\n");
		for ( i = 0; i < item->m_dict.m_size; i++ )
		{
			printIndent(indent + 1);
			for ( j = 0; j < item->m_dict.m_keys[i].m_len; j++ )
				printf("%c", ((uint8_t *)item->m_dict.m_keys[i].m_data)[j]);
			printf(" -->\n");
			debugItem(&(item->m_dict.m_values[i]), indent + 1);
		}
		break;

	default:
		break;
	}   
}

void freeItem(Bencoding_Item *item)
{
	uint32_t i;

	switch ( item->m_type )
	{
	case Bencoding_TypeList:
		for ( i = 0; i < item->m_list.m_size; i++ )
			freeItem(&(item->m_list.m_vector[i]));
		free(item->m_list.m_vector);
		break;
	case Bencoding_TypeDict:
		for ( i = 0; i < item->m_dict.m_size; i++ )
			freeItem(&(item->m_dict.m_values[i]));
		free(item->m_dict.m_keys);
		free(item->m_dict.m_values);
		break;
	default:
		return;
	}

}


