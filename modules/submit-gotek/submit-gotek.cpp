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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "submit-gotek.hpp"
#include "gotekCTRLDialogue.hpp"
#include "gotekDATADialogue.hpp"


#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"
#include "DownloadBuffer.hpp"
#include "Config.hpp"
#include "SocketManager.hpp"

#include "DNSManager.hpp"
#include "DNSResult.hpp"

#include "EventManager.hpp"
#include "EventHandler.cpp"

#include <cstdlib>

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sub | l_hlr

Nepenthes *g_Nepenthes;
GotekSubmitHandler *g_GotekSubmitHandler;

GotekSubmitHandler::GotekSubmitHandler(Nepenthes *nepenthes)
{

	m_ModuleName        = "submit-gotek";
	m_ModuleDescription = "send files to a G.O.T.E.K daemon";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_SubmitterName = "submit-file";
	m_SubmitterDescription = "store with md5sum as name in /tmp";

	g_Nepenthes = nepenthes;
	g_GotekSubmitHandler = this;
	
	REG_EVENT_HANDLER(this);
}

GotekSubmitHandler::~GotekSubmitHandler()
{

}

bool GotekSubmitHandler::Init()
{
	logPF();

	if ( m_Config == NULL )
	{
		logCrit("No G.O.T.E.K. Configuration given!\n");
		return false;
	}

	try
	{
		m_GotekHost = m_Config->getValString("submit-gotek.host");
	        m_GotekPort = (uint16_t) m_Config->getValInt("submit-gotek.port");

		m_User = m_Config->getValString("submit-gotek.user");
		m_CommunityKey = (unsigned char *) m_Config->getValString("submit-gotek.communitykey");

    	} catch ( ... )
	{
		logCrit("Could not get G.O.T.E.K. host/port/user/key from configuration!\n");
		return false;
	}
	
	try
	{
		if(m_Config->getValInt("submit-gotek.spool.enable"))
		{
			m_SpoolDirectory = m_Config->getValString("submit-gotek.spool.directory") + string("/");
			m_HandleSpool = true;
		} else
		{
			m_HandleSpool = false;			
		}
	} catch ( ... )
	{
		logCrit("Broken spool configuration, disabling.\n");
		m_HandleSpool = false;
	}

	m_ControlConnStatus = GSHS_RESOLVING;
	g_Nepenthes->getDNSMgr()->addDNS(this, (char *) m_GotekHost.c_str(), NULL);
    
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_SUBMIT_HANDLER(this);

	m_CTRLSocket = NULL;
	m_Timeout = 0;
	
	return scanSpoolDirectory();
}

bool GotekSubmitHandler::scanSpoolDirectory()
{
	if(!m_HandleSpool)
	{
		logInfo("G.O.T.E.K. spooling disabled, not scanning spool directory.\n");
		return true;
	}

	DIR * spoolStream;
	struct dirent * currentEntry;
	
	logPF();
	
	if((spoolStream = opendir(m_SpoolDirectory.c_str())) == NULL)
	{
		logCrit("Failed to open G.O.T.E.K. spool directory %s: %s!\n", m_SpoolDirectory.c_str(), strerror(errno));
		return false;
	}
	
	errno = 0;
	
	while((currentEntry = readdir(spoolStream)) != NULL)
	{
		string fileName = m_SpoolDirectory + string(currentEntry->d_name);
		struct stat fileStat;
		
		if(* currentEntry->d_name == '.')
		{
			// skip hidden files, `.' and `..'
			errno = 0;
			continue;
		}
		
		if(stat(fileName.c_str(), &fileStat) < 0)
		{
			logCrit("Checking \"%s\" in G.O.T.E.K. spool failed: %s!\n", fileName.c_str(), strerror(errno));
			
			errno = 0;			
			continue;
		}
		
		if(!S_ISREG(fileStat.st_mode))
		{
			// ignore directories, block devices, ...
			errno = 0;
			continue;
		}
		
		
		logInfo("Adding spool entry \"%s\" into list...\n", fileName.c_str());
		
		{
			GotekContext * ctx = new GotekContext;
			struct stat fileStat;
			unsigned char * fileBuffer;
			
			ctx->m_FileName = fileName;
			ctx->m_EvCID = 0;
			ctx->m_Length = 0;
			ctx->m_DataBuffer = 0;
			
			if(stat(ctx->m_FileName.c_str(), &fileStat) < 0)
			{
				logWarn("Error while accessing \"%s\": %s!\n", ctx->m_FileName.c_str(), strerror(errno));
				continue;
			}
			
			if(!S_ISREG(fileStat.st_mode))
			{
				logWarn("Spool file \"%s\" not regular!\n", ctx->m_FileName.c_str());
				continue;
			}
			
			ctx->m_Length = fileStat.st_size;
			
			fileBuffer = (unsigned char *) malloc(ctx->m_Length);
			assert(fileBuffer != NULL);
			
			FILE * filePointer = fopen(ctx->m_FileName.c_str(), "rb");
		
			if(!filePointer || fread(fileBuffer, 1, ctx->m_Length, filePointer) != ctx->m_Length)
			{
				logCrit("Failed to read data from spool file \"%s\"!", ctx->m_FileName.c_str());
				
				if(filePointer)
				{
					fclose(filePointer);
				}
				
				continue;
			}
			
			fclose(filePointer);
			
			g_Nepenthes->getUtilities()->sha512(fileBuffer, ctx->m_Length, ctx->m_Hash);
			free(fileBuffer);
			
			m_Goten.push_back(ctx);
		}
	
		errno = 0;
	}
	
	if(errno != 0)
	{
		logCrit("Error enumerating contents of spool directory %s: %s!\n", m_SpoolDirectory.c_str(), strerror(errno));
		
		closedir(spoolStream);
		return false;
	}
	
	closedir(spoolStream);	
	return true;
}

