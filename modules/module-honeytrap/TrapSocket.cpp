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

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>




#include "TrapSocket.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"

#include "DialogueFactoryManager.hpp"
#include "Utilities.hpp"

#include "Buffer.hpp"

#include "Message.hpp"

#include "ShellcodeManager.hpp"

#include "Config.hpp"

#include "Download.hpp"

#include "module-honeytrap.hpp"
#include "PCAPSocket.hpp"

#ifdef STDTAGS 
	#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;


TrapSocket::TrapSocket()
{
	m_HTType = HT_NONE;
#ifdef HAVE_PCAP
	m_RawListener = NULL;
#endif

#ifdef HAVE_IPQ
	m_IPQHandle = NULL;
#endif 

#ifdef HAVE_IPFW
	m_DivertSocket = -1;
#endif

	
}

TrapSocket::TrapSocket(string pcap_device)
{
	TrapSocket();
#ifdef HAVE_PCAP
	m_PcapDevice = pcap_device;
	m_HTType = HT_PCAP;
#else 
	m_HTType = HT_NONE;
#endif

	m_DialogueFactory = "bridge Factory";

}

TrapSocket::TrapSocket(uint16_t divert_port)
{
	TrapSocket();
	m_HTType = HT_IPFW;
#ifdef HAVE_IPFW
	m_DivertPort = divert_port;
#else 
	m_HTType = HT_NONE;
#endif
	m_DialogueFactory = "bridge Factory";
}

TrapSocket::TrapSocket(bool param)
{
	TrapSocket();
	m_HTType = HT_IPQ;
	m_DialogueFactory = "bridge Factory";
}



TrapSocket::~TrapSocket()
{

}


