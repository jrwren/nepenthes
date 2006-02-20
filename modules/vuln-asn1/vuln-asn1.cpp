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

#include <ctype.h>

#include "vuln-asn1.hpp"
#include "IISDialogue.hpp"
#include "SMBDialogue.hpp"
#include "sch_asn1_iis.hpp"
#include "sch_asn1_smb_bind.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.cpp"

#include "Config.hpp"

#include "ShellcodeManager.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;

/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * The Constructor
 * creates a new ASN1Vuln Module, 
 * ASN1Vuln is an example for binding a socket & setting up the Dialogue & DialogueFactory
 * 
 * 
 * it can be used as a shell emu to allow trigger commands 
 * 
 * 
 * sets the following values:
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
ASN1Vuln::ASN1Vuln(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-asn1";
	m_ModuleDescription = "provides dialogues and factories for asn1 flaw";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "ASN1 Dialogue Factory";
	m_DialogueFactoryDescription = "creates dialogues for the SMB and IIS flaw killbill showed us";

	g_Nepenthes = nepenthes;


}

ASN1Vuln::~ASN1Vuln()
{

}


/**
 * Module::Init()
 * 
 * binds the port, adds the DialogueFactory to the Socket
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool ASN1Vuln::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();

	int timeout;
	try
	{
		m_SMBPort 	= m_Config->getValInt("vuln-asn1.smbport");
		m_IISPort	= m_Config->getValInt("vuln-asn1.iisport");
		timeout 	= m_Config->getValInt("vuln-asn1.accepttimeout");

	} catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

    m_Nepenthes->getSocketMgr()->bindTCPSocket(0,m_IISPort,0,timeout,this);
	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,m_SMBPort,0,timeout,this);

	m_ShellcodeHandlers.push_back( new ASN1IISBase64	(m_Nepenthes->getShellcodeMgr()));
	m_ShellcodeHandlers.push_back( new ASN1SMBBind		(m_Nepenthes->getShellcodeMgr()));

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

bool ASN1Vuln::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new ASN1VulnDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *ASN1Vuln::createDialogue(Socket *socket)
{
//	return new ASN1VulnDialogue(socket);
	if ( socket->getLocalPort() == m_IISPort )
	{
		return new IISDialogue(socket);
	} else
	if ( socket->getLocalPort() == m_SMBPort )
	{
		return new SMBDialogue(socket);
	} 

	return NULL;
	
}



extern "C" int module_init(int version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new ASN1Vuln(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
