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

#define HAVE_PCAP
#define HAVE_IPQ

#include <pcap.h>



extern "C"
{
	#include <linux/netfilter.h>
	#include <libipq.h>
	#include <libnet.h>
}

#include <sys/types.h>
#include <netinet/in.h>


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

	/* IP header */
	struct ip_header
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		u_int ip_hl:4, /* header length */
		ip_v:4;	/* version */
#if BYTE_ORDER == BIG_ENDIAN
		u_int ip_v:4, /* version */
		ip_hl:4; /* header length */
#endif
#endif /* not _IP_VHL */
		u_char ip_tos; /* type of service */
		u_short ip_len;	/* total length */
		u_short ip_id; /* identification */
		u_short ip_off;	/* fragment offset field */
#define IP_RF 0x8000 /* reserved fragment flag */
#define IP_DF 0x4000 /* dont fragment flag */
#define IP_MF 0x2000 /* more fragments flag */
#define IP_OFFMASK 0x1fff /* mask for fragmenting bits */
		u_char ip_ttl; /* time to live */
		u_char ip_p; /* protocol */
		u_short ip_sum;	/* checksum */
		struct in_addr ip_src,ip_dst; /* source and dest address */
	};

/* tcp header */
	struct tcp_header
	{
		u_int16_t       th_sport;		/* tcp source port */
		u_int16_t       th_dport;		/* tcp dest port */
		u_int32_t       th_seqno;		/* tcp sequence number,identifies the byte in the stream of data */
		u_int32_t       th_ackno;		/* contains the next seq num that the sender expects to recieve */
		u_int8_t        th_res:4,		/* 4 reserved bits */
		th_doff:4;		/* data offset */
		u_int8_t        th_flags;
#define FIN 0x01
#define SYN 0x02
#define RST 0x04
#define PUSH 0x08
#define ACK 0x10
#define URG 0x20
#define ECE 0x40
#define CWR 0x80
#define FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
		u_int16_t       th_window;		/* maxinum number of bytes able to recieve*/
		u_int16_t       th_sum;			/* checksum to cover the tcp header and data portion of the packet*/
		u_int16_t       th_urp;			/* vaild only if the urgent flag is set, used to transmit emergency data */
	};


/* These enums are used by IPX too. :-( */
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
		HT_PCAP,
		HT_IPQ
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

		bool Exit();
		bool Exit_PCAP();
		bool Exit_IPQ();

		bool wantSend();

		int32_t doSend();

		int32_t doRecv();
		int32_t doRecv_PCAP();
		int32_t doRecv_IPQ();

		int32_t getSocket();
		int32_t   getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen);

		bool isPortListening(uint16_t localport, uint32_t localhost);

	protected:
#ifdef HAVE_PCAP
		pcap_t	*m_RawListener;
#endif
 
#ifdef HAVE_IPQ
		struct ipq_handle *m_IPQHandle;
#endif

		Nepenthes *m_Nepenthes;
		honeytrap_type m_HTType;
	};

}
extern nepenthes::Nepenthes *g_Nepenthes;