/**
 * Module::Init()
 * 
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool TrapSocket::Init()
{
	bool retval = false;
	switch ( m_HTType )
	{
	case HT_PCAP:
		retval = Init_PCAP();
		break;

	case HT_IPQ:
		retval = Init_IPQ();
		break;

	case HT_IPFW:
		retval = Init_IPFW();
		break;

	default:
		logCrit("Invalid mode for module-honeytrap\n");
		return false;

	}

	if ( retval == true )
	{
		g_Nepenthes->getSocketMgr()->addPOLLSocket(this);
		return true;
	}
	else
	{
		return false;
	}

}

bool TrapSocket::Init_IPQ()
{
#ifdef HAVE_IPQ
	if ( (m_IPQHandle = ipq_create_handle(0, PF_INET)) == NULL )
	{
		logCrit("Could not create ipq handle %s\n",ipq_errstr());
		return false;
	}

	if ( ipq_set_mode(m_IPQHandle, IPQ_COPY_PACKET, IPQ_PACKET_BUFSIZE) < 0 )
	{
		logCrit("Could not set ipq mode %s\n",ipq_errstr());
		return false;

	}
	logInfo("Initialised libipq\n");
#endif // HAVE_IPQ
	return true;
}

bool TrapSocket::Init_IPFW()
{
#ifdef HAVE_IPFW
    if ((m_DivertSocket = socket(PF_INET, SOCK_RAW, IPPROTO_DIVERT)) == -1) 
    {
        logCrit("Could not create divert socket for ipfw %s\n",strerror(errno));
        return false;
    }
    bzero(&m_DivertSin, sizeof(m_DivertSin));
	m_DivertSin.sin_port = htons(m_DivertPort);	// FIXME
    m_DivertSin.sin_family = PF_INET;
    m_DivertSin.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_DivertSocket, (struct sockaddr *)&m_DivertSin, sizeof(m_DivertSin)) == -1) 
    {
        logCrit("Could not bind divert socket %s\n",strerror(errno));
        return false;
    }
	logInfo("Bound divert socket on port %i\n",m_DivertPort);	//FIXME
	return true;
#else // HAVE_IPFW
	logCrit("IPFW not supported, check your plattform\n");
	return false;
#endif // HAVE_IPFW

}


bool TrapSocket::Init_PCAP()
{

#ifdef HAVE_PCAP
	char errbuf[PCAP_ERRBUF_SIZE];

	logInfo("Using pcap %s\n",pcap_lib_version());



	if ( (m_RawListener = pcap_open_live(m_PcapDevice.c_str(), 1500, 1, 50, errbuf)) == NULL )
	{
		logCrit("Could not open raw listener on device %s '%s'\n",m_PcapDevice.c_str(),errbuf);
		return false;
	}

	string bpf_filter_string = "tcp[tcpflags] & tcp-rst != 0 and tcp[4:4] = 0 ";

	pcap_if_t *alldevsp = NULL;
	
	if( pcap_findalldevs(&alldevsp,errbuf) == -1)
	{
		logCrit("pcap_findalldevs failed %s\n",errbuf);
		return false;

	}

	string bpf_filter_string_addition;

	for(pcap_if_t *alldev = alldevsp;alldev != NULL;alldev = alldev->next)
	{
		if (m_PcapDevice != "any" &&  alldev->name != m_PcapDevice)
			continue;

		if (alldev->name)
			logSpam("name %s\n",alldev->name);
		if (alldev->description)
			logSpam("\tdescription %s\n",alldev->description);

		logSpam("\tflags %i\n",alldev->flags);


		for (pcap_addr_t *addr = alldev->addresses; addr != NULL; addr = addr->next)
		{
			switch(addr->addr->sa_family)
			{
			case AF_INET:
				logSpam("\t\tAF_INET\n");
				if (addr->addr)
                	logSpam("\t\t\taddr %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->addr)->sin_addr)));
				if (addr->netmask)
					logSpam("\t\t\tnetmask %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->netmask)->sin_addr)));
				if (addr->broadaddr)
					logSpam("\t\t\tbcast %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->broadaddr)->sin_addr)));
				if (addr->dstaddr )
					logSpam("\t\t\tdstaddr %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->dstaddr)->sin_addr)));

				if ( bpf_filter_string_addition == "" )
				{
					bpf_filter_string_addition +=   string("src host ") + 
													string(inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->addr)->sin_addr))) + 
													string(" ");
				}
				else
				{
					bpf_filter_string_addition +=   string("or src host ") + 
													string(inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->addr)->sin_addr))) + 
													string(" ");
				}
				break;

			default:
				logSpam("\t\tAF_ not supported %i\n",addr->addr->sa_family);

			}
			logSpam("\n");

			
		}
	}

	pcap_freealldevs(alldevsp);

	if (bpf_filter_string_addition != "")
	{
		bpf_filter_string += "and ( " + bpf_filter_string_addition + ")";
	}

	struct bpf_program filter;

	logInfo("BPF Filter is %s\n",bpf_filter_string.c_str());

	if ( pcap_compile(m_RawListener, &filter,  (char *)bpf_filter_string.c_str(), 0, 0) == -1 )
	{
		logCrit("Invalid BPF string: %s.\n", pcap_geterr(m_RawListener));
		return false;
	}

	if ( pcap_setfilter(m_RawListener, &filter) == -1 )
	{
		logCrit("Unable to set BPF Filter: %s\n", pcap_geterr(m_RawListener));
		return false;
	}

	if ( pcap_setnonblock(m_RawListener, 1, errbuf) == -1 )
	{
		logCrit("Could not set RawListener to nonblock: %s.\n", errbuf);
		return false;
	}

	int i;
	i = pcap_getnonblock(m_RawListener, errbuf);

	if ( i == -1 )
	{
		logCrit("Error obtaining nonblock information from RawListener: %s\n",errbuf);
		return false;
	}
	else
	{
		logInfo("RawListener NonBlockingMode is %i\n",i);
	}

	m_PcapDataLinkType = pcap_datalink(m_RawListener);

	switch ( m_PcapDataLinkType )
	{
	case DLT_NULL:
	case DLT_EN10MB:
	case DLT_PPP:
	case DLT_RAW:
	case DLT_PPP_ETHER:

#ifdef DLT_LINUX_SLL
	case DLT_LINUX_SLL:
#endif
		logInfo("DataLinkLayer %s %s\n",
				pcap_datalink_val_to_name(m_PcapDataLinkType),
				pcap_datalink_val_to_description(m_PcapDataLinkType));
		break;

	default:
		logCrit("DataLinkLayer  %s %s not supported\n",
				pcap_datalink_val_to_name(m_PcapDataLinkType),
				pcap_datalink_val_to_description(m_PcapDataLinkType));
		return false;
	}


	

	return true;
#else // HAVE_PCAP
	logCrit("pcap not supported, hit the docs\n");
	return false;
#endif // HAVE_PCAP
	
}

bool TrapSocket::Exit()
{

	bool retval = false;
	switch ( m_HTType )
	{
	case HT_PCAP:
		retval = Exit_PCAP();
		break;

	case HT_IPQ:
		retval = Exit_IPQ();
		break;

	case HT_IPFW:
		retval = Exit_IPFW();
		break;

	default:
		logCrit("Invalid mode for module-honeytrap\n");
	}
	return retval;
}



bool TrapSocket::Exit_PCAP()
{
#ifdef HAVE_PCAP
	if ( m_RawListener != NULL )
	{
//		int pcap_stats(pcap_t *p, struct pcap_stat *ps)
		struct pcap_stat ps;
		memset(&ps,0,sizeof(struct pcap_stat));
		if ( pcap_stats(m_RawListener, &ps) != 0 )
		{
			logWarn("Could not obtain statistics information from pcap RawListener %s\n",pcap_geterr(m_RawListener));
		}
		else
		{
			logInfo("RawListener Statistics\n"
					"\t%i packets received\n"
					"\t%i packets dropped\n"
					"\t%i packets dropped by kernel\n",
					ps.ps_recv,
					ps.ps_drop,
					ps.ps_ifdrop);
		}

		pcap_close(m_RawListener);
	}
#endif // HAVE_PCAP
	return true;
}



bool TrapSocket::Exit_IPQ()
{
#ifdef HAVE_IPQ
	if ( m_IPQHandle != NULL )
	{
		ipq_destroy_handle(m_IPQHandle);
	}
#endif // HAVE_IPQ
	return true;
}



bool TrapSocket::Exit_IPFW()
{
#ifdef HAVE_IPFW
	if (m_DivertSocket != -1)
	{
		close(m_DivertSocket);
	}
#endif // HAVE_IPFW
	return true;
}



bool TrapSocket::wantSend()
{
	return false;
}


int32_t TrapSocket::doSend()
{
	return 1;
}



int32_t TrapSocket::doRecv()
{

	int retval = 1;
	switch ( m_HTType )
	{
	case HT_PCAP:
		retval = doRecv_PCAP();
		break;

	case HT_IPQ:
		retval = doRecv_IPQ();
		break;

	case HT_IPFW:
		retval = doRecv_IPFW();
		break;


	default:
		logCrit("Invalid mode for module-honeytrap\n");
	}
	return retval;
}



int32_t TrapSocket::doRecv_PCAP()
{

	logPF();
	// int pcap_next_ex(pcap_t *p, struct pcap_pkthdr **pkt_header, const u_char **pkt_data)
#ifdef HAVE_PCAP
	struct pcap_pkthdr *pkt_header;
	const u_char *pkt_data;
	int retval;

	retval = pcap_next_ex(m_RawListener,&pkt_header, &pkt_data);

	if ( retval == 1 )
	{
//		g_Nepenthes->getUtilities()->hexdump((byte *)pkt_data,52);

		int offset=0;

		switch ( m_PcapDataLinkType )
		{

		case DLT_NULL:
			offset = 4;
			break;

		case DLT_EN10MB:
			offset = 14;
			break;


		case DLT_PPP:
			/*	PPP; if the first 2 bytes are 0xff and 0x03, 
			* it's PPP in HDLC-like framing, with the PPP header following  those  two  bytes,  
			* otherwise it's PPP without framing, and the packet begins with the PPP header.
			*/              
			offset = 4;
			static char hldc_frame[] = { 0xff, 0x03 };
			if (memcmp(pkt_data,hldc_frame,2) == 0)
				offset += 2;
			break;


		case DLT_RAW:
			offset = 0;
			break;


		case DLT_PPP_ETHER:
			offset = 6;
			break;

