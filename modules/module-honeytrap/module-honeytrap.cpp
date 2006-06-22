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

#include <ctype.h>
#include <netinet/in.h>

#include "module-honeytrap.hpp"

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


/*
 * This module is derived from honeytrap (honeytrap.sf.net) by Werner Tillmann
 *
 * The idea is really good, and it as it was easy to make a nepenthes module of it, we stole it
 *
 */ 




/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;

/**
 * The Constructor
 * creates a new ModuleHoneyTrap Module, 
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
ModuleHoneyTrap::ModuleHoneyTrap(Nepenthes *nepenthes)
{
	m_ModuleName        = "module-honeytrap";
	m_ModuleDescription = "open closed ports to accept connections - idea from http://honeytrap.sf.net ";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	g_Nepenthes = nepenthes;

#ifdef HAVE_PCAP
	m_RawListener = NULL;
#endif

#ifdef HAVE_IPQ
	m_IPQHandle = NULL;
#endif 

	m_HTType = HT_IPQ;
}

ModuleHoneyTrap::~ModuleHoneyTrap()
{

}


/**
 * Module::Init()
 * 
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool ModuleHoneyTrap::Init()
{
	string isupport = "";

#ifdef HAVE_PCAP
	isupport += "pcap,";
#endif

#ifdef HAVE_IPQ
	isupport += "ipq";
#endif

	logInfo("ModuleHoneyTrap compiled with support for %s\n",isupport.c_str());

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	string mode;
	try
	{

		mode = m_Config->getValString("module-honeytrap.listen_mode");
	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	if (mode == "pcap")
	{
		m_HTType = HT_PCAP;
		
	}else
	if (mode == "ipq")
	{
		m_HTType = HT_IPQ;
	}else
	{
		logCrit("Invalid mode %s for module-honeytrap\n",mode.c_str());
		return false;
	}

	logInfo("ModuleHoneyTrap mode %s\n",mode.c_str());

	bool retval = false;
	switch ( m_HTType )
	{
	case HT_PCAP:
		retval = Init_PCAP();
		break;

	case HT_IPQ:
		retval = Init_IPQ();
		break;
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

bool ModuleHoneyTrap::Init_IPQ()
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

bool ModuleHoneyTrap::Init_PCAP()
{

#ifdef HAVE_PCAP
	char errbuf[PCAP_ERRBUF_SIZE];

	logInfo("Using pcap %s\n",pcap_lib_version());

	if ( (m_RawListener = pcap_open_live("any", 1500, 1, 0, errbuf)) == NULL )
	{
		logCrit("Could not open raw listener '%s'\n",errbuf);
		return false;
	}



	struct bpf_program filter;

	// int pcap_compile(pcap_t *p, struct bpf_program *fp, char *str, int optimize, bpf_u_int32 netmask)
	if ( pcap_compile(m_RawListener, &filter,  "tcp[tcpflags] & tcp-rst != 0", 0, 0) == -1 )
	//	if ( pcap_compile(m_RawListener, &filter,  "host 192.168.53.20", 0, 0) == -1 )
	{
		logCrit("Invalid BPF string: %s.\n", pcap_geterr(m_RawListener));
		return false;
	}

	if ( pcap_setfilter(m_RawListener, &filter) == -1 )
	{
		logCrit("Unable to set BPF Filter: %s\n", pcap_geterr(m_RawListener));
		return false;
	}

	// int pcap_setnonblock(pcap_t *p, int nonblock, char *errbuf);
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

#endif // HAVE_PCAP
	return true;
}

bool ModuleHoneyTrap::Exit()
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
	}
	return retval;
}

bool ModuleHoneyTrap::Exit_PCAP()
{
#ifdef HAVE_PCAP
	if ( m_RawListener != NULL )
	{
//		int pcap_stats(pcap_t *p, struct pcap_stat *ps)
		struct pcap_stat ps;
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

bool ModuleHoneyTrap::Exit_IPQ()
{
#ifdef HAVE_IPQ
	if ( m_IPQHandle != NULL )
	{
		ipq_destroy_handle(m_IPQHandle);
	}
#endif // HAVE_IPQ
	return true;
}




bool ModuleHoneyTrap::wantSend()
{

	return false;
}

int32_t ModuleHoneyTrap::doSend()
{

	return 1;
}

int32_t ModuleHoneyTrap::doRecv()
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
	}
	return retval;
}

int32_t ModuleHoneyTrap::doRecv_PCAP()
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

		struct ip_header *ip = (struct ip_header *) (pkt_data + ETHER_HDRLEN);
		struct tcp_header *tcp = (struct tcp_header *) (pkt_data + ETHER_HDRLEN + ip->ip_hl * 4);

		/* new connections are welcome */
		if ( ntohl(tcp->th_seqno) != 0 )
			return 0;
		logInfo("Got RST packet from localhost:%i %i\n",ntohs(tcp->th_sport),tcp->th_sport);

		Socket *sock = g_Nepenthes->getSocketMgr()->bindTCPSocket(INADDR_ANY,ntohs(tcp->th_sport),600,60);
		if ( sock != NULL )
		{

			DialogueFactory *diaf;
			if ( (diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL )
			{
				logCrit("No WinNTShell DialogueFactory availible \n");
				return 1;
			}

			sock->addDialogueFactory(diaf);
		}

	}//else
	// 0: // packets are being read from a live capture, and the timeout expired
	// -1: // an error occurred while reading the packet
	// -2: // packets are being read from a ``savefile'', and there are no more packets to read from the savefile.

