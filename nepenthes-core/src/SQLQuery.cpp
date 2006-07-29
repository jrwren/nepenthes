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

#include <errno.h>
#include "SQLManager.hpp"
#include "SQLCallback.hpp"
#include "SQLResult.hpp"
#include "SQLQuery.hpp"
#include "SQLHandler.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;


SQLQuery::SQLQuery(string *query,SQLCallback *callback, void *obj)
{
	logPF();
	m_Callback = callback;
	m_Query = *query;
	m_Object = obj;
}

SQLQuery::~SQLQuery()
{
	
}

SQLCallback *SQLQuery::getCallback()
{
	return m_Callback;
}

bool SQLQuery::cancelCallback()
{
	if (m_Callback != NULL)
	{
		m_Callback = NULL;
		return true;
	}else
	{
		return false;
	}

}

string SQLQuery::getQuery()
{
	return m_Query;
}

void *SQLQuery::getObject()
{
	return m_Object;
}
