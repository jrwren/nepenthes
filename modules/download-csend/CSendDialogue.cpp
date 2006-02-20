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
 
#include <arpa/tftp.h>
#include <netinet/in.h>

#include "CSendDialogue.hpp"

#include "UDPSocket.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "SubmitManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_dia | l_hlr


using namespace nepenthes;



CSendDialogue::CSendDialogue(Socket *socket)
{
	m_DialogueName = "CSendDialogue";
	m_DialogueDescription = "download a file via tftp";

	m_Socket = socket;
	m_ConsumeLevel = CL_ASSIGN;
}


CSendDialogue::~CSendDialogue()
{
	logPF();
	delete m_Download;
}

void CSendDialogue::setDownload(Download *down)
{
	m_Download = down;
}


void CSendDialogue::setMaxFileSize(unsigned long ul)
{
	m_MaxFileSize = ul;
}

ConsumeLevel CSendDialogue::incomingData(Message *msg)
{
	logInfo("got %i bytes data\n",msg->getMsgLen());
	m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getMsgLen());
	return CL_ASSIGN;
}

ConsumeLevel CSendDialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

ConsumeLevel CSendDialogue::handleTimeout(Message *msg)
{
	
	return CL_DROP;
}

ConsumeLevel CSendDialogue::connectionLost(Message *msg)
{
    return CL_DROP;
}

ConsumeLevel CSendDialogue::connectionShutdown(Message *msg)
{
	logPF();
	msg->getSocket()->getNepenthes()->getSubmitMgr()->addSubmission(m_Download);
    return CL_DROP;
}
