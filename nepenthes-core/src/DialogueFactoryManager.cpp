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

#include "DialogueFactoryManager.hpp"
#include "DialogueFactory.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dia | l_mgr


using namespace nepenthes;



DialogueFactoryManager::DialogueFactoryManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}

DialogueFactoryManager::~DialogueFactoryManager()
{
	while (m_Factories.size() > 0)
	{
        m_Factories.pop_front();
	}
}

bool DialogueFactoryManager::Init()
{
	return true;
}
bool DialogueFactoryManager::Exit()
{
	return true;
}

void DialogueFactoryManager::doList()
{
	list <DialogueFactory *>::iterator diaf;
	int32_t i =0;
	logSpam("=--- DialogueFactoryManager --=\n");
	for (diaf = m_Factories.begin();diaf != m_Factories.end();diaf++, i++)
	{
		logSpam("%i %20s %s\n",i,(*diaf)->getFactoryName().c_str(),(*diaf)->getFactoryDescription().c_str());
	}
	logSpam("=--- %i Factories --=\n", m_Factories.size());

}

bool DialogueFactoryManager::registerFactory(DialogueFactory *diaf)
{
	m_Factories.push_back(diaf);
	return true;
}

bool DialogueFactoryManager::unregisterFactory(DialogueFactory *diaf)
{
	return true;
}

DialogueFactory *DialogueFactoryManager::getFactory(const char *factoryname)
{
	list <DialogueFactory *>::iterator diaf;
	for (diaf = m_Factories.begin();diaf != m_Factories.end();diaf++)
	{
		if ((*diaf)->getFactoryName() == factoryname)
		{
			return (*diaf);
		}
	}
	return NULL;
}
