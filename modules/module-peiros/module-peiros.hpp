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

 /* $Id: x-2.hpp 318 2006-02-20 08:03:24Z common $ */

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"
#include "ShellcodeManager.hpp"

using namespace std;


#include "PeirosParser.hpp"
using namespace peiros;


#include <stdint.h>


#include "TapInterface.hpp"


namespace nepenthes
{
	struct NetRange
	{
		uint8_t * usageMap;
		uint32_t baseAddress;
		uint32_t numAddresses;
		uint8_t prefixLength;
	};

	class Peiros : public Module, public DialogueFactory
	{
	public:
		Peiros(Nepenthes *);
		~Peiros();
		
		Dialogue * createDialogue(Socket *socket);
		
		bool Init();
		bool Exit();
		
		uint32_t allocateAddress();
		void freeAddress(uint32_t address);
		
	protected:
		bool initializeNetrange(const char * description);
		void uninitializeNetrange();
		
	private:
		TapInterface m_tapInterface;
		NetRange m_NetRange;
	};

	class PeirosDialogue : public Dialogue, public TapEncapsulator
	{
	public:
		PeirosDialogue(Socket *socket, string name, TapInterface * tap, Peiros * peiros);
		virtual ~PeirosDialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
		
		virtual void encapsulatePacket(const char * buffer, uint16_t length);

	protected:
		bool handleRequest(PeirosRequest request);
		bool parseAddress(const char * address, uint32_t * hostp, uint16_t * portp);
		sch_result analyzeShellcode(const char * data, unsigned int length,
			uint32_t srcHost, uint16_t srcPort, uint32_t dstHost, uint16_t dstPort);
		
	private:
		Peiros * m_peiros;
		PeirosParser m_peirosParser;
		TapInterface * m_tapInterface;
		string m_name;
		bool m_saidHello;
		uint32_t m_address, m_remoteAddress;
	};

}


extern nepenthes::Nepenthes *g_Nepenthes;
