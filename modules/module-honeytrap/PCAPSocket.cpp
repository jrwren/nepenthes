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
#include <sys/socket.h>
#include <arpa/inet.h>

#include "LogManager.hpp"
#include "PCAPSocket.hpp"
#include "module-honeytrap.hpp"

using namespace nepenthes;



PCAPSocket::PCAPSocket(uint32_t remotehost, uint16_t remoteport, uint32_t localhost, uint16_t localport)
{
	logPF();
	setRemoteHost(remotehost);
	setRemotePort(remoteport);
	setLocalHost(localhost);
	setLocalPort(localport);
	m_Polled = false;

	m_NetworkDevice = "";
	m_Type = ST_POLL|ST_CONNECT;

	m_PacketCount = 0;
}

PCAPSocket::~PCAPSocket()
{
	logPF();
	g_ModuleHoneytrap->socketDel(this);
}


bool PCAPSocket::Init()
{
	logPF();
	bpf_u_int32 mask;
	bpf_u_int32 net;
	char errbuf[PCAP_ERRBUF_SIZE];

	
	/* find out which device has the 'local' ip*/
	pcap_if_t *alldevsp = NULL;

	if( pcap_findalldevs(&alldevsp,errbuf) == -1)
	{
		logCrit("pcap_findalldevs failed %s\n",errbuf);
		return false;

	}

	for ( pcap_if_t *alldev = alldevsp;alldev != NULL;alldev = alldev->next )
	{
		for ( pcap_addr_t *addr = alldev->addresses; addr != NULL; addr = addr->next )
		{
			if ( addr->addr )
			{
			
				switch ( addr->addr->sa_family )
				{
				case AF_INET:
					if ( *(uint32_t *)&(((struct sockaddr_in *)addr->addr)->sin_addr) == m_LocalHost )
					{
						if ( alldev->name )
						{
                        	logSpam("name %s\n",alldev->name);
							m_NetworkDevice = alldev->name;
						}

						if ( alldev->description )
							logSpam("\tdescription %s\n",alldev->description);


						logSpam("\t\t\taddr %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->addr)->sin_addr)));
						if ( addr->netmask )
							logSpam("\t\t\tnetmask %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->netmask)->sin_addr)));
						if ( addr->broadaddr )
							logSpam("\t\t\tbcast %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->broadaddr)->sin_addr)));
						if ( addr->dstaddr )
							logSpam("\t\t\tdstaddr %s\n",inet_ntoa(*(struct in_addr*) &(((struct sockaddr_in *)addr->dstaddr)->sin_addr)));
					}
					break;

				default:
					break;
				}
			}
		}
	}

	pcap_freealldevs(alldevsp);


	if (m_NetworkDevice == "")
	{
		logCrit("Could not find interface for ip %s\n",inet_ntoa(*(struct in_addr*) &m_LocalHost));
		return false;
	}
	logInfo("Using Interface %s for ip %s\n",m_NetworkDevice.c_str(),inet_ntoa(*(struct in_addr*) &m_LocalHost));


	/* lookup the netmask */
	if ( pcap_lookupnet(m_NetworkDevice.c_str(), &net, &mask, errbuf) == -1 )
	{
		logCrit("Couldn't get netmask for device %s: %s\n", m_NetworkDevice.c_str(), errbuf);
		return false;
	}


	/* open the sniffer */
	if ( (m_PcapSniffer = pcap_open_live(m_NetworkDevice.c_str(), 2048, 0, 10, errbuf)) == NULL )
	{
		logCrit("Could not create pcap listener '%s'\n",errbuf);
		return false;
	}

	/* create the bpf filter */
	string rhost = inet_ntoa(*(in_addr *)&m_RemoteHost);
	string lhost = inet_ntoa(*(in_addr *)&m_LocalHost);

	char *bpffilter;
	asprintf(&bpffilter,
			 "(src host %s and src port %i and dst host %s and dst port %i)"
			 " or "
			 "(src host %s and src port %i and dst host %s and dst port %i)",
			 rhost.c_str(),getRemotePort(),
			 lhost.c_str(),getLocalPort(),
			 lhost.c_str(),getLocalPort(),
			 rhost.c_str(),getRemotePort());

	logDebug("connection logger bpf is '%s'.\n",bpffilter);

	/* compile the bpf filter */
	struct bpf_program filter;
	if ( pcap_compile(m_PcapSniffer, &filter, bpffilter, 0, net) == -1 )
	{
		logCrit("Pcap error - Invalid BPF string: %s.\n", pcap_geterr(m_PcapSniffer));
		free(bpffilter);
		return false;
	}


	/* apply the bpf filter */
	if ( pcap_setfilter(m_PcapSniffer, &filter) == -1 )
	{
		logCrit("Pcap error - Unable to start tcp sniffer: %s\n", errbuf);
		free(bpffilter);
		return false;
	}

	/* create the path for logging */
	char *pcap_file_path;

	asprintf(&pcap_file_path,"var/log/nepenthes/pcap/%i_%s-%i_%s-%i.pcap",
			 (int)time(NULL),
			 rhost.c_str(),getRemotePort(),
			 lhost.c_str(),getLocalPort()
			 );



	/* create the buddy who will write the pcap file, the pcap_dumper_t */
	if ( (m_PcapDumper = pcap_dump_open(m_PcapSniffer,pcap_file_path)) == NULL )
	{
		logCrit("Pcap error - Could not create pcap dumpfile %s\n", pcap_geterr(m_PcapSniffer));
		free(bpffilter);
		free(pcap_file_path);
		return false;
	}

	/* the the socket async */
	if ( pcap_setnonblock(m_PcapSniffer, 1, errbuf) == -1 )
	{
		logCrit("Pcap error - Could not set fd nonblocking %s\n", errbuf);
		free(bpffilter);
		free(pcap_file_path);
		return false;
	}


	/* clean up*/
	free(bpffilter);
	free(pcap_file_path);

	m_LastAction = time(NULL);
	m_TimeoutIntervall = 10;

	return true;
}


bool PCAPSocket::Exit()
{
	logDebug("connectionlogger logged %i packets\n", m_PacketCount);
	pcap_dump_close(m_PcapDumper);
	pcap_close(m_PcapSniffer);
	setStatus(SS_CLOSED);
	return true;
}


bool PCAPSocket::wantSend()
{
	return false;
}


int32_t PCAPSocket::doSend()
{
	return 0;
}


int32_t PCAPSocket::doRecv()
{
	struct pcap_pkthdr *pkt_header;
	const u_char *pkt_data;

	/* receive the packet */
	if ( pcap_next_ex(m_PcapSniffer,&pkt_header, &pkt_data) == 1 )
	{
		/* dump the packet */
		pcap_dump((u_char *)m_PcapDumper,pkt_header,pkt_data);
		m_PacketCount++;
	}
	return 1;
}


int32_t PCAPSocket::getSocket()
{
	return pcap_get_selectable_fd(m_PcapSniffer);
}


int32_t PCAPSocket::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
{
#if defined(linux) || defined(__linux)	
	return getsockopt(getSocket(), level, optname, optval, optlen);
#else
	return 0;	
#endif
}

bool PCAPSocket::checkTimeout()
{
	if ( m_TimeoutIntervall > 0 )
	{
		if ( time(NULL) - m_LastAction > m_TimeoutIntervall )
		{
			setStatus(SS_TIMEOUT);
			return false;
		}
	}
	return true;
}


void PCAPSocket::active()
{
	logPF();
	m_TimeoutIntervall = 0;
}

void PCAPSocket::dead()
{
	logPF();
	Exit();
}

