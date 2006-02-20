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

#ifndef HAVE_DIALOGUEFACTORYMANAGER_HPP
#define HAVE_DIALOGUEFACTORYMANAGER_HPP

#include <list>

#include "Manager.hpp"

#define REG_DIALOGUEFACTORY(diaf) g_Nepenthes->getFactoryMgr()->registerFactory(diaf)

using namespace std;

namespace nepenthes
{

	class DialogueFactory;
	class Nepenthes;

	/**
	 * Some DialogueFactories are independent, for example 
	 * the "WintNTShell"
	 * if you want to register a independent DialogueFactory so you can use it in a 
	 * different module, register it here.
	 * then ask the DialogueFactoryManager for the registerd DialogueFactory 
	 * in the other module, and your done.
	 */
	class DialogueFactoryManager : public Manager
	{
	public:
		DialogueFactoryManager(Nepenthes *nepenthes);
		virtual ~DialogueFactoryManager();
		bool Init();
		bool Exit();
		void doList();

		virtual bool registerFactory(DialogueFactory *diaf);
		virtual bool unregisterFactory(DialogueFactory *diaf);
		virtual DialogueFactory *getFactory(char *factoryname);
	protected:
		list <DialogueFactory *> m_Factories;
	};
}

#endif
