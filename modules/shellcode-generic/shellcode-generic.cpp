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



/* Additional notes:
 *
 * The pcre's and processing logic in this module is derived from mwcollect written by Georg Wicherski
 *
 */

 
 /* $Id$ */

#include "shellcode-generic.hpp"

#include "sch_generic_xor.hpp"
#include "sch_generic_createprocess.hpp"
#include "sch_generic_url.hpp"
#include "sch_generic_linkxor.hpp"
#include "sch_generic_cmd.hpp"
#include "sch_generic_link_trans.hpp"
#include "sch_generic_link_bind_trans.hpp"
#include "sch_generic_stuttgart.hpp"
#include "sch_generic_wuerzburg.hpp"
#include "sch_generic_bielefeld_connect.hpp"
#include "sch_generic_mainz_bind.hpp"
#include "sch_generic_bind.hpp"
#include "sch_generic_connect.hpp"
#include "sch_generic_konstanz_xor.hpp"
#include "sch_generic_connect_trans.hpp"

#include "sch_generic_unicode.hpp"
#include "sch_generic_winexec.hpp"
#include "sch_generic_leimbach_url_xor.hpp"
#include "sch_generic_wget.hpp"

#include "ShellcodeManager.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_dia | l_hlr



using namespace nepenthes;
Nepenthes *g_Nepenthes;
GenericShellcodeHandler *g_GenericShellcodeHandler;

GenericShellcodeHandler::GenericShellcodeHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "generic shellcode module";
	m_ModuleDescription = "prove xor, url and createprocess shelldecoder";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

//	m_ShellcodeHandlers.push_back(new GenericXOR(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new GenericCreateProcess(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new GenericUrl(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new LinkXOR(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new GenericCMD(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new LinkTrans(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new LinkBindTrans(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new Stuttgart(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new Wuerzburg(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new BieleFeldConnect(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new MainzBind(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new GenericBind(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new GenericConnect(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new KonstanzXOR(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new GenericConnectTrans(m_Nepenthes->getShellcodeMgr()));

//	m_ShellcodeHandlers.push_back(new GenericUniCode(m_Nepenthes->getShellcodeMgr()));
//	m_ShellcodeHandlers.push_back(new GenericWinExec(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new LeimbachUrlXORXOR(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back(new Genericwget(m_Nepenthes->getShellcodeMgr()));

	g_Nepenthes = nepenthes;
	g_GenericShellcodeHandler = this;
}

GenericShellcodeHandler::~GenericShellcodeHandler()
{
	Exit();
}
/*
struct pcremap
{
	char 			*m_pcreString;
    uint32_t	*m_retval;
	pcre    		**m_pcre;
};
*/
bool GenericShellcodeHandler::Init()
{
	if (m_Config == NULL)
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	m_ModuleManager 	= m_Nepenthes->getModuleMgr();


	list <ShellcodeHandler *>::iterator handler;
	for (handler = m_ShellcodeHandlers.begin(); handler != m_ShellcodeHandlers.end(); handler++)
	{
		if ((*handler)->Init() == false)
		{
			logCrit("ERROR %s\n",__PRETTY_FUNCTION__);
			return false;
		}
		REG_SHELLCODE_HANDLER((*handler));

	}
	return true;
}

bool GenericShellcodeHandler::Exit()
{
	list <ShellcodeHandler *>::iterator handler;
	for (handler = m_ShellcodeHandlers.begin(); handler != m_ShellcodeHandlers.end(); handler++)
	{
		if ((*handler)->Exit() == false)
		{
			logCrit("ERROR %s\n",__PRETTY_FUNCTION__);
			return false;
		}
		m_Nepenthes->getShellcodeMgr()->unregisterShellcodeHandler((*handler));
	}
	return true;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new GenericShellcodeHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
