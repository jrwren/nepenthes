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

#ifndef HAVE_SHELLCODEMANAGER_HPP
#define HAVE_SHELLCODEMANAGER_HPP

#include <list>

#include "Manager.hpp"

using namespace std;

#define REG_SHELLCODE_HANDLER(handler) g_Nepenthes->getShellcodeMgr()->registerShellcodeHandler(handler)

namespace nepenthes
{
	class ShellcodeHandler;
	class Nepenthes;
	class Message;

	typedef enum
	{
		SCH_NOTHING=0,
		SCH_REPROCESS,	// if something was changes f.e. xor decoder
		SCH_REPROCESS_BUT_NOT_ME,
		SCH_DONE,
	} sch_result;


    class ShellcodeManager : public Manager
    {
    public:
        ShellcodeManager(Nepenthes *nepenthes);
        virtual ~ShellcodeManager();
        virtual bool registerShellcodeHandler(ShellcodeHandler *handler);
		virtual bool unregisterShellcodeHandler(ShellcodeHandler *handler);
        virtual sch_result  handleShellcode(Message **msg);    
				sch_result fileCheck(Message **nmsg);

		bool Init();
		bool Exit();
		void doList();

    private:
        list <ShellcodeHandler *> m_ShellcodeHandlers;
	};

}

#endif
