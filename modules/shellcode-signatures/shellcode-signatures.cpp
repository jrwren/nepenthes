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

#include "shellcode-signatures.hpp"

#include "sch_namespace_xor.hpp"
#include "sch_namespace_bindshell.hpp"
#include "sch_namespace_connectbackshell.hpp"
#include "sch_namespace_execute.hpp"
#include "sch_namespace_url.hpp"
#include "sch_namespace_linkxor.hpp"
#include "sch_namespace_connectbackfiletransfer.hpp"
#include "sch_namespace_bindfiletransfer.hpp"
#include "sch_namespace_base64.hpp"
#include "sch_engine_unicode.hpp"
#include "sch_namespace_konstanzxor.hpp"

#include "ShellcodeManager.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Message.cpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_dia | l_hlr



using namespace nepenthes;
Nepenthes *g_Nepenthes;
SignatureShellcodeHandler *g_SignatureShellcodeHandler;

SignatureShellcodeHandler::SignatureShellcodeHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "signature shellcode module";
	m_ModuleDescription = "signature based shellcode handler with patterns in a seperate file";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;
	g_SignatureShellcodeHandler = this;
}

SignatureShellcodeHandler::~SignatureShellcodeHandler()
{
	logPF();
}

bool SignatureShellcodeHandler::Init()
{
	m_ModuleManager 	= m_Nepenthes->getModuleMgr();

g_Nepenthes->getShellcodeMgr()->registerShellcodeHandler(new EngineUnicode());
	return loadSignaturesFromFile(string("var/cache/nepenthes/signatures/shellcode-signatures.sc"));
}

bool SignatureShellcodeHandler::Exit()
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

bool SignatureShellcodeHandler::loadSignaturesFromFile(string path)
{
	logInfo("Loading signatures from file %s\n",path.c_str());
	sc_shellcode *sc,*sc_free;
	bool load_success = true;

	if ( (sc = sc_parse_file(path.c_str())) == NULL)
	{
		logCrit("could not parse shellcodes from file %s\n",path.c_str());
		logCrit("error %s\n",sc_get_error());
		return false;
	}

	sc_free = sc;


	ShellcodeHandler *sch = NULL;

	while (sc != NULL && load_success == true )
	{
		if (sc->name == NULL)
		{
			sc = sc->next;
        	continue;
		}

		sch = NULL;

		switch(sc->nspace)
		{
		case sc_xor:
			sch = new NamespaceXOR(sc);
			break;

		case sc_linkxor:
			sch = new NamespaceLinkXOR(sc);
			break;

		case sc_konstanzxor:
			sch = new NamespaceKonstanzXOR(sc);
			break;

		case sc_leimbachxor:
			break;

		case sc_connectbackshell:
			sch = new NamespaceConnectbackShell(sc);
			break;

		case sc_connectbackfiletransfer:
			sch = new NamespaceConnectbackFiletransfer(sc);
			break;

		case sc_bindshell:
			sch = new NamespaceBindShell(sc);
			break;

		case sc_execute:
			sch = new NamespaceExecute(sc);
			break;

		case sc_download:
			break;

		case sc_url:
			sch = new NamespaceUrl(sc);
			break;

		case sc_bindfiletransfer:
			sch = new NamespaceBindFiletransfer(sc);
			break;

		case sc_base64:
			sch = new NamespaceBase64(sc);
			break;
		}
		

		if ( sch != NULL )
		{
			if ( sch->Init() == false )
			{
				load_success = false;
			}else
			{
				g_Nepenthes->getShellcodeMgr()->registerShellcodeHandler(sch);
			}
		}

		sc = sc->next;
	}

	

	return load_success;

}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new SignatureShellcodeHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
