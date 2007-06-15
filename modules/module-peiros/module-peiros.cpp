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

 /* $Id: x-2.cpp 550 2006-05-04 10:25:35Z common $ */

#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "module-peiros.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "Config.hpp"

#include "Download.hpp"

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
 * creates a new Peiros Module, 
 * Peiros is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
Peiros::Peiros(Nepenthes *nepenthes)
{
	g_Nepenthes = nepenthes;

	logPF();

	m_ModuleName        = "module-peiros";
	m_ModuleDescription = "Peiros server for shellcode handling and packet decapsulation.";
	m_ModuleRevision    = "$Rev: 550 $";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "module-peiros Factory";
	m_DialogueFactoryDescription = "Behind you!!1111";

	
}

Peiros::~Peiros()
{
	logPF();
}


/**
 * Module::Init()
 * 
 * binds the port, adds the DialogueFactory to the Socket
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool Peiros::Init()
{
	bool manageDefaultRoute = false;
	
	logPF();
	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	uint32_t port;
	int32_t timeout;
	
	m_NetRange.usageMap = 0;	
	
	try
	{
		port = m_Config->getValInt("module-peiros.port");
		
		timeout = 30;
		
		if(!initializeNetrange(m_Config->getValString("module-peiros.netrange")))
		{
			logCrit("Could not parse the network range \"%s\"! Use base/preflen format.\n", m_Config->getValString("module-peiros.netrange"));
			return false;
		}
	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config!\n");
		return false;
	}
	
	try
	{
		manageDefaultRoute = m_Config->getValString("module-peiros.manage-default-route") == string("yes");
	} catch ( ... )
	{ }
	
	{
		uint32_t netmask = 0;
		
		for(int a = 0; a < m_NetRange.prefixLength; ++a)
			netmask |= 1 << (31 - a);
			
		netmask = htonl(netmask);
		
		if(!m_tapInterface.Init(netmask, manageDefaultRoute))
		{
			logCrit("Failed to initialize TAP interface!\n");
			return false;
		}
	}

	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,port,0,timeout,this);
	
	return true;
}

bool Peiros::initializeNetrange(const char * description)
{
	logPF();
	string baseAddress;
	uint32_t prefixLength = 0;
	bool state = false;
	
	while(* description)
	{
		if(!state)
		{
			if(* description == '/')
				state = true;
			else
				baseAddress.push_back(* description);
		}
		else
		{
			if(* description < '0' || * description > '9')
				return false;
			else
				prefixLength = (prefixLength * 10) + (* description - '0');
		}
		
		++description;
	}
	
	if(prefixLength > 28)
	{
		logCrit("Offering less than 16 IPs through peiros interface: /%u\n", prefixLength);
		return false;
	}
	else if(prefixLength < 16)
	{
		logCrit("I cannot efficiently handle a prefix length < 16: /%u\n", prefixLength);
		return false;
	}
	
	if(inet_aton(baseAddress.c_str(), (struct in_addr *) &m_NetRange.baseAddress) == 0)
		return false;
	
	for(uint8_t i = 0; i < 32 - (uint8_t) prefixLength; ++i)
		m_NetRange.baseAddress &= htonl(~ (1 << i));
		
	m_NetRange.numAddresses = 1 << (32 - prefixLength);	
	m_NetRange.prefixLength = (uint8_t) prefixLength;
	m_NetRange.usageMap = (uint8_t *) malloc(m_NetRange.numAddresses >> 3);
	memset(m_NetRange.usageMap, 0, m_NetRange.numAddresses >> 3);
	
	return true;
}

uint32_t Peiros::allocateAddress()
{
	logPF();
	uint32_t address;
	
	for(address = 0; address < m_NetRange.numAddresses; ++address)
	{
		if((address & 0xff) == 0 || (address & 0xff) == 0xff)
			continue;
			
		if(!(m_NetRange.usageMap[address >> 3] & (1 << (address & 7))))
			break;
	}
			
	m_NetRange.usageMap[address >> 3] |= 1 << (address & 7);
	
	address = htonl(address + ntohl(m_NetRange.baseAddress));	
	return address;
}

void Peiros::freeAddress(uint32_t address)
{
	logPF();
	address = ntohl(address) - ntohl(m_NetRange.baseAddress);
	
	if(address > m_NetRange.numAddresses)
		return;
	
	m_NetRange.usageMap[address >> 3] &= ~ (1 << (address & 7));
}

bool Peiros::Exit()
{
	logPF();
	if(m_NetRange.usageMap)
	{
		free(m_NetRange.usageMap);
		m_NetRange.usageMap = 0;
	}
	
	return m_tapInterface.Exit();
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new PeirosDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *Peiros::createDialogue(Socket *socket)
{
	logPF();

	PeirosDialogue * dia;
	
	try
	{
		dia = new PeirosDialogue(socket, m_Config->getValString("module-peiros.name"), &m_tapInterface, this);
	}
	catch(...)
	{
		dia = new PeirosDialogue(socket, "##unnamed##", &m_tapInterface, this);
	}
	
	m_tapInterface.setEncapsulator(dia);
	return dia;
}







/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the PeirosDialogue, creates a new PeirosDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
PeirosDialogue::PeirosDialogue(Socket *socket, string name, TapInterface * tap, Peiros * peiros)
{
	logPF();

	m_Socket = socket;
    m_DialogueName = "PeirosDialogue";
	m_DialogueDescription = "handles peiros ctrl/encaps connections";
//	m_Timeout = 0;

	m_name = name;
	m_tapInterface = tap;
	m_peiros = peiros;
	
	m_ConsumeLevel = CL_ASSIGN;
	m_saidHello = false;
	
	m_remoteAddress = peiros->allocateAddress();
}

PeirosDialogue::~PeirosDialogue()
{
	logPF();

	m_peiros->freeAddress(m_remoteAddress);
}

ConsumeLevel PeirosDialogue::incomingData(Message *msg)
{
	logPF();

	if(!m_peirosParser.parseData(msg->getMsg(), msg->getSize()))
		return CL_DROP;
	
	while(m_peirosParser.hasRequest())
		if(!handleRequest(m_peirosParser.getRequest()))
			return CL_DROP;
	
	return CL_ASSIGN;
}

bool PeirosDialogue::handleRequest(PeirosRequest request)
{
	logPF();

	PeirosRequest response;
	string rbuf;
	
	if(request.command == "HELO")
	{
		if(request.resource == "shellcode-handling")
		{
			m_address = m_peiros->allocateAddress();
			struct in_addr conv = { m_address };
			
			logInfo("New Peiros client: %s/%s; will have %s\n", request.headers["Name"].c_str(), request.headers["Version"].c_str(), inet_ntoa(conv));
			
			response.command = "200";
			response.resource = "Welcome";
			response.headers["Name"] = m_name;
			response.headers["Version"] = "nepenthes/module-peiros v" VERSION;
			response.headers["Address"] = inet_ntoa(conv);
			
			conv.s_addr = m_remoteAddress;
			response.headers["Source-Address"] = inet_ntoa(conv);
			
			rbuf = m_peirosParser.renderRequest(&response);
			m_Socket->doRespond((char *) rbuf.data(), rbuf.size());
			
			if(!m_tapInterface->addAddress(m_address))
			{
				logCrit("Failed to add address %s to TUN.\n", inet_ntoa(conv));
				return false;
			}
			
			m_saidHello = true;
			
			return true;
		}
		else
		{
			response.command = "400";
			response.resource = "Feature unknown";
			response.headers["X-Offers"] = "shellcode-handling";
			
			rbuf = m_peirosParser.renderRequest(&response);
			m_Socket->doRespond((char *) rbuf.data(), rbuf.size());
			
			return false;
		}
	}
	else if(request.command == "HEAD")
	{
		response.command = "201";
		response.resource = "Features available";
		response.headers["Name"] = m_name;
		response.headers["Version"] = "nepenthes/module-peiros v" VERSION;
		response.headers["Offers"] = "shellcode-handling";
		
		rbuf = m_peirosParser.renderRequest(&response);
		m_Socket->doRespond((char *) rbuf.data(), rbuf.size());
		
		return true;
	}
	else if(m_saidHello && request.command == "BYE")
	{
		response.command = "200";
		response.resource = "Freeing resources";
		
		m_tapInterface->removeAddress(m_address);
		m_peiros->freeAddress(m_address);
		m_saidHello = false;
		
		rbuf = m_peirosParser.renderRequest(&response);
		m_Socket->doRespond((char *) rbuf.data(), rbuf.size());
	}
	else if(m_saidHello && request.command == "ANALYZE")
	{
		sch_result res;
		
		if(!request.resource.empty())
			return false;
			
		logInfo("Analyze request for %u bytes of data, exploit send from %s to %s!\n", request.appendedData.size(), request.headers["Source"].c_str(), request.headers["Target"].c_str());
		
		{
			uint32_t srcHost, dstHost;
			uint16_t srcPort, dstPort;
			
			if(!parseAddress(request.headers["Source"].c_str(), &srcHost, &srcPort) || !parseAddress(request.headers["Target"].c_str(), &dstHost, &dstPort))
			{
				response.command = "500";
				response.resource = "Could not parse Source/Target header!";
				
				rbuf = m_peirosParser.renderRequest(&response);
				m_Socket->doRespond((char *) rbuf.data(), rbuf.size());
				
				return false;
			}				
			
			res = analyzeShellcode(request.appendedData.data(), request.appendedData.size(),
				srcHost, srcPort, dstHost, dstPort);
		}
		
		if(res == SCH_DONE)
		{
			response.command = "200";
			response.resource = "Shellcode parsed and reaction initiated";
			
			rbuf = m_peirosParser.renderRequest(&response);
			m_Socket->doRespond((char *) rbuf.data(), rbuf.size());
			
			return true;
		}
		else
		{
			response.command = "400";
			response.resource = "Shellcode not detected";
			
			rbuf = m_peirosParser.renderRequest(&response);
			m_Socket->doRespond((char *) rbuf.data(), rbuf.size());
			
			return true;
		}
	}
	else if(request.command == "TRANS" && !request.appendedData.empty())
	{
		m_tapInterface->doWrite((char *) request.appendedData.data(), request.appendedData.size());
		return true;
	}
	
	return false;
}

sch_result PeirosDialogue::analyzeShellcode(const char * data, unsigned int length,
	uint32_t srcHost, uint16_t srcPort, uint32_t dstHost, uint16_t dstPort)
{
	logPF();

	const char * t;
	char * fixup = 0;
	sch_result res;
	
	for(t = data; t < data + 2; ++ t)
	{ // quick hack to detect unicode embedded shellcodes (SMB and stuff)
		unsigned int zero = 0;
		
		for(unsigned int i = 1; i < length - 1; i += 2)
			if(t[i] == 0)
				++zero;
			
		if(zero && ((float) zero / (float) length >= 0.35))
		{
			if(t != data)
				--length;
				
			fixup = (char *) malloc((length + 1) >> 1);
			
			for(unsigned int i = 0; i < length; i += 2)
				fixup[i >> 1] = t[i];
				
			length = (length + 1) >> 1;
			data = fixup;

			logInfo("Heuristic Unicode shellcode fixup performed (delta = %.2f)!\n",
				((float) zero / (float) length));
				
			break;
		}
	}
	
	Message * Msg = new Message((char *) data, length, srcPort, dstPort, srcHost, dstHost, 0, 0);
	res = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg);
	delete Msg;
	
	if(fixup)
		free(fixup);
	
	return res;
}

void PeirosDialogue::encapsulatePacket(const char * pkt, uint16_t length)
{
	logPF();

	char buf[32];
	PeirosRequest encaps;
	string encapsulated;
	
	snprintf(buf, sizeof(buf) - 1, "%hu", length);
	
	encaps.command = "TRANS";
	encaps.headers["Content-length"] = buf;
	
	encaps.appendedData.erase();
	encaps.appendedData.append(pkt, length);
	
	encapsulated = m_peirosParser.renderRequest(&encaps);
	m_Socket->doRespond((char *) encapsulated.data(), encapsulated.size());
}

bool PeirosDialogue::parseAddress(const char * address, uint32_t * hostp, uint16_t * portp)
{
	logPF();

	char * work = strdup(address);
	char * host = work, * port = work;
	
	while(* port && * port != ':')
		++port;
		
	if(!port)
		return false;
	
	* port = 0;
	++port;
	
	* hostp = (uint32_t) inet_addr(host);
	* portp = (uint16_t) atoi(port);
	
	free(work);
	return true;
}

ConsumeLevel PeirosDialogue::outgoingData(Message *msg)
{
	logPF();

	return CL_ASSIGN;
}

ConsumeLevel PeirosDialogue::handleTimeout(Message *msg)
{
	logPF();

	m_tapInterface->setEncapsulator(0);
	return CL_DROP;
}


ConsumeLevel PeirosDialogue::connectionLost(Message *msg)
{
	logPF();

	m_tapInterface->setEncapsulator(0);
	return CL_DROP;
}

ConsumeLevel PeirosDialogue::connectionShutdown(Message *msg)
{
	logPF();
	return CL_DROP;
}




#ifdef WIN32
extern "C" int32_t __declspec(dllexport)  module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#else
extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#endif

{
	if (version == MODULE_IFACE_VERSION) {
        *module = new Peiros(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