#ifdef DLT_LINUX_SLL
		case DLT_LINUX_SLL:
			offset = 16;
			break;
#endif
		}

		struct libnet_ipv4_hdr *ip = (struct libnet_ipv4_hdr *) (pkt_data + offset);
		struct libnet_tcp_hdr *tcp = (struct libnet_tcp_hdr *) (pkt_data + offset + ip->ip_hl * 4);

		/* new connections are welcome */
		if ( ntohl(tcp->th_seq) != 0 )
			return 0;

		logInfo("Got RST packet from localhost:%i %i\n",ntohs(tcp->th_sport),tcp->th_sport);
		createListener(ip,tcp,(unsigned char *)pkt_data + offset,ip->ip_len);


	}//else
	// 0: // packets are being read from a live capture, and the timeout expired
	// -1: // an error occurred while reading the packet
	// -2: // packets are being read from a ``savefile'', and there are no more packets to read from the savefile.

#endif	// HAVE_PCAP
	return 1;
}




int32_t TrapSocket::doRecv_IPQ()
{
	logPF();
#ifdef HAVE_IPQ
	unsigned char buf[IPQ_PACKET_BUFSIZE];

	int status = ipq_read(m_IPQHandle, buf, IPQ_PACKET_BUFSIZE, 0);
	if ( status < 0 )
		return 1;

	switch ( ipq_message_type(buf) )
	{
	case NLMSG_ERROR:
		logWarn("IPQ Received error message %d\n",ipq_get_msgerr(buf));
		break;

	case IPQM_PACKET: 
		{
			ipq_packet_msg_t *m = ipq_get_packet(buf);

			if ( ntohs(m->hw_protocol) == ETHERTYPE_IP )
			{
				const struct libnet_ipv4_hdr* ip;

				ip = (struct libnet_ipv4_hdr*)(m->payload);

				int hlen = ip->ip_hl * 4;

				const struct libnet_tcp_hdr* tcp;
				tcp = (struct libnet_tcp_hdr*) ((u_char *)m->payload+hlen);

				if ( tcp->th_flags & TH_SYN && !(tcp->th_flags & TH_ACK) )
				{

					createListener((libnet_ipv4_hdr *)ip,(libnet_tcp_hdr *)tcp, 
								   (unsigned char *)m->payload,(uint16_t)m->data_len);

				}
			}

			status = ipq_set_verdict(m_IPQHandle, m->packet_id,NF_ACCEPT, 0, NULL);
			if ( status < 0 )
				logWarn("ipq_set_verdict failed %s\n",ipq_errstr());
		}
		break;


	default:
		fprintf(stderr, "Unknown message type!\n");
		break;
	}
#endif // HAVE_IPQ
	return 1;
}

