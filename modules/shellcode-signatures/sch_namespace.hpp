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


#ifndef HAVE_NAMESPACE_HPP
#define HAVE_NAMESPACE_HPP

#include <pcre.h>
#include <stdint.h>
#include <stdio.h>
#include "ShellcodeHandler.hpp"
#include "parser.hpp"


namespace nepenthes
{
	class NamespaceShellcodeHandler : public ShellcodeHandler
	{
	public:
		NamespaceShellcodeHandler(sc_shellcode *sc);
		~NamespaceShellcodeHandler();
		virtual sch_result handleShellcode(Message **msg)=0;
		bool Init();
		bool Exit();
	protected:

		pcre 	*m_Pcre;

		string m_Author;
		string m_Reference;
		string m_Pattern;
		int m_MapItems;
		enum sc_mapping m_Map[MAP_MAX];
		int flags;

	};
}
#endif
