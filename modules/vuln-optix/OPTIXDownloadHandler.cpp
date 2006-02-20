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


#include "OPTIXDownloadHandler.hpp"
#include "OPTIXDownloadDialogue.hpp"
#include "OPTIXBindDialogue.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "Download.hpp"

#ifdef STDTAGS
#undef STDTAGS
#endif

#define STDTAGS l_hlr | l_dia | l_dl | l_mgr

using namespace nepenthes;

OPTIXDownloadHandler::OPTIXDownloadHandler(Nepenthes *nepenthes)
{
	logPF();
	m_DownloadHandlerName = "Optix Download Handler";
	m_DownloadHandlerDescription = "download files via optix";

	m_DialogueFactoryName = "Optix DownloadHandler DialogueFactory";
	m_DialogueFactoryDescription = "creates a dialogue to download a file from via the optix bindport 500";

	m_Socket = NULL;
}

OPTIXDownloadHandler::~OPTIXDownloadHandler()
{
	logPF();
}

bool OPTIXDownloadHandler::Init()
{
	return true;
}

bool OPTIXDownloadHandler::Exit()
{
	return true;
}

Dialogue *OPTIXDownloadHandler::createDialogue(Socket *socket)
{
	return new OPTIXDownloadDialogue(socket);
}

bool OPTIXDownloadHandler::download(Download *down)
{
	if (m_Socket == NULL)
	{
		if ((m_Socket = g_Nepenthes->getSocketMgr()->bindTCPSocket(0,500,45,120,this)) == NULL)
		{
			logCrit("Optix .. error binding port %i\n",500);
			return false;
		}else
		{
			m_Dialogue = new OPTIXBindDialogue(m_Socket,this);
			m_Socket->addDialogue(m_Dialogue);
		}

	}
	delete down;
	return true;
}


void OPTIXDownloadHandler::setSocket(Socket *s)
{
	m_Socket = s;
}

void OPTIXDownloadHandler::setDialogue(Dialogue *dia)
{
	m_Dialogue = dia;
}