int32_t TrapSocket::doRecv_IPFW()
{
	logPF();
#ifdef HAVE_IPFW
	int len;
	char buf[2024];

//	bzero(&m_DivertSin,sizeof(struct sockaddr_in));
	m_DivertSinLen = sizeof(m_DivertSin);
	if ( (len = recvfrom(m_DivertSocket, buf, sizeof(buf), 0,(struct sockaddr *)&m_DivertSin, &m_DivertSinLen)) == -1 )
	{
		logWarn("recvfrom() on divert socket failed %s\n",strerror(errno));
		return 1;
	}


	const struct libnet_ipv4_hdr* ip = (struct libnet_ipv4_hdr*)buf;
	int hlen = ip->ip_hl * 4;
	const struct libnet_tcp_hdr* tcp = (struct libnet_tcp_hdr*) ((u_char *)buf+hlen);

	if (1 && ( tcp->th_flags & TH_SYN && !(tcp->th_flags & TH_ACK) ) ) // isPortListening(ntohs(tcp->th_dport),*(uint32_t *)&(ip->ip_dst)) == false )
	/*
	 * FreeBSD got no /proc/net/tcp and the code to retrieve the data from the kvm or sys*whatever* is pretty cruel
	 * http://cvsup.pt.freebsd.org/cgi-bin/cvsweb/cvsweb.cgi/src/usr.bin/systat/netstat.c?rev=1.25&content-type=text/x-cvsweb-markup
	 * I hope to replace this someday with a real check, to avoid all the crit warnings when trying to bind a already bound port
	 */
	{

		createListener((libnet_ipv4_hdr *)ip,(libnet_tcp_hdr *)tcp,(unsigned char *)buf,len);
	}


	if ( sendto(m_DivertSocket, buf, len, 0,(struct sockaddr *)&m_DivertSin, m_DivertSinLen) == -1 )
	{
		logWarn("Writing packet back to divert socket failed %s\n",strerror(errno));
	}


#endif
	return 1;
}


int32_t TrapSocket::getSocket()
{
	switch ( m_HTType )
	{
	case HT_PCAP:
#ifdef HAVE_PCAP
		return pcap_get_selectable_fd(m_RawListener);
#endif
		break;

	case HT_IPQ:
#ifdef HAVE_IPQ
		return m_IPQHandle->fd;
#endif

	case HT_IPFW:
#ifdef HAVE_IPFW
		return m_DivertSocket;
#endif
		break;


	default:
		logCrit("Invalid mode for module-honeytrap\n");
	}
	return -1;
}

int32_t TrapSocket::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
{
#if defined(linux) || defined(__linux)	
	return getsockopt(getSocket(), level, optname, optval, optlen);
#else
	return 0;	
#endif
}


