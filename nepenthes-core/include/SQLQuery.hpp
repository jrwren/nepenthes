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

#ifndef HAVE_SQLQUERY_HPP
#define HAVE_SQLQUERY_HPP


#include <stdint.h>
#include <string>


namespace nepenthes
{
	class SQLCallback;

	class SQLQuery
	{
	public:
		SQLQuery(string *query, SQLCallback *callback, void *obj);
		virtual ~SQLQuery();

		virtual SQLCallback *getCallback();
		virtual bool 		cancelCallback();
		virtual string      getQuery();
		virtual void        *getObject();

	protected:
		SQLCallback  		*m_Callback;
		void        		*m_Object;

		string      		m_Query;
	};

}

#endif
