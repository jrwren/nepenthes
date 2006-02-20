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
#include <ctype.h>

#include "Config.hpp"

#include "Compatibility.hpp"

using namespace nepenthes;


const char *g_tokenMap[] = {
	/* T_PAREN_OPEN */		"(",
	/* T_PAREN_CLOSE */		")",
	/* T_BRACES_OPEN */		"{",
	/* T_BRACES_CLOSE */	"}",
	/* T_SEMICOLON */		";",
	/* T_COMMA */			",",
	/* T_KEY */				"T_KEY",
	/* T_STRING */			"T_STRING",
};


ConfigItem::ConfigItem(const char *key, ConfigItem *parent, ValueType type)
{
	m_key = key;
	m_parent = parent;
	m_type = type;
}

Config::Config()
{
	m_root = 0;
    m_terminatedstring = NULL;
}

Config::~Config()
{   // ich brauch den destruktor um das in nem plugin nutzen zu koennen
	m_root = 0;
}



// char *strsep(char **stringp, const char *delim);
ConfigItem *Config::findKey(const char *key)
{
	ConfigItem 	*item = m_root;
	char		*path, *orig;

	orig = path = strdup(key);

	do
	{
		if( !item || item->m_type != TYPE_ITEM_LIST)
			return 0;

		item = (*item->m_valItemList)[strsep(&path, ".")];
	}
	while( path );

	free(orig);

	return item;
}

int32_t Config::getValInt(ConfigItem *key)
{
	if( !key )
		throw InvalidKey();
	else if( key->m_type != TYPE_STRING )
		throw InvalidType();
	else
		return atoi(key->m_valString);
}

const char *Config::getValString(ConfigItem *key)
{
	if( !key )
		throw InvalidKey();
	else if( key->m_type != TYPE_STRING )
		throw InvalidType();
	else
		return key->m_valString;
}

StringList *Config::getValStringList(ConfigItem *key)
{
	if( !key )
		throw InvalidKey();
	else if( key->m_type != TYPE_STRING_LIST )
		throw InvalidType();
	else
		return key->m_valStringList;
}

ItemList *Config::getValItemList(ConfigItem *key)
{
	if( !key )
		throw InvalidKey();
	else if( key->m_type != TYPE_ITEM_LIST )
		throw InvalidType();
	else
		return key->m_valItemList;
}

ValueType Config::getValType(ConfigItem *key)
{
	if( !key )
		throw InvalidKey();
	else
		return key->m_type;
}

void Config::load(const char *filename)
{
#ifdef WIN32
	struct			_stat file;
#else
	struct			stat file;
#endif

	unsigned char	*data;


	if( stat(filename, &file) )
		throw LoadError(strerror(errno));

#ifdef WIN32
	// memory mapping in windows is really ugly

	data = (unsigned char *)malloc(file.st_size);
	FILE *f = fopen(filename,"rb");
	fread(data,1,file.st_size,f);
	fclose(f);

#else
	int32_t				fd;
	if( (fd = open(filename, O_RDONLY)) == -1 )
		throw LoadError(strerror(errno));

	data = (unsigned char *)mmap(0, file.st_size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);

	if( data == (unsigned char *)-1 )
		throw LoadError(strerror(errno));
#endif
	CharField		field(data, file.st_size);

	tokenize(&field);

#ifdef WIN32
	free(data);
#else
	if( munmap((void *)data, file.st_size) )
		throw LoadError(strerror(errno));
#endif

}

