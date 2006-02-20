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

#include "Socket.hpp"
#include "Responder.hpp"

namespace nepenthes
{

/*
  ---------------------------------------------- 
  | ip header | tcp header(or x header) | data | 
  ---------------------------------------------- 
                                                                      
  IP header structure: 
            4       8                16                            32 bit 
  |--------|--------|----------------|--------------------------------| 
  | Ver    | IHL    |Type of service |     Total length               | 
  |-------------------------------------------------------------------| 
  | Identification  |   Flags        |     Fragment offset            | 
  |----------------------------------|--------------------------------| 
  | Time to live    |   Protocol     |     Header checksum            | 
  |----------------------------------|--------------------------------| 
  |                            Source address                         | 
  |----------------------------------|--------------------------------| 
  |                          Destination address                      | 
  |----------------------------------|--------------------------------| 
  |                           Option + Padding                        | 
  |----------------------------------|--------------------------------| 
  |                                 Data                              | 
  -----------------------------------|--------------------------------| 

  TCP header structure: 
                                   16                               32 bit 
  |----------------------------------|--------------------------------| 
  |            Source port           |        Destination port        | 
  |----------------------------------|--------------------------------| 
  |                          Sequence number                          | 
  |----------------------------------|--------------------------------| 
  |                        Acknowledgement number                     | 
  |----------------------------------|--------------------------------| 
  | Offset | Resrvd      |U|A|P|R|S|F|             Window             | 
  |----------------------------------|--------------------------------| 
  |              Checksum            |          Urgent pointer        | 
  |----------------------------------|--------------------------------| 
  |                           Option + Padding                        | 
  |----------------------------------|--------------------------------| 
  |                                 Data                              | 
  |----------------------------------|--------------------------------|


  UDP Header
                                     16                               32 bit
  |----------------------------------|--------------------------------| 
  |            Source port           |        Destination port        | 
  |----------------------------------|--------------------------------| 
  |              Length              |          Checksum              |
  |----------------------------------|--------------------------------| 

*/

    struct iphdr
	{

		unsigned char       ip_length:4;
		unsigned char       ip_version:4;
		unsigned char       ip_tos;
		uint16_t      ip_total_length;
		uint16_t      ip_id;
		uint16_t      ip_flags;
		unsigned char       ip_ttl;
		unsigned char       ip_protocol;
		uint16_t      ip_cksum;
		uint32_t        ip_source;
		uint32_t        ip_dest;
	};

	struct tcphdr
	{
		uint16_t      tcp_source_port;
		uint16_t      tcp_dest_port;
		uint32_t        tcp_seqno;
		uint32_t        tcp_ackno;
		uint32_t        tcp_res1:4,
		tcp_hlen:4,
		tcp_fin:1,
		tcp_syn:1,
		tcp_rst:1,
		tcp_psh:1,
		tcp_ack:1,
		tcp_urg:1,
		tcp_res2:2;
		uint16_t      tcp_winsize;
		uint16_t      tcp_cksum;
		uint16_t      tcp_urgent;
	};

    struct udphdr
    {
        uint16_t udp_source_port;
        uint16_t udp_dest_port;
        uint16_t udp_length;
        uint16_t udp_chksum;
    };


	class RAWSocketReader : public Socket
	{
	public:
		RAWSocketReader(Nepenthes *nepenthes,uint32_t localhost,uint16_t localport,uint32_t remotehost,uint16_t remoteport, time_t connecttimeout, uint32_t protocoll);
		~RAWSocketReader();
		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int32_t doSend();
		int32_t doRecv();
		socket_state doRead(char *msg, uint32_t len);
		int32_t doWrite(char *msg, uint32_t len);
		bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, uint32_t len);
		string getDescription();
	protected:
		uint32_t				m_Protocoll;
	};

	struct ListenDialogueFactoryMap
	{
		uint16_t 			m_LocalPort;
		uint16_t 			m_RemotePort;

		uint16_t			m_Protocoll;
		list <DialogueFactory *> m_DialogueFactories;
	};

	class RAWSocketListener : public Socket
	{
	public:
		RAWSocketListener(Nepenthes *nepenthes, char *ninterface,uint32_t localhost, uint32_t protocoll);
        RAWSocketListener(Nepenthes *nepenthes, uint32_t localhost);
		~RAWSocketListener();
		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int32_t doSend();
		int32_t doRecv();
		
		int32_t doWrite(char *msg, uint32_t len);
		bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, uint32_t len);

		string getDescription();

		bool addListenPort(uint32_t port);
		bool addListenFactory(uint32_t localport, uint32_t remoteport,uint16_t protocoll, DialogueFactory *diaf);

        int32_t getSocket();

	protected:
		uint32_t				m_Protocoll;
		string  m_Interface;
		list <RAWSocketReader *>	m_Sockets;
		list <uint32_t>			m_ListenPorts;

		list<ListenDialogueFactoryMap *> m_ListenFactories;

	};
}
