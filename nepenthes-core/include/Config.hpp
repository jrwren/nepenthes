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

#ifndef CONFIG_H
#define CONIFG_H

#include <vector>
#include <map>

// mmap()
#ifdef WIN32

#include <windows.h>
#else
#include <sys/mman.h>
// stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// errno()
#include <errno.h>
// open()
#include <fcntl.h>

#include <stdint.h>

#endif

using namespace std;



namespace nepenthes
{


	struct confltstr
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};


//struct Value;

	enum ValueType
	{
		TYPE_STRING,
		TYPE_STRING_LIST,
		TYPE_ITEM_LIST,
	};

	struct ConfigItem;

	typedef vector< const char * > StringList;
	typedef vector< const char * >::iterator StringListIt;

	typedef map< const char *, ConfigItem *, confltstr > ItemList;
	typedef map< const char *, ConfigItem *, confltstr >::iterator ItemListIt;

	struct ConfigItem
	{
		const char      *m_key;
		ConfigItem      *m_parent;
		ValueType       m_type;

		union
		{
			const char  *m_valString;
			StringList  *m_valStringList;
			ItemList    *m_valItemList;
		};

		bool            isRoot()
		{
			return !m_parent;
		};
		ConfigItem(const char *key, ConfigItem *parent, ValueType type);
	};

#define UNEXP_EOF "Unexpected end of file"

	class ParseError
	{
		int32_t         m_line;
		const char  *m_msg;

	public:
		ParseError(const char *msg, int32_t line)
		{
			m_line = line; m_msg = msg;
		};
		int32_t         getLine()
		{
			return m_line;
		};
		const char  *getMessage()
		{
			return m_msg;
		};
	};

	class LoadError
	{
		const char  *m_msg;

	public:
		LoadError(const char *msg)
		{
			m_msg = msg;
		};
		const char  *getMessage()
		{
			return m_msg;
		};
	};

	class AccessError
	{
	};

	class InvalidKey : public AccessError
	{
	public:
		InvalidKey()
		{
		};
	};

	class InvalidType : public AccessError
	{
	public:
		InvalidType()
		{
		};
	};


	enum TokenType
	{
		T_PAREN_OPEN = 0,
		T_PAREN_CLOSE,
		T_BRACES_OPEN,
		T_BRACES_CLOSE,
		T_SEMICOLON,
		T_COMMA,
		T_KEY,
		T_STRING,

		_T_COUNT,
	};

	struct Token
	{
		TokenType       t;
		int32_t             lineNum;

		union
		{
			char        c;
			char        *str;
		};
	};

	class CharField
	{
		const unsigned char     *m_ptr;
		uint32_t    m_len;
		uint32_t    m_pos;

	public:
		CharField(const unsigned char *ptr, uint32_t len);
		inline unsigned char    getChar();
		inline void         ungetChar(uint32_t amt = 1);
		inline bool     isEOF();
	};

	class OutOfBounds
	{
	};

	class Config
	{
		ConfigItem      *m_root;

		virtual void            skipWS(CharField *data, int32_t *lineNum);
		virtual char            *parseString(CharField *data, int32_t lineNum);
		virtual char            *parseKey(CharField *data);

		virtual void            tokenize(CharField *data);
		virtual void            readObject(vector< Token > *tokenList, vector< Token >::iterator *pos, ConfigItem *item);
		virtual void            dump(map< const char *, ConfigItem *, confltstr > *m, int32_t level = 0);

		virtual ConfigItem      *findKey(const char *);
	public:
		Config();
		virtual ~Config();
		virtual void            load(const char *filename);
		virtual ConfigItem      *getRoot()
		{
			return m_root;
		};


		char            *m_terminatedstring;
		virtual char            *terminateString(char *str);

		// return the int32_t value of key
		virtual int32_t             getValInt(const char *key)
		{
			return getValInt(findKey(key));
		};
		virtual int32_t             getValInt(ConfigItem *key);

		// return a char * of the key
		virtual const char      *getValString(const char *key)
		{
			return getValString(findKey(key));
		};
		virtual const char      *getValString(ConfigItem *key);

		// string list
		virtual StringList      *getValStringList(const char *key)
		{
			return getValStringList(findKey(key));
		};
		virtual StringList      *getValStringList(ConfigItem *key);

		// item list
		virtual ItemList        *getValItemList(const char *key)
		{
			return getValItemList(findKey(key));
		};
		virtual ItemList        *getValItemList(ConfigItem *key);

		// return type
		virtual ValueType       getValType(const char *key)
		{
			return getValType(findKey(key));
		};
		virtual ValueType       getValType(ConfigItem *key);

		virtual bool            valExists(const char *key)
		{
			return(bool)findKey(key);
		};
	};


}

#endif