/*
skip whitespaces and comments, return a pointer to the first non-ws getChar()
*/
void Config::skipWS(CharField *data, int32_t *lineNum)
{
	unsigned char c;

	try
	{
		while( !data->isEOF() )
		{
			c = data->getChar();

			if( c == '\n' )
				(*lineNum)++;

			if( isspace(c) )
				continue;

			else if( c == '/' )
			{
				c = data->getChar();
			
				if( c == '/' )
				{
					do
						c = data->getChar();
					while( c != '\n' );
					(*lineNum)++;
				}
				else if( c == '*' )
				{
					for(;;)
					{
						c = data->getChar();
						if( c == '\n' )
							(*lineNum)++;
						else if( c == '*' )
						{
							c = data->getChar();
							if( c == '/' )
								break;
							else
								data->ungetChar();
						}
					}
				}
				else
				{
					data->ungetChar(2);
					return;
				}
			}
			else
			{
				data->ungetChar();
				return;
			}
		}
	}
	catch( OutOfBounds )
	{
		return;
	}

}

/*
parse and malloc "word" containing [a-z][A-Z]-_
*/
char *Config::parseKey(CharField *data)
{
	uint32_t		len = 0;
	unsigned char		c;

	while( !data->isEOF() )
	{
		c = data->getChar();
		len++;

		if( !isalpha(c) && !isdigit(c) && c != '-' && c != '_' )
		{
			len--;
			data->ungetChar();
			break;
		}
	}

	data->ungetChar(len);

	char *key = new char[len + 1];

	for( uint32_t i = 0; i < len; i++ )
		key[i] = data->getChar();

	key[len] = 0;
	return key;
}


/*
parse a string, \" \x4f \0 etc will be added later
*/
char *Config::parseString(CharField *data, int32_t lineNum)
{
	uint32_t len = 0, rawLen = 0;
	unsigned char c;
	char *str;

	try
	{
		for(;;)
		{
			c = data->getChar();
			rawLen++;

			if( c == '\\' )
			{
				c = data->getChar();
				rawLen++;
				
				if( c != '0' && c != '"' && c != '\\' && c != 'x' )//&& c != '{' )
					throw ParseError("Invalid escape sequence in string", lineNum);
				if (c == 'x')
				{
					len++;
                    c = data->getChar();
					c = data->getChar();
					rawLen +=2;
				}
				
				else
					len++;
			}
			else if( c == '"' )
			{
				rawLen--;
				data->ungetChar();
				break;
			}
			else
				len++;
		}

		data->ungetChar(rawLen);

		str = new char[len + 1];

//		printf("strlen is %i\n",len);
		for( uint32_t i = 0; i < len; i++ )
		{
//			printf("uint32_t i is %i\n",i);
			c = data->getChar();
			if( c == '\\' )
			{
				c = data->getChar();

				if( c == '0' )
					str[i] = 0;
				else
				if( c == 'x' )
				{// escaped hex value
					char szHexConv[3];

					szHexConv[0] = data->getChar();
					szHexConv[1] = data->getChar();
					szHexConv[3] = 0;
//					printf("uint32_t i is here %i\n",i);
//					printf("Converting %i %c%c -> %i\n",i, szHexConv[0],szHexConv[1], (unsigned char)strtol(szHexConv,NULL,16) );
//					strtol(szHexConv,NULL,16);
//					str[i] = (char) strtol(szHexConv,(char **)&szHexConv+1,16);
//					i+=2;
					str[i] = (unsigned char)strtol(szHexConv,NULL,16);
//					printf("uint32_t i is later %i\n",i);
					
				}
				
				else
					str[i] = c;
			}
			else
				str[i] = c;
		}
		
		data->getChar(); // ending "
		str[len] = 0;
		return str;

	}
	catch( OutOfBounds )
	{
		throw ParseError("Premature end of file while looking for string end", lineNum);
	}

}

