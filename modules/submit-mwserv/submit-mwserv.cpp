/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2007 Georg Wicherski <gw@mwcollect.org>
 * Copyright (C) 2005 Paul Baecher & Markus Koetter
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

#include "Download.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"
#include "Event.hpp"
#include "EventManager.hpp"
#include "EventHandler.cpp"
#include "Config.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"

#include "DownloadBuffer.hpp"
#include "DownloadUrl.hpp"

#include "submit-mwserv.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SUBMIT_URI "nepenthes/submit"
#define HEARTBEAT_URI "nepenthes/heartbeat"


namespace nepenthes
{


SubmitMwservModule::SubmitMwservModule(Nepenthes * nepenthes)
{
	m_ModuleName = "submit-mwserv";
	m_ModuleDescription = "mwserv.py HTTP Post Submission";
	m_ModuleRevision = "$Rev: 921 $";
	m_Nepenthes = nepenthes;
	m_SubmitterName = "submit-mwserv";
	m_SubmitterDescription = "mwserv.py HTTP Post Submission";

	m_Timeout = time(0);
	m_Events.reset();
}

bool SubmitMwservModule::Init()
{
	if(!m_Config)
	{
		logCrit("No configuration for submit-mwserv provided.\n");
		return false;
	}
	
	try
	{
		m_url = m_Config->getValString("submit-mwserv.url");
		m_guid = m_Config->getValString("submit-mwserv.guid");
		m_maintainer = m_Config->getValString("submit-mwserv.maintainer");
		m_secret = m_Config->getValString("submit-mwserv.secret");
	}
	catch(...)
	{
		logCrit("Missing configuration option for submit-mwserv.\n");
		return false;
	}
	
	if(m_guid.find(":") != string::npos || m_maintainer.find(":")
		!= string::npos || m_secret.find(":") != string::npos ||
		m_guid.find("+") != string::npos || m_maintainer.find("+")
		!= string::npos || m_secret.find("+") != string::npos)
	{
		logCrit("submit-mwserv: guid, maintainer or secret from configuration"
			"contained ':' or '+'; this is not allowed.\n");
		return false;
	}
	
	REG_SUBMIT_HANDLER(this);
	REG_EVENT_HANDLER(this);
	
	return true;
}

bool SubmitMwservModule::Exit()
{
	return true;
}

void SubmitMwservModule::Submit(Download * download)
{
	Hit(download);
}

void SubmitMwservModule::Hit(Download * download)
{
	TransferSample sample;
	TransferSession * session = new TransferSession(TransferSession::
		TSS_INSTANCE, this);
	
	{
		struct in_addr saddr, daddr;
	
		saddr.s_addr = download->getRemoteHost();
		daddr.s_addr = download->getLocalHost();

		sample.saddr = inet_ntoa(saddr);
		sample.daddr = inet_ntoa(daddr);

		sample.guid = m_guid;
		sample.maintainer = m_maintainer;
		sample.secret = m_secret;
		
		sample.url = download->getUrl();
		sample.sha512 = download->getSHA512Sum();
		
		sample.binarySize = download->getDownloadBuffer()->getSize();
		sample.binary = new char[sample.binarySize];
		memcpy(sample.binary, download->getDownloadBuffer()->getData(),
			sample.binarySize);
	}
	
	session->transferSample(sample, m_url + SUBMIT_URI);
	g_Nepenthes->getSocketMgr()->addPOLLSocket(session);
}

void SubmitMwservModule::retrySample(TransferSample& sample)
{
}

void SubmitMwservModule::submitSample(TransferSample& sample)
{
	TransferSession * session = new TransferSession(TransferSession::
		TSS_SAMPLE, this);
	
	session->transferSample(sample, m_url + SUBMIT_URI);
	g_Nepenthes->getSocketMgr()->addPOLLSocket(session);
}

uint32_t SubmitMwservModule::handleEvent(Event * ev)
{
	return 0;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	g_Nepenthes = nepenthes;
	
	if(version == MODULE_IFACE_VERSION)
	{
		* module = new SubmitMwservModule(nepenthes);
		return 1;
	}
	
	return 0;
}


}

Nepenthes * g_Nepenthes;
