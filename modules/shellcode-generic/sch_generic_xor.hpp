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

#ifndef HAVE_GENERICXOR_HPP
#define HAVE_GENERICXOR_HPP

#include <stdint.h>
#include <pcre.h>
#include "ShellcodeHandler.hpp"


namespace nepenthes
{
	struct XORPcreHelper
	{
		const char *m_PCRE;
		const char *m_Name;
		uint16_t m_Options; // will use this later

	};


	struct XORPcreContext
	{
		pcre *m_Pcre;
		string m_Name;
		uint16_t m_Options; // 
	};

	class GenericXOR : public ShellcodeHandler
	{
	public:
		GenericXOR(ShellcodeManager *shellcodemanager);
		~GenericXOR();
		sch_result handleShellcode(Message **msg);
		bool Init();
		bool Exit();
	protected:
        list <XORPcreContext *> m_Pcres;
	};
}

#endif
