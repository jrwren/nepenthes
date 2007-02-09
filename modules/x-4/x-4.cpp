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

#include "x-4.hpp"
#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"

using namespace nepenthes;


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;


/**
 * Constructor
 * creates a new X4 Module, where X4 is public Module, public SubmitHanvler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the SubmitHandlerName
 * - sets the SubmitHandlerDescription
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
X4::X4(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-4";
	m_ModuleDescription = "eXample Module 4 -submit handler example-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_SubmitterName = "hexdump";
	m_SubmitterDescription = "print file to console as hexdump";

	g_Nepenthes = nepenthes;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
X4::~X4()
{

}

/**
 * Module::Init()
 * register the submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a module loading error
 */
bool X4::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_SUBMIT_HANDLER(this);
	return true;
}


/**
 * Module::Exit()
 * 
 * unregister the Submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a heavy error
 */
bool X4::Exit()
{
	return true;
}


/**
 * SubmitHandler::Submit(Download *down)
 * 
 * get and submit a file.
 * here we just hexdump it to shell
 * 
 * @param down   the download to hexdump
 */
void X4::Submit(Download *down)
{
//	m_Nepenthes->getUtilities()->hexdump((byte *)down->getDownloadBuffer()->getData(),down->getDownloadBuffer()->getSize());
}

/**
 * SubmitHandler::Hit(Download *down)
 * 
 * the file is known to the core, we could count the hits per session here
 * be we just do nothing
 * 
 * @param down   the download to hexdump
 */
void X4::Hit(Download *down)
{
	return;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if(version == MODULE_IFACE_VERSION)
	{
		*module = new X4(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}
