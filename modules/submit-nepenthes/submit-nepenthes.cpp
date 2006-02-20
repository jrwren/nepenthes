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

#include <arpa/inet.h>

#include "SubmitNepenthesDialogue.hpp"

#include "submit-nepenthes.hpp"
#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"

#include "Config.hpp"

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
 * creates a new SubmitNepenthes Module, where SubmitNepenthes is public Module, public SubmitHanvler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the SubmitHandlerName
 * - sets the SubmitHandlerDescription
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
SubmitNepenthes::SubmitNepenthes(Nepenthes *nepenthes)
{
	m_ModuleName        = "submit-nepenthes";
	m_ModuleDescription = "give new toys to good friends";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_SubmitterName = "submit-nepenthes";
	m_SubmitterDescription = "sends malware to known friends";

	g_Nepenthes = nepenthes;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
SubmitNepenthes::~SubmitNepenthes()
{

}

/**
 * Module::Init()
 * register the submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a module loading error
 */
bool SubmitNepenthes::Init()
{

	logPF();

	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	try
	{
		m_Host = inet_addr(m_Config->getValString("submit-nepenthes.host"));
		m_Port = m_Config->getValInt("submit-nepenthes.port");
    } catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

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
bool SubmitNepenthes::Exit()
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
void SubmitNepenthes::Submit(Download *down)
{
	Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,m_Host,m_Port,m_ConnectTimeout);

	socket->addDialogue(new SubmitNepenthesDialogue(socket,
													down->getDownloadBuffer()->getData(),
													down->getDownloadBuffer()->getLength(),
													(char *)down->getMD5Sum().c_str()
													)
						);
}

/**
 * SubmitHandler::Hit(Download *down)
 * 
 * the file is known to the core, we could count the hits per session here
 * be we just do nothing
 * 
 * @param down   the download to hexdump
 */
void SubmitNepenthes::Hit(Download *down)
{
	return;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if(version == MODULE_IFACE_VERSION)
	{
		*module = new SubmitNepenthes(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}