void TrapSocket::printIPpacket(unsigned char *buf, uint32_t len)
{
	const struct libnet_ipv4_hdr* ip;

	ip = (struct libnet_ipv4_hdr*)(buf);

	int hlen = ip->ip_hl * 4;

	const struct libnet_tcp_hdr* tcp;
	tcp = (struct libnet_tcp_hdr*) ((u_char *)buf+hlen);


	logSpam("-- IP v%d, ID = %d, Header Length = %d, Total Length = %d\n",
			ip->ip_v,
			ip->ip_id,
			ip->ip_hl * 4,
			ntohs(ip->ip_len) );

	logSpam("  |- Source       %s \n" , 
			inet_ntoa(ip->ip_src) );

	logSpam("  |- Destionation %s \n" , 
			inet_ntoa(ip->ip_dst) );

	logSpam("  |- Bits: %s %s, Offset : %d, checksum = %.4x, TTL = %d\n", 
			ntohs(ip->ip_off) & IP_DF? "DF":"", 
			ntohs(ip->ip_off) & IP_MF? "MF":"", 
			ntohs(ip->ip_off) & IP_OFFMASK, 
			ntohs(ip->ip_sum),
			ip->ip_ttl);   

	logSpam("  |- proto = %d : \n", 
			ip->ip_p  );


	logSpam("  `-- TCP, Header Length = %d Payload Length = %d\n",
			tcp->th_off *4,
			len); // <- this number is wrong

	logSpam("     |- port Source = %d --> port Destination = %d\n",
			ntohs(tcp->th_sport),
			ntohs(tcp->th_dport));

	logSpam("     |- Seq nb = %.4x ,Acknowledgement nb:%.4x\n",
			ntohs(tcp->th_seq),
			ntohs(tcp->th_ack));

	logSpam("     |- bits %s %s %s %s %s %s %s %s\n",
			(tcp->th_flags) & TH_FIN?"FIN":"", 
			(tcp->th_flags) & TH_SYN?"SYN":"",
			(tcp->th_flags) & TH_RST?"RST":"",
			(tcp->th_flags) & TH_PUSH?"PUSH":"",
			(tcp->th_flags) & TH_ACK?"ACK":"",
			(tcp->th_flags) & TH_URG?"URG":"",
			(tcp->th_flags) & TH_ECE?"ECE":"",
			(tcp->th_flags) & TH_CWR?"CWR":""
		   );

	logSpam("     `- checksum = %.4x, windows = %.4x, urgent = %.4x\n", 
			ntohs(tcp->th_sum),
			ntohs(tcp->th_win), 
			ntohs(tcp->th_urp) );
}

bool TrapSocket::createListener(libnet_ipv4_hdr *ip, libnet_tcp_hdr *tcp, unsigned char *data, uint16_t size)
{
	printIPpacket(data,size);

	uint16_t port;

	if ( tcp->th_flags & TH_SYN && !(tcp->th_flags & TH_ACK) )
		port = ntohs(tcp->th_dport); // inline mode
	else
		port = ntohs(tcp->th_sport); // pcap mode

	if (1)// isPortListening(ntohs(tcp->th_dport),*(uint32_t *)&(ip->ip_dst)) == false )
	{
		logInfo("Connection to unbound port %i requested, binding port\n",port);

		Socket *sock = g_Nepenthes->getSocketMgr()->bindTCPSocket(INADDR_ANY,port,60,60);
		if ( sock != NULL && (sock->getDialogst()->size() == 0 && sock->getFactories()->size() == 0) )
		{

			DialogueFactory *diaf;
			if ( (diaf = g_Nepenthes->getFactoryMgr()->getFactory((char *)m_DialogueFactory.c_str())) == NULL )
			{
				logCrit("No %s availible \n",m_DialogueFactory.c_str());
				return false;
			}

			sock->addDialogueFactory(diaf);
		}
#ifdef HAVE_PCAP
		if ( g_ModuleHoneytrap->getPcapDumpFiles() &&  m_HTType != HT_PCAP )
		{
			if ( g_ModuleHoneytrap->socketExists((uint32_t)ip->ip_src.s_addr, ntohs(tcp->th_sport),
												 (uint32_t)ip->ip_dst.s_addr,ntohs(tcp->th_dport)) == false )
			{
				POLLSocket *ps = new PCAPSocket((uint32_t)ip->ip_src.s_addr, ntohs(tcp->th_sport) ,
												(uint32_t)ip->ip_dst.s_addr,ntohs(tcp->th_dport));
				if ( ps->Init() == true )
				{
					g_Nepenthes->getSocketMgr()->addPOLLSocket(ps);
					g_ModuleHoneytrap->socketAdd((uint32_t)ip->ip_src.s_addr, ntohs(tcp->th_sport),
												 (uint32_t)ip->ip_dst.s_addr,ntohs(tcp->th_dport),
												 ps);
				}
			} else
			{
				logWarn("Already listening for this buddy\n");
			}
		}
#endif // HAVE_PCAP
	}
	return true;
}


string TrapSocket::getSupportedModes()
{
	string isupport = "";

#ifdef HAVE_PCAP
	isupport += "pcap,";
#endif 

#ifdef HAVE_IPQ
	isupport += "ipq,";
#endif

#ifdef HAVE_IPFW
	isupport += "ipfw";
#endif

	return isupport;
}

/*
*/

