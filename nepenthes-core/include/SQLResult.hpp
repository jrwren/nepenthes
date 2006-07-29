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

#ifndef HAVE_SQLRESULT_HPP
#define HAVE_SQLRESULT_HPP

#include <list>
#include <string>
#include <map>
#include <vector>

using namespace std;

namespace nepenthes
{

	class SQLResult
	{
	public:
		SQLResult(string query, void *obj);
		virtual ~SQLResult();
		virtual string  getQuery();
		virtual void    *getObject();

		virtual vector< map<string,string> > *getResult();
	protected:
		string m_Query;
		void    *m_Object;
		vector< map<string,string> >        m_Result;

	};
}

#endif
