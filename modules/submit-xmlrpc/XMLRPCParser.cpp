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

 
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "XMLRPCParser.hpp"

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef HAVE_STRNDUP
 // from http://www.unixpapa.com/incnote/string.html
 char *strndup(const char *str, size_t len)
 {
     char *dup= (char *)malloc( len+1 );
     if (dup) {
         strncpy(dup,str,len);
         dup[len]= '\0';
     }
     return dup;
  }
#endif /* HAVE_STRNDUP */

namespace nepenthes
{


// the wrapper class


XMLRPCParser::XMLRPCParser(char *s)
{
        m_tree = parseXMLString(s);
}

XMLRPCParser::~XMLRPCParser()
{
        freeXMLTree(m_tree);
}

const char  *XMLRPCParser::getValue(char *key)
{
	return getXMLValue(key, m_tree);
}











typedef enum TokenType
{
	t_tag,
	t_value,
} TokenType;

typedef struct Token
{
	struct Token	*m_next;
	const char		*m_token;
	TokenType		m_type;
} Token;


static void printEscaped(const char *s, uint32_t n);
static Token *allocToken(Token *next, const char *token, TokenType type);
static void freeTokenList(Token *list);
static int32_t matchTag(const char *start, const char *end);

#ifdef _NDEBUG
static void debugTokenList(Token *list);
#endif

static Token *readTag(const char **s);
static Token *readValue(const char *start, const char **s);
static Token *tokenizeXMLString(const char *s);
static Node *buildXMLTree(Token **list);
static void dumpNodeTree(Node *tree, uint32_t depth);
static Node *findKey(const char *key, Node *tree);

/*
 * debug helper
 */
void printEscaped(const char *s, uint32_t n)
{
	while( *s && n-- > 0 )
	{
		if( *s == '\t' )
			printf("\\t");
		else if( *s == '\n' )
			printf("\\n");
		else
			printf("%c", *s);

		s++;
	}
}

/*
 * allocate a new token.
 */
Token *allocToken(Token *next, const char *token, TokenType type)
{
	Token *t = (Token *)malloc(sizeof(Token));
	assert(t);

	t->m_next = next;
	t->m_token = token;
	t->m_type = type;

	return t;
}

/*
 * free a list of tokens and their contents.
 *
 */
void freeTokenList(Token *list)
{
	Token *t, *old;

	for( t = list; t; )
	{
		old = t;
		t = t->m_next;

		free((void *)old->m_token);
		free(old);
	}	
}

/*
 * check if two tags correlate correctly with each other (i.e. start and end tag).
 */
int32_t matchTag(const char *start, const char *end)
{
	if( *end != '/' )
		return 0;

	if( strcmp(start, end + 1) )
		return 0;

	return 1;
}

/*
 * debug helper
 */
#ifdef NDEBUG
void debugTokenList(Token *list)
{
	Token *t;

	for( t = list; t; t = t->m_next )
	{
		printf("token (%08x) ", (uint32_t)t);
		printEscaped(t->m_token, strlen(t->m_token));
		printf("\n");
	}
}
#endif

/*
 * read a tag-token from s and make s point beyond the tag.
 */
Token *readTag(const char **s)
{
	assert(**s == '<');

	(*s)++;

	char *end = strchr(*s, '>');
	assert(end);

	const char *tag = strndup(*s, end - *s);
	assert(tag);
	
	*s = ++end;

	return allocToken(0, tag, t_tag);
}

/*
 * read an xml value starting at start and make s point beyond the value.
 */
Token *readValue(const char *start, const char **s)
{
	char *end = strchr(start, '<');
	assert(end);

	const char *value = strndup(start, end - start);
	assert(value);
	
	*s = end;

	return allocToken(0, value, t_value);
}

/*
 * read a string of xml data and return a tokenized list.
 */
Token *tokenizeXMLString(const char *s)
{
	Token		*tokenList = 0, *lastToken = 0, *t;

	const char	*last = s;
	
	while( *s )
	{
		if( !isspace(*s) )
		{
			if( *s == '<' )
				t = readTag(&s);
			else
				t = readValue(last, &s);

			if( !tokenList )
				tokenList = t;
			else
				lastToken->m_next = t;	
			lastToken = t;

			last = s;
		}
		else
			s++;
	}

	return tokenList;
}

/*
 * build a xml parse tree from a token-list.
 */
Node *buildXMLTree(Token **list)
{
	Node *nodeList = 0, *lastNode = 0, *n;

	while( *list && *((*list)->m_token) != '/' )
	{
		//printf("got %s: ", (*list)->m_token);
		n = (Node *)malloc(sizeof(Node));

		n->m_next = 0;
		n->m_key = strdup((*list)->m_token);

		assert((*list)->m_next);

		// empty tag?
		if( matchTag((*list)->m_token, (*list)->m_next->m_token) )
		{
			//printf("empty\n");
			n->m_type = n_subnode;
			n->m_subNode = 0;
			*list = (*list)->m_next->m_next;
		}
		else
		{
			if( (*list)->m_next->m_type == t_value ) // value
			{
				//printf("value\n");
				assert((*list)->m_next->m_next);
				assert(matchTag((*list)->m_token, (*list)->m_next->m_next->m_token));
				
				n->m_type = n_value;
				n->m_value = strdup((*list)->m_next->m_token);
			
				*list = (*list)->m_next->m_next->m_next;
			}
			else // recurse
			{
				*list = (*list)->m_next;

				//printf("subnode-recurse\n");
				n->m_type = n_subnode;
				n->m_subNode = buildXMLTree(list);

				assert(*list); // premature end of data?
				assert(matchTag(n->m_key, (*list)->m_token));

				*list = (*list)->m_next;
			}
		}

		if( !nodeList )
			nodeList = n;
		else
			lastNode->m_next = n;

		lastNode = n;
	}
	
	//printf("-\n");
	return nodeList;
}

/*
 * debug helper
 */
void dumpNodeTree(Node *tree, uint32_t depth)
{
	Node *n;
	uint32_t i;

	for( n = tree; n; n = n->m_next )
	{
		for( i = 0; i < depth; i++ )
			printf("  ");
		
		printf("%s  %08x:", n->m_key, (unsigned int)((intptr_t)n));
		
		if( n->m_type == n_value )
		{
			printf(" ");
			printEscaped(n->m_value, strlen(n->m_value));
			printf("\n");
		}
		else
		{
			printf("\n");
			dumpNodeTree(n->m_subNode, depth + 1);
		}
	}
}

/*
 * read a string of xml data and return a parse tree.
 */
Node *parseXMLString(const char *s)
{
	Token *start= tokenizeXMLString(s);
	Token *t = start->m_next;
//	assert(!strncmp(start->m_token, "?xml version='1.0'?"));
	Node *n = buildXMLTree(&t);
	freeTokenList(start);

	return n;
}

/*
 * free a parse tree of xml data.
 */
void freeXMLTree(Node *tree)
{
	Node *n, *old;

	for( n = tree; n; )
	{
		old = n;
		n = n->m_next;

		if( old->m_type == n_subnode )
			freeXMLTree(old->m_subNode);
		else
			free((void *)old->m_value);

		free((void *)old->m_key);
		free(old);
	}
}

/*
 * find a key on the top-level of a (sub)tree.
 */
Node *findKey(const char *key, Node *tree)
{
	while( tree )
	{
		if( !strcmp(tree->m_key, key) )
			return tree;

		tree = tree->m_next;
	}

	return 0;
}

/*
 * obtain a value from a xml tree.
 */
const char *getXMLValue(const char *key, Node *tree)
{
	static const char	*notfound = "(not found)";
	static const char	*noval = "(not a value)";
	char				*path = strdup(key);
	const char			*k;
	char				*oldPath = path;

	while( path )
	{
		k = strsep(&path, ".");

		if( path )
		{
			if( !findKey(k, tree) )
			{
				free(oldPath);
				return notfound;
			}


			tree = findKey(k, tree)->m_subNode;
		}
		else if( tree->m_type != n_value )
		{
			free(oldPath);
			return noval;
		}
		else
		{
			free(oldPath);
			return tree->m_value;
		}
	}

	free(oldPath);
	return notfound;
}

/*int32_t main(int32_t argc, char **argv)
{
	const char foo[] = "<?xml version='1.0'?>\n\t\t\t\n<methodResponse>\n<params>\n<param>\n<value><boolean>\t \n     \n 1 \n \n \t</boolean></value>\n</param><param><value><string>noch nen param</string></value></param>\n</params>\n</methodResponse><footag></footag>\n";

	Node *n = parseXMLString(foo);
	dumpNodeTree(n, 0);
	freeXMLTree(n);
	return 0;
}*/

}
