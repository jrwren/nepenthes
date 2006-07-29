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

#ifndef HAVE_SQLCALLBACK_HPP
#define HAVE_SQLCALLBACK_HPP

#include <string>

using namespace std;

namespace nepenthes
{
	class SQLResult;

	class SQLCallback
	{
		public:
		virtual ~SQLCallback(){};
        virtual bool sqlSuccess(SQLResult *result)=0;
		virtual bool sqlFailure(SQLResult *result)=0;

		virtual string getSQLCallbackName()
		{
			return m_SQLCallbackName;
		}

	protected:
		string m_SQLCallbackName;
	};
}

#endif