/*
debug helper to visualize a memory config tree (stdout)
*/
void Config::dump(map< const char *, ConfigItem *, confltstr > *m, int32_t level)
{
	map< const char *, ConfigItem *, confltstr >::iterator it;

	for( it = m->begin(); it != m->end(); it++ )
	{
		for( int32_t k = 0; k < level; k++ )
			printf("%s", "  ");

		if( (*it).second->m_type == TYPE_STRING )
			printf("(str) %s -> \"%s\"\n", (*it).first, (*it).second->m_valString);
		else if( (*it).second->m_type == TYPE_ITEM_LIST )
		{
			printf("(obj) %s\n", (*it).first);
			dump((*it).second->m_valItemList, level + 1);
		}
		else if( (*it).second->m_type == TYPE_STRING_LIST )
		{
			printf("(stringlist) %s\n", (*it).first);

			uint32_t i = 0;

			while( i < (*it).second->m_valStringList->size() )
			{
				for( int32_t k = 0; k < (level + 1); k++ )
					printf("%s", "  ");

				printf("(str) \"%s\"\n", (*(*it).second->m_valStringList)[i]);
				i++;
			}
		}
	}
}

/*
parses the tokens into the config tree structure
*/
void Config::readObject(vector< Token > *tokenList, vector< Token >::iterator *pos, ConfigItem *item)
{
	ConfigItem *i;

	while( *pos != tokenList->end() )
	{
		if( (**pos).t != T_KEY )
		{
			if( (**pos).t == T_BRACES_CLOSE )
			{
				if( item->isRoot() )
					throw ParseError("`}' is not valid on the root level", (**pos).lineNum);
				else
				{
					// };
					(*pos)++;
					if( *pos == tokenList->end() )
						throw ParseError(UNEXP_EOF, (**pos).lineNum);
					
					if( (**pos).t != T_SEMICOLON )
						throw ParseError("Missing `;' after `}'", (**pos).lineNum);
					else
					{
						(*pos)++;
						return;
					}
				}
			}
			else
				throw ParseError("`T_KEY' or `}' expected", (**pos).lineNum);
		}

		(*pos)++;

		if( *pos == tokenList->end() )
			throw ParseError(UNEXP_EOF, (**pos).lineNum);

		switch( (**pos).t )
		{
		case T_STRING:
			if( ((*pos) + 1) == tokenList->end() )
				throw ParseError(UNEXP_EOF, (**pos).lineNum);

			if( (*((*pos) + 1)).t != T_SEMICOLON )
				throw ParseError("Missing `;' after `T_STRING'", (**pos).lineNum);

			// we got a valid foo "bar";
			i = new ConfigItem((*((*pos) - 1)).str, item, TYPE_STRING);
			i->m_valString = (**pos).str;

			(*item->m_valItemList)[i->m_key] = i;
			(*pos) += 2; // skip the ;
			break;

		case T_PAREN_OPEN:
			i = new ConfigItem((*((*pos) - 1)).str, item, TYPE_STRING_LIST);
			i->m_valStringList = new vector< const char * >;
			(*item->m_valItemList)[i->m_key] = i;
			(*pos)++;

			while( *pos != tokenList->end() )
			{
				if( (**pos).t == T_STRING )
				{
					i->m_valStringList->push_back((**pos).str);
					(*pos)++;
					if( *pos == tokenList->end() )
						throw ParseError(UNEXP_EOF, (**pos).lineNum);
					if( (**pos).t == T_COMMA )
						(*pos)++;
				}
				else if( (**pos).t == T_PAREN_CLOSE )
				{
					(*pos)++;
					if( *pos == tokenList->end() )
						throw ParseError(UNEXP_EOF, (**pos).lineNum);
					if( (**pos).t != T_SEMICOLON )
						throw ParseError("Missing `;' after `)'", (**pos).lineNum);

					(*pos)++;
					break;
				}
				else
					throw ParseError("`T_STRING' or `)' expected", (**pos).lineNum);
			}

			break;

		case T_BRACES_OPEN:
			i = new ConfigItem((*((*pos) - 1)).str, item, TYPE_ITEM_LIST);
			i->m_valItemList = new map< const char *, ConfigItem *, confltstr >;
			(*item->m_valItemList)[i->m_key] = i;
			(*pos)++;

			readObject(tokenList, pos, i);
			break;

		default:
			throw ParseError("`T_STRING' or `(' or `{' expected", (**pos).lineNum);
		}
	}

	// we have reached the end, check if we are on root, otherwise unexpected
	if( !item->isRoot() )
		throw ParseError(UNEXP_EOF, (**pos).lineNum);

	//dump(m_root->m_valItemList);
//	exit(0);
}

