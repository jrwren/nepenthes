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


#include "config.h"

#ifdef HAVE_PCAP
#include <pcap.h>
#endif


extern "C"
{
#ifdef HAVE_IPQ
	#include <linux/netfilter.h>
	#include <libipq.h>
#endif	
//	#include <libnet.h>
	#include <sys/types.h>
	#include <netinet/in.h>

}




#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"
#include "Socket.cpp"

#include "POLLSocket.hpp"
#include "POLLSocket.cpp"


using namespace std;

namespace nepenthes
{

#define ETHER_HDRLEN 16
#define IPQ_PACKET_BUFSIZE 2048
#define LIBNET_LIL_ENDIAN 1






/*
 *  IPv4 header
 *  Internet Protocol, version 4
 *  Static header size: 20 bytes
 *
 *  taken from libnet 1.1
 *
 */
struct libnet_ipv4_hdr
{
#if (LIBNET_LIL_ENDIAN)
    u_int8_t ip_hl:4,      /* header length */
           ip_v:4;         /* version */
#endif
#if (LIBNET_BIG_ENDIAN)
    u_int8_t ip_v:4,       /* version */
           ip_hl:4;        /* header length */
#endif
    u_int8_t ip_tos;       /* type of service */
#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY      0x10
#endif
#ifndef IPTOS_THROUGHPUT
#define IPTOS_THROUGHPUT    0x08
#endif
#ifndef IPTOS_RELIABILITY
#define IPTOS_RELIABILITY   0x04
#endif
#ifndef IPTOS_LOWCOST
#define IPTOS_LOWCOST       0x02
#endif
    u_int16_t ip_len;         /* total length */
    u_int16_t ip_id;          /* identification */
    u_int16_t ip_off;
#ifndef IP_RF
#define IP_RF 0x8000        /* reserved fragment flag */
#endif
#ifndef IP_DF
#define IP_DF 0x4000        /* dont fragment flag */
#endif
#ifndef IP_MF
#define IP_MF 0x2000        /* more fragments flag */
#endif
#ifndef IP_OFFMASK
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */
#endif
    u_int8_t ip_ttl;          /* time to live */
    u_int8_t ip_p;            /* protocol */
    u_int16_t ip_sum;         /* checksum */
    struct in_addr ip_src, ip_dst; /* source and dest address */
};

/*
 *  TCP header
 *  Transmission Control Protocol
 *  Static header size: 20 bytes
 *
 *  taken from libnet 1.1
 *
 */
struct libnet_tcp_hdr
{
    u_int16_t th_sport;       /* source port */
    u_int16_t th_dport;       /* destination port */
    u_int32_t th_seq;          /* sequence number */
    u_int32_t th_ack;          /* acknowledgement number */
#if (LIBNET_LIL_ENDIAN)
    u_int8_t th_x2:4,         /* (unused) */
           th_off:4;        /* data offset */
#endif
#if (LIBNET_BIG_ENDIAN)
    u_int8_t th_off:4,        /* data offset */
           th_x2:4;         /* (unused) */
#endif
    u_int8_t  th_flags;       /* control flags */
#ifndef TH_FIN
#define TH_FIN    0x01      /* finished send data */
#endif
#ifndef TH_SYN
#define TH_SYN    0x02      /* synchronize sequence numbers */
#endif
#ifndef TH_RST
#define TH_RST    0x04      /* reset the connection */
#endif
#ifndef TH_PUSH
#define TH_PUSH   0x08      /* push data to the app layer */
#endif
#ifndef TH_ACK
#define TH_ACK    0x10      /* acknowledge */
#endif
#ifndef TH_URG
#define TH_URG    0x20      /* urgent! */
#endif
#ifndef TH_ECE
#define TH_ECE    0x40
#endif
#ifndef TH_CWR
#define TH_CWR    0x80
#endif
    u_int16_t th_win;         /* window */
    u_int16_t th_sum;         /* checksum */
    u_int16_t th_urp;         /* urgent pointer */
};



/* These enums are used by IPX too. :-( 
 * 
 * mappings to determine the state of a tcp connection in /proc/net/tcp
 *
 * taken from net-tools
 *
 */
	enum
	{
		TCP_ESTABLISHED = 1,
		TCP_SYN_SENT,
		TCP_SYN_RECV,
		TCP_FIN_WAIT1,
		TCP_FIN_WAIT2,
		TCP_TIME_WAIT,
		TCP_CLOSE,
		TCP_CLOSE_WAIT,
		TCP_LAST_ACK,
		TCP_LISTEN,
		TCP_CLOSING					/* now a valid state */
	};


	class Buffer;

	typedef enum 
	{
		HT_NONE,
		HT_PCAP,
		HT_IPQ,
		HT_IPFW
	} honeytrap_type;

	class ModuleHoneyTrap : public Module, public POLLSocket //, public DialogueFactory
	{
	public:
		ModuleHoneyTrap(Nepenthes *);
		~ModuleHoneyTrap();
//		Dialogue *createDialogue(Socket *socket);
		bool Init();
		bool Init_PCAP();
		bool Init_IPQ();
		bool Init_IPFW();

		bool Exit();
		bool Exit_PCAP();
		bool Exit_IPQ();
		bool Exit_IPFW();

		bool wantSend();

		int32_t doSend();

		int32_t doRecv();
		int32_t doRecv_PCAP();
		int32_t doRecv_IPQ();
		int32_t doRecv_IPFW();

		int32_t getSocket();
		int32_t   getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen);

		bool isPortListening(uint16_t localport, uint32_t localhost);

		void printIPpacket(unsigned char *buf, uint32_t len);

	protected:
#ifdef HAVE_PCAP
		pcap_t	*m_RawListener;
		int 	m_LinkLayerHeaderLength;
		string 	m_PcapDevice;
#endif
 
#ifdef HAVE_IPQ
		struct ipq_handle *m_IPQHandle;
#endif

#ifdef HAVE_IPFW
		uint16_t			m_DivertPort;
		int 				m_DivertSocket;
		struct sockaddr_in 	m_DivertSin;
		socklen_t			m_DivertSinLen;
#endif

		Nepenthes *m_Nepenthes;
		honeytrap_type m_HTType;
	};

}
extern nepenthes::Nepenthes *g_Nepenthes;
