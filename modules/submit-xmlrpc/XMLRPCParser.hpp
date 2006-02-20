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

 
#ifndef XMLRPCPARSER_HPP
#define XMLRPCPARSER_HPP

#include <string>

namespace nepenthes
{


// the wrapper class
	struct Node;


	class XMLRPCParser
	{
	public:
		XMLRPCParser(char *s);
		~XMLRPCParser();
		const char *getValue(char *key);
	private:
		Node                    *m_tree;
	};



// the parser
	typedef enum NodeType
	{
		n_subnode,
		n_value,
	} NodeType;

	typedef struct Node
	{
		struct Node     *m_next;
		const char      *m_key;

		NodeType        m_type;

		union
		{
			struct Node     *m_subNode;
			const char      *m_value;
		};
	} Node;

	extern Node *parseXMLString(const char *s);
	extern void freeXMLTree(Node *tree);
	extern const char *getXMLValue(const char *key, Node *tree);


}
#endif