void Config::tokenize(CharField *data)
{
	vector< Token >	tokenList;
	Token tok;
	int32_t lineNum = 1;
	unsigned char c;

	for(;;)
	{
		skipWS(data, &lineNum);
		
		if( data->isEOF() )
			break;
		
		c = data->getChar();

		tok.lineNum = lineNum;
		tok.c = c;

		switch( c )
		{
		case '{':
			tok.t = T_BRACES_OPEN;
			tokenList.push_back(tok);
			break;

		case '}':
			tok.t = T_BRACES_CLOSE;
			tokenList.push_back(tok);
			break;

		case '(':
			tok.t = T_PAREN_OPEN;
			tokenList.push_back(tok);
			break;

		case ')':
			tok.t = T_PAREN_CLOSE;
			tokenList.push_back(tok);
			break;

		case ';':
			tok.t = T_SEMICOLON;
			tokenList.push_back(tok);
			break;

		case ',':
			tok.t = T_COMMA;
			tokenList.push_back(tok);
			break;
		
		case '"':
			tok.t = T_STRING;
			tok.str = parseString(data, lineNum);
			tokenList.push_back(tok);
			break;

		default:
			if( isalpha(c) || isdigit(c) )
			{
				data->ungetChar();

				tok.t = T_KEY;
				tok.str = parseKey(data);
				tokenList.push_back(tok);
			}
			// ingore this char
		}
	}


	m_root = new ConfigItem(0, 0, TYPE_ITEM_LIST);
	m_root->m_valItemList = new map< const char *, ConfigItem *, confltstr >;

	vector< Token >::iterator pos = tokenList.begin();

	readObject(&tokenList, &pos, m_root);
}



CharField::CharField(const unsigned char *ptr, uint32_t len)
{
	m_ptr = ptr;
	m_len = len;
	m_pos = 0;
}

inline bool CharField::isEOF()
{
	return (m_pos >= m_len);
}

inline unsigned char CharField::getChar()
{
	if( !isEOF() )
		return m_ptr[m_pos++];
	else
		throw OutOfBounds();
}

inline void CharField::ungetChar(uint32_t amt)
{
	if( amt > m_pos )
		throw OutOfBounds();
	else
		m_pos -= amt;
}

char *Config::terminateString(char *str)
{
    if (str == NULL)
        return NULL;

    if (m_terminatedstring != NULL )
    {
        free(m_terminatedstring);
        m_terminatedstring = NULL;
    }

    
    int32_t i=0;    // the \ counter
    int32_t j=0;    // the " counter

    while (strstr(str+i,"\\") != NULL )
        i++;
    
    while (strstr(str+i,"\"") != NULL )
        j++;
    
    i = i+j;

    if (i == 0)
        return str;

    m_terminatedstring = (char *)malloc(strlen(str) + i +3);
    memset(m_terminatedstring,0,strlen(str) + i +3);

    i=0;
    while(strlen(str) > 1 )
    {
        if (*str == '\\')
        {
            m_terminatedstring[i] = '\\';
            i++;
            m_terminatedstring[i] = '\\';
        }else
        if (*str == '\"')
        {
            m_terminatedstring[i] = '\\';
            i++;
            m_terminatedstring[i] = '\"';
        }
        else
            m_terminatedstring[i] = *str;


        i++;
        *str++;
    }

//    m_terminatedstring = strdup(str);
//    printf(" TERMINATED %s \n",m_terminatedstring);
    return m_terminatedstring;

}