bool GotekSubmitHandler::Exit()
{
	return true;
}

void GotekSubmitHandler::Submit(Download *down)
{
	FILE * filePointer;
	string fileName = m_SpoolDirectory;
	GotekContext * ctx = new GotekContext;
	
	if(m_HandleSpool)
	{		
		{ // TODO substitute with clean std::string solution
			char * temp;
			
			if (asprintf(&temp, "sample-%u-%03u", (unsigned int) time(NULL), rand() % 1000) == -1) {
				logCrit("Memory allocation error\n");
				exit(EXIT_FAILURE);
			}
			fileName += temp;
			free(temp);
		}
		
		if(!(filePointer = fopen(fileName.c_str(), "wb")))
		{
			logWarn("Could not open \"%s\" for writing, discarding G.O.T.E.K. submission: %s!\n", fileName.c_str(), strerror(errno));
			return;
		}
		
		if(fwrite(down->getDownloadBuffer()->getData(), 1, down->getDownloadBuffer()->getSize(), filePointer) != down->getDownloadBuffer()->getSize())
		{
			logWarn("Could not write %u bytes submission to \"%s\": %s!\n", down->getDownloadBuffer()->getSize(), fileName.c_str(), strerror(errno));
			
			fclose(filePointer);
			return;
		}
		
		logInfo("G.O.T.E.K. Submission %s saved into %s\n", down->getMD5Sum().c_str(), fileName.c_str());	
		fclose(filePointer);
		
		ctx->m_FileName = fileName;
		ctx->m_EvCID = 0;
		memcpy(ctx->m_Hash, down->getSHA512(), 64);
		ctx->m_Length = down->getDownloadBuffer()->getSize();
		ctx->m_DataBuffer = 0;
		m_Goten.push_back(ctx);
	} else
	{
		if(m_ControlConnStatus != GSHS_CONNECTED)
		{
			logCrit("G.O.T.E.K. Submission %s lost, not connected!\n",down->getMD5Sum().c_str()); // spool here
			return;
		}
		
		logWarn("G.O.T.E.K. Submission %s\n", down->getMD5Sum().c_str());
		
		ctx->m_EvCID = 0; //down->getEvCID();
		ctx->m_Length = down->getDownloadBuffer()->getSize();
		ctx->m_DataBuffer = (unsigned char *) malloc(ctx->m_Length);
		memcpy(ctx->m_DataBuffer, down->getDownloadBuffer()->getData(), ctx->m_Length);
		memcpy(ctx->m_Hash, down->getSHA512(), 64);

		m_Goten.push_back(ctx);
	}

    	if(m_CTRLSocket != NULL)
	{
		unsigned char request[1+64+8];
		
		request[0] = 0x01;
		memcpy(request + 1, ctx->m_Hash, 64);
		memcpy(request + 65, &ctx->m_EvCID, 8);

		m_CTRLSocket->doWrite((char *) request, sizeof(request));
	}else
	{
		// shit happens for now
		logWarn("No G.O.T.E.K. control connection, saved to spool if enabled.\n");
	}

	return;
}


void GotekSubmitHandler::Hit(Download *down)
{
	Submit(down);
	return;
}


string GotekSubmitHandler::getUser()
{
	return m_User;
}

unsigned char* GotekSubmitHandler::getCommunityKey()
{
	return m_CommunityKey;
}

