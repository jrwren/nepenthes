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
#include "SQLHandlerFactory.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;



SQLManager::SQLManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}

SQLManager::~SQLManager()
{

}

bool SQLManager::Init()
{

	return true;
}

bool SQLManager::Exit()
{
	return true;
}

void SQLManager::doList()
{

	list <SQLHandlerFactory *>::iterator it;
	logSpam("=--- %-69s ---=\n","SQLManager");
	int32_t i=0;
	for(it = m_SQLHandlerFactories.begin();it != m_SQLHandlerFactories.end();it++,i++)
	{
		logSpam("  %i) %-8s \n",i,(*it)->getDBType().c_str());
	}
	logSpam("=--- %2i %-66s ---=\n\n",i, "SQLHandlerFactories registerd");
}


bool SQLManager::registerSQLHandlerFactory(SQLHandlerFactory * handlerfactory)
{
	m_SQLHandlerFactories.push_back(handlerfactory);
	return true;
}

void SQLManager::unregisterSQLHandlerFactory(const char *dbtype)
{ // FIXME
	return;
}

SQLHandler *SQLManager::createSQLHandler(const char *dbtype, string server, string user, string passwd, string table, string options)
{
	list <SQLHandlerFactory *>::iterator it;
	int i=0;
	for (it = m_SQLHandlerFactories.begin(); it != m_SQLHandlerFactories.end(); i++)
	{
		if (dbtype == (*it)->getDBType())
		{
			SQLHandler *sqlh =  (*it)->createSQLHandler(server, user,passwd,table,options);
			if (sqlh->Init() == true)
			{
				return sqlh;
			}else
			{
				return NULL;
			}
		}
	}
	return NULL;
}
