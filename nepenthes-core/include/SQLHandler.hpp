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

#ifndef HAVE_SQLHANDLER_HPP
#define HAVE_SQLHANDLER_HPP

#include <string>

#include "SQLQuery.hpp"

using namespace std;

namespace nepenthes
{
	class SQLQuery;


	class SQLHandler
	{
	public:
		virtual ~SQLHandler(){};

		virtual bool Init()=0;
		virtual bool Exit()=0;

		virtual bool runQuery(SQLQuery *query)=0;
		virtual string escapeString(string *str)=0;
		virtual string escapeBinary(string *str)=0;
		virtual string unescapeBinary(string *str)=0;

		virtual SQLQuery *addQuery(string *query, SQLCallback *callback, void *obj)
		{
//			logPF();
//			logSpam("Query %s\nCallback %x\n",query->c_str(),callback);
			SQLQuery *sqlquery = new SQLQuery(query,callback, obj);
			runQuery(sqlquery);
			return sqlquery;
		}

		virtual string getSQLHandlerName()
		{
			return m_SQLHandlerName;
		}


	protected:
		string m_SQLHandlerName;
	};
}

#endif