void GotekSubmitHandler::setSessionKey(uint64_t key)
{
	logInfo("G.O.T.E.K. Session key is 0x016lx.\n", key);
	m_Sessionkey = key;
}


bool GotekSubmitHandler::dnsResolved(DNSResult *result)
{
	list <uint32_t> resolved = result->getIP4List();
	uint32_t host = resolved.front();


	if(m_ControlConnStatus == GSHS_RESOLVING)
	{
		Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,host,m_GotekPort,14400);
		socket->addDialogue(new gotekCTRLDialogue(socket, result->getDNS(), this));	
		
		m_ControlConnStatus = GSHS_CONNECTED;
	} else
	{
		m_Timeout = 0; // if we're already / still waiting, reresolving perhaps solved shit - retry NOW
	}

	m_GotekHostIP = host;
	return true;
}

bool GotekSubmitHandler::dnsFailure(DNSResult *result)
{

	return true;

}

void GotekSubmitHandler::setSocket(Socket *s)
{
	m_CTRLSocket = s;
}


bool GotekSubmitHandler::popGote()
{
	if(m_HandleSpool)
	{
		if(unlink(m_Goten.front()->m_FileName.c_str()) < 0)
		{
			logCrit("Deleting existing file \"%s\" from spool failed: %s!\n", m_Goten.front()->m_FileName.c_str(), strerror(errno));
		}
	}

	m_Goten.pop_front();	
	
	return true;
}

bool GotekSubmitHandler::sendGote()
{
	logPF();
	GotekContext *ctx = m_Goten.front();
	gotekDATADialogue * dialogue = new gotekDATADialogue(ctx);
	
	if(!dialogue->loadFile())
	{
		logCrit("Failed to load G.O.T.E.K. submission \"%s\" for sending!\n", ctx->m_FileName.c_str());
		return false;
	}

	Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0,m_GotekHostIP,m_GotekPort,30);
	dialogue->setSocket(socket);
	socket->addDialogue(dialogue);
	popGote();
	
	return true;
}


void GotekSubmitHandler::childConnectionEtablished()
{
	if(m_HandleSpool)
	{
		for(list<GotekContext *>::iterator i = m_Goten.begin(); i != m_Goten.end(); ++i)
		{
			unsigned char request[1+64+8];
		
			request[0] = 0x01;
			memcpy(request + 1, (* i)->m_Hash, 64);
			memcpy(request + 65, &((* i)->m_EvCID), 8);

			m_CTRLSocket->doWrite((char *) request, sizeof(request));
		}
	}
}

void GotekSubmitHandler::childConnectionLost()
{
	m_CTRLSocket = NULL;
	
	m_Events.set(EV_TIMEOUT);
	
	switch(m_ControlConnStatus)
	{
	case GSHS_RESOLVING:
		logCrit("Lost child connection while resolving DNS -- impossible!\n\n");
		break;
		
	case GSHS_WAITING_SHORT:
		logInfo("G.O.T.E.K. reconnection attempt to \"%s\" failed, retrying in %i seconds.", m_GotekHost.c_str(), GOTEK_CTRL_WAIT);
		
		// first reconnection attempt failed, perhaps re-resolving helps?
		g_Nepenthes->getDNSMgr()->addDNS(this,(char *)m_GotekHost.c_str(), NULL);
		
		m_ControlConnStatus = GSHS_WAITING_SHORT;
		m_Timeout = time(0) + GOTEK_CTRL_WAIT;		
		break;
		
	case GSHS_CONNECTED:
		logCrit("G.O.T.E.K. connection to \"%s\" lost, reconnecting in %i seconds.\n", m_GotekHost.c_str(), GOTEK_CTRL_WAIT);
		m_ControlConnStatus = GSHS_WAITING_SHORT;
		m_Timeout = time(0) + GOTEK_CTRL_WAIT;
		break;
	}
}

uint32_t GotekSubmitHandler::handleEvent(Event * event)
{
	logPF();
	m_Events.reset(EV_TIMEOUT);
	
	if(m_ControlConnStatus == GSHS_WAITING_SHORT)
	{
		Socket *socket = g_Nepenthes->getSocketMgr()->connectTCPHost(0, m_GotekHostIP, m_GotekPort, 14400);
		socket->addDialogue(new gotekCTRLDialogue(socket, m_GotekHost, this));
		
		logInfo("Reconnecting to G.O.T.E.K. server \"%s\".\n", m_GotekHost.c_str());
		
		m_ControlConnStatus = GSHS_CONNECTED;
	}
	
	return 0;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if(version == MODULE_IFACE_VERSION)
	{
		*module = new GotekSubmitHandler(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}
