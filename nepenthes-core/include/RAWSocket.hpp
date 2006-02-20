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
		unsigned short      ip_total_length;
		unsigned short      ip_id;
		unsigned short      ip_flags;
		unsigned char       ip_ttl;
		unsigned char       ip_protocol;
		unsigned short      ip_cksum;
		unsigned int        ip_source;
		unsigned int        ip_dest;
	};

	struct tcphdr
	{
		unsigned short      tcp_source_port;
		unsigned short      tcp_dest_port;
		unsigned int        tcp_seqno;
		unsigned int        tcp_ackno;
		unsigned int        tcp_res1:4,
		tcp_hlen:4,
		tcp_fin:1,
		tcp_syn:1,
		tcp_rst:1,
		tcp_psh:1,
		tcp_ack:1,
		tcp_urg:1,
		tcp_res2:2;
		unsigned short      tcp_winsize;
		unsigned short      tcp_cksum;
		unsigned short      tcp_urgent;
	};

    struct udphdr
    {
        unsigned short udp_source_port;
        unsigned short udp_dest_port;
        unsigned short udp_length;
        unsigned short udp_chksum;
    };


	class RAWSocketReader : public Socket
	{
	public:
		RAWSocketReader(Nepenthes *nepenthes,unsigned long localhost,unsigned short localport,unsigned long remotehost,unsigned short remoteport, time_t connecttimeout, unsigned int protocoll);
		~RAWSocketReader();
		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int doSend();
		int doRecv();
		socket_state doRead(char *msg, unsigned int len);
		int doWrite(char *msg, unsigned int len);
		bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, unsigned int len);
		string getDescription();
	protected:
		unsigned int				m_Protocoll;
	};

	struct ListenDialogueFactoryMap
	{
		unsigned short 			m_LocalPort;
		unsigned short 			m_RemotePort;

		unsigned short			m_Protocoll;
		list <DialogueFactory *> m_DialogueFactories;
	};

	class RAWSocketListener : public Socket
	{
	public:
		RAWSocketListener(Nepenthes *nepenthes, char *ninterface,unsigned long localhost, unsigned int protocoll);
        RAWSocketListener(Nepenthes *nepenthes, unsigned long localhost);
		~RAWSocketListener();
		bool bindPort();
		bool Init();
		bool Exit();
		bool connectHost();
		Socket * acceptConnection();
		bool wantSend();

		int doSend();
		int doRecv();
		
		int doWrite(char *msg, unsigned int len);
		bool checkTimeout();
		bool handleTimeout();
		bool doRespond(char *msg, unsigned int len);

		string getDescription();

		bool addListenPort(unsigned int port);
		bool addListenFactory(unsigned int localport, unsigned int remoteport,unsigned short protocoll, DialogueFactory *diaf);

        int getSocket();

	protected:
		unsigned int				m_Protocoll;
		string  m_Interface;
		list <RAWSocketReader *>	m_Sockets;
		list <unsigned int>			m_ListenPorts;

		list<ListenDialogueFactoryMap *> m_ListenFactories;

	};
}
