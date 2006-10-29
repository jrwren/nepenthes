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
 */

#include "TapInterface.hpp"
#include "SocketManager.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "Buffer.hpp"
#include "Message.hpp"
#include "ShellcodeManager.hpp"
#include "Config.hpp"
#include "Download.hpp"

#include "Socket.cpp"
#include "POLLSocket.cpp"

#include "module-peiros.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#if defined(__linux__)
#include <net/route.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#endif

#include <string.h>
#include <errno.h>

TapInterface::TapInterface() : POLLSocket()
{
//	logPF();
	m_encapsulator = 0;
}

bool TapInterface::Init(uint32_t netmask, bool manageRoute)
{
#if defined(__linux_)
	logPF();
    struct ifreq ifr;
    int fd, ret;
    
    fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0) {
        logCrit("Could not open /dev/net/tun for module-peiros!\n");
        return false;
    }
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name, "tun%d", IFNAMSIZ);
    ret = ioctl(fd, TUNSETIFF, (void *) &ifr);
    if (ret != 0) {
        logCrit("Could not configure /dev/net/tun for module-peiros!\n");
        close(fd);
        return false;
    }
    
    m_devname = ifr.ifr_name;
    
    int ctlsocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(ctlsocket < 0)
    	return false;
    	
    memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, m_devname.c_str(), IFNAMSIZ);
	ifr.ifr_flags = IFF_UP | IFF_NOARP | IFF_DYNAMIC;
	
	if(ioctl(ctlsocket, SIOCSIFFLAGS, &ifr) < 0)
		return false;
		
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, m_devname.c_str(), IFNAMSIZ);
	
	if(ioctl(ctlsocket, SIOCGIFHWADDR, &ifr) < 0)
		return false;
		
	m_devname = ifr.ifr_name;
		
	memcpy(m_hwaddr, ifr.ifr_hwaddr.sa_data, sizeof(m_hwaddr));
		
	m_netmask = netmask;
    
    fcntl(fd, F_SETFL, O_NONBLOCK);
    close(ctlsocket);
    
    m_Socket = m_fd = fd;
    m_Polled = true;
    
    g_Nepenthes->getSocketMgr()->addPOLLSocket(this);
        
	if((m_manageRoute = manageRoute))
	{
		int sockfd;
		struct rtentry rt = { 0 };
		
		((struct sockaddr_in *) &rt.rt_dst)->sin_family = AF_INET;		
		((struct sockaddr_in *) &rt.rt_genmask)->sin_family = AF_INET;
		
		logInfo("Adding route for \"%s\"!\n", ifr.ifr_name);

		rt.rt_dev = (char *) m_devname.c_str();
		rt.rt_flags = RTF_UP;
		
		
		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			logCrit("Could not create socket for peiros default route ioctl: %s!\n", strerror(errno));
			return false;
		}
		
		if(ioctl(sockfd, SIOCADDRT, &rt) < 0)
		{
			logCrit("Could not add peiros default route: %s!\n", strerror(errno));
			//return false;
		}
		
		close(sockfd);
	}
	
	return true;
#else
	logCrit("this module does not work on your operating system, use linux\n");
	return false;

#endif
    
}

int32_t TapInterface::getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen)
{
    return 0;
}

int TapInterface::getSocket()
{
	return m_fd;
}

bool TapInterface::Exit()
{
#if defined(__linux_)
	if(m_manageRoute)
	{

		int sockfd;
		struct rtentry rt = { 0 };
		
		((struct sockaddr_in *) &rt.rt_dst)->sin_family = AF_INET;
		((struct sockaddr_in *) &rt.rt_dst)->sin_addr.s_addr = 0;
		
		((struct sockaddr_in *) &rt.rt_genmask)->sin_family = AF_INET;
		((struct sockaddr_in *) &rt.rt_genmask)->sin_addr.s_addr = 0;

		rt.rt_dev = (char *) m_devname.c_str();
		rt.rt_flags = RTF_UP;
		
		
		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			logCrit("Could not create socket for peiros default route removal ioctl: %s!\n", strerror(errno));
			return false;
		}
		
		if(ioctl(sockfd, SIOCDELRT, &rt) < 0)
		{
			logCrit("Could not remove peiros default route: %s!\n", strerror(errno));
			return false;
		}
		
		close(sockfd);

	}

	logPF();
	close(m_fd);
#endif
	return true;
}

bool TapInterface::wantSend()
{
	return false;
}

int32_t TapInterface::doSend()
{
	return 0;
}

int32_t TapInterface::doRecv()
{
	logPF();
	static char buffer[2048];
	ssize_t len = read(m_fd, buffer, sizeof(buffer));
	
	if(len <= 0)
	{
		logWarn("len <= 0: %i\n", len);
		return len;
	}
	
	if(m_encapsulator)
		m_encapsulator->encapsulatePacket(buffer, len);
	else
		logWarn("Lost %i bytes due to absence of encapsulator!\n", len);
		
	return (int32_t) len;
}

int32_t TapInterface::doWrite(char * buf, uint32_t len)
{	
	logPF();
	return write(m_fd, buf, len);
}

void TapInterface::setEncapsulator(TapEncapsulator * e)
{
	logPF();
	m_encapsulator = e;
}

bool TapInterface::addAddress(uint32_t address)
{
#if defined(__linux__)
	logPF();
	int ctlsocket;
	struct ifreq ifr;
	struct sockaddr_in addr;
	
	{
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = address;
		addr.sin_port = 0;
		
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, m_devname.c_str(), IFNAMSIZ);
		memcpy(&ifr.ifr_addr, &addr, sizeof(addr));
		
		if(ioctl((ctlsocket = socket(AF_INET, SOCK_STREAM, 0)), SIOCSIFADDR, &ifr) < 0)
		{
			logWarn("Failed to set address %s: %s\n", inet_ntoa(addr.sin_addr), strerror(errno));
        	return false;
		}
	}
		
	{
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = m_netmask;
		addr.sin_port = 0;
		
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, m_devname.c_str(), IFNAMSIZ);
		memcpy(&ifr.ifr_addr, &addr, sizeof(addr));
		
		if(ioctl(ctlsocket, SIOCSIFNETMASK, &ifr) < 0)
		{
			logWarn("Failed to set netmask %s: %s\n", inet_ntoa(addr.sin_addr), strerror(errno));
			return false;
		}
   	}
   	
   	logInfo("Added address %s.\n", inet_ntoa(addr.sin_addr));
   	
	close(ctlsocket);
#endif			
	return true;
}

void TapInterface::removeAddress(uint32_t address)
{
	logPF();
	// TODO: implement
}
