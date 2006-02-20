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


#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "TFTPDownloadHandler.hpp"
#include "TFTPDialogue.hpp"

#include "FILESocket.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "SubmitManager.hpp"

#include "Config.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_hlr | l_mod


using namespace nepenthes;

Nepenthes *g_Nepenthes;

TFTPDownloadHandler::TFTPDownloadHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "TFTP Download Module";
	m_ModuleDescription = "provides a downloadhandler for tftp protocol";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DownloadHandlerName ="tftp download handler";
	m_DownloadHandlerDescription = "download files via tftp";

	m_DialogueFactoryName = "TFTPDialogueFactory";
	m_DialogueFactoryDescription = "creates a dialogue to download a file via tftp";

	g_Nepenthes = nepenthes;
}

TFTPDownloadHandler::~TFTPDownloadHandler()
{
	logPF();
}

bool TFTPDownloadHandler::Init()
{
	logPF();

	if ( m_Config == NULL )
	{
		logCrit("%s","I need a config\n");
		return false;
	}

	try
	{
		m_MaxFileSize = m_Config->getValInt("download-tftp.max-filesize");
		m_MaxResends = m_Config->getValInt("download-tftp.max-resends");
    } catch ( ... )
	{
		logCrit("%s","Error setting needed vars, check your config\n");
		return false;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_DOWNLOAD_HANDLER(this,"tftp");
	return true;
}

bool TFTPDownloadHandler::Exit()
{

	return true;
}

bool TFTPDownloadHandler::download(Download *down)
{
	logPF();
/*	Socket *socket = m_Nepenthes->getSocketMgr()->openFILESocket((char *)down->getDownloadUrl()->getPath().c_str(),O_RDONLY);
	Dialogue *xdown = createDialogue(socket);
	((TFTPDownloadHandlerDownload *)xdown)->setDownload(down);
	socket->addDialogue(xdown);
*/	
	int32_t Port = down->getDownloadUrl()->getPort();
	uint32_t Host = inet_addr(down->getDownloadUrl()->getHost().c_str());

	Socket *socket = m_Nepenthes->getSocketMgr()->connectUDPHost(INADDR_ANY,Host,Port,7);

	Dialogue *dia = createDialogue(socket);
    ((TFTPDialogue*)dia)->setDownload(down);

	((TFTPDialogue*)dia)->setMaxFileSize(m_MaxFileSize);	// FIXME CONFIG 4mb static value
	((TFTPDialogue*)dia)->setMaxRetries(m_MaxResends);				// FIXME CONFIG 7 retries static value

	socket->addDialogue(dia);
		// form & send request
	int32_t iReqLen = ((TFTPDialogue *) dia)->setRequest((char *)down->getDownloadUrl()->getFile().c_str());
	socket->doWrite(((TFTPDialogue *) dia)->getRequest(),iReqLen);

	return true;
}

Dialogue *TFTPDownloadHandler::createDialogue(Socket *socket)
{
	return new TFTPDialogue(socket);
//	return NULL;
}



extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new TFTPDownloadHandler(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