#endif	// HAVE_PCAP
	return 1;
}

int32_t ModuleHoneyTrap::doRecv_IPQ()
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


					logSpam("-- IP v%d, ID = %d, Header Length = %d, Total Length = %d\n",
							ip->ip_v,
							ip->ip_id,
							ip->ip_hl * 4,
							ntohs(ip->ip_len) );

					logSpam("  | %s --> " , 
							inet_ntoa(ip->ip_src) );

					logSpam("%s \n" , 
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
							m->data_len); // <- this number is wrong

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


					if ( isPortListening(ntohs(tcp->th_dport),*(uint32_t *)&(ip->ip_dst)) == false )
					{
						logInfo("Connection to unbound port %i requested, binding port\n",ntohs(tcp->th_dport));

						Socket *sock = g_Nepenthes->getSocketMgr()->bindTCPSocket(INADDR_ANY,ntohs(tcp->th_dport),60,60);
						if ( sock != NULL )
						{

							DialogueFactory *diaf;
							if ( (diaf = g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")) == NULL )
							{
								logCrit("No WinNTShell DialogueFactory availible \n");
								return 1;
							}

							sock->addDialogueFactory(diaf);
						}
					}
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

int32_t ModuleHoneyTrap::getSocket()
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
		break;
	}
	return -1;
}

int32_t ModuleHoneyTrap::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
{
	return getsockopt(getSocket(), level, optname, optval, optlen);
}


bool ModuleHoneyTrap::isPortListening(uint16_t localport, uint32_t localhost)
{
	logSpam("looking for %s:%i\n",inet_ntoa(*(struct in_addr *)&localhost),localport);
	unsigned long rxq, txq, time_len, retr, inode;
	int num, local_port, rem_port, d, state, uid, timer_run, timeout;
	char rem_addr[128], local_addr[128], more[512];
	char line[512];
	struct sockaddr_in localaddr; //, remaddr;

	/*
	 * parsing logic basically taken from netstat.c tcp_do_one() in net-tools 
	 *
	 */

	FILE *fp;
	if ( (fp = fopen("/proc/net/tcp", "r")) == NULL )
	{
		logCrit("Could not open /proc/net/tcp \n");
		return true;
	}


	bool port_is_listening = false;
	if ( fgets(line, sizeof(line), fp) != (char *) NULL )
	{
		for ( ; fgets(line, sizeof(line), fp) && port_is_listening == false; )
		{

			num = sscanf(line,
						 "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %ld %512s\n",
						 &d, local_addr, &local_port, rem_addr, &rem_port, &state,
						 &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode, more);

			sscanf(local_addr, "%X",&((struct sockaddr_in *) &localaddr)->sin_addr.s_addr);

			if ( state != TCP_LISTEN )
				continue;


			logSpam("Listening on port %i localport %i\n",local_port,localport);


			if ( local_port != localport )
				continue;

			uint32_t any_addr = 0;
			if ( memcmp(&((struct sockaddr_in *) &localaddr)->sin_addr.s_addr,&any_addr,4) == 0 ||
				 memcmp(&((struct sockaddr_in *) &localaddr)->sin_addr.s_addr,&localhost,4) == 0 )
			{
				logSpam("port already bound on ip %s\n", 
						inet_ntoa( (struct in_addr) (((struct sockaddr_in *)&localaddr)->sin_addr))
					   );
				port_is_listening = true;
			}
		}

	}

	fclose(fp);
	return port_is_listening;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new ModuleHoneyTrap(nepenthes);
		return 1;
	}
	else
	{
		return 0;
	}
}

