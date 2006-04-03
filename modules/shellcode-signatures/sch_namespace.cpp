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

#include <stdint.h>

#include "sch_namespace.hpp"

#include "Nepenthes.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "Utilities.hpp"
#include "Socket.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "DownloadManager.hpp"


#include "parser.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_hlr

using namespace nepenthes;

NamespaceShellcodeHandler::NamespaceShellcodeHandler(sc_shellcode *sc)
{
	m_ShellcodeHandlerName = sc_get_namespace_by_numeric(sc->nspace);
	m_ShellcodeHandlerName += "::";
	m_ShellcodeHandlerName += sc->name;

	int i;
	for ( i=0;i< sc->map_items;i++ )
	{
		m_Map[i] = sc->map[i];
	}
	m_MapItems = sc->map_items;

	if (sc->pattern != NULL)
	{
		m_Pattern 	= sc->pattern;
	}else
	{
		m_Pattern 	= "";
	}
	

	if (sc->author != NULL)
	{
		m_Author 	= sc->author;
	}else
	{
		m_Author 	= "unknown";
	}
	
	

	if (sc->reference != NULL)
	{
		m_Reference	= sc->reference;
	}else
	{
		m_Reference = "no docs";
	}

	m_Pcre = NULL;
}


NamespaceShellcodeHandler::~NamespaceShellcodeHandler()
{

}

bool NamespaceShellcodeHandler::Init()
{
	const char * pcreEerror;
	int32_t pcreErrorPos;

	if ( (m_Pcre = pcre_compile(m_Pattern.c_str(), PCRE_DOTALL, &pcreEerror, (int *)&pcreErrorPos, 0)) == NULL )
	{
		logCrit("%s could not compile pattern \n\t\"%s\"\n\t Error:\"%s\" at Position %u", 
				m_ShellcodeHandlerName.c_str(), pcreEerror, pcreErrorPos);
		return false;
	} else
	{
		logInfo("%s loaded ...\n",m_ShellcodeHandlerName.c_str());
	}

//	printf("%s\n",m_Pattern.c_str());
//	g_Nepenthes->getUtilities()->hexdump((byte *)m_Shellcode->pattern,m_Shellcode->pattern_size);
	return true;
}

bool NamespaceShellcodeHandler::Exit()
{
	if (m_Pcre != NULL)
	{
    	pcre_free(m_Pcre);
	}
	return true;
}




