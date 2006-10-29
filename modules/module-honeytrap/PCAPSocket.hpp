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

#ifndef HAVE_PCAPSOCKET_HPP
#define HAVE_PCAPSOCKET_HPP


#include "config.h"

#ifdef HAVE_PCAP
#include <pcap.h>
#endif




#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Socket.hpp"


#include "POLLSocket.hpp"



using namespace std;

namespace nepenthes
{

	class PCAPSocket : public POLLSocket
	{
	public:
		PCAPSocket(uint32_t remotehost, uint16_t remoteport, uint32_t localhost, uint16_t localport);
		~PCAPSocket();
		bool Init();
		bool Exit();
		bool wantSend();

		int32_t doSend();

		int32_t doRecv();
		int32_t getSocket();
		int32_t getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen);

		bool checkTimeout();

		void active();

		void dead();


	private:
#ifdef HAVE_PCAP
			pcap_t 			*m_PcapSniffer;
			pcap_dumper_t 	*m_PcapDumper;
#endif
			uint32_t 		m_PacketCount;
			string 			m_NetworkDevice;
			string 			m_DumpFilePath;
		};
}
#endif
