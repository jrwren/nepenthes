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

#include <string>

#include "DownloadManager.hpp"
#include "DownloadHandler.hpp"
#include "Nepenthes.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"

#include "LogManager.hpp"
#include "Config.hpp"

#include "SubmitEvent.hpp"
#include "EventManager.hpp"

using namespace std;
using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_mgr

#ifdef WIN32
#define uint32_t uint32_t
#endif


DownloadManager::DownloadManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}

DownloadManager::~DownloadManager()
{
	logPF();
	while(m_DownloadHandlers.size() > 0)
	{
        m_DownloadHandlers.pop_front();
	}
}

bool  DownloadManager::Init()
{
	logPF();
	m_ReplaceLocalIps = false;
	try
	{
		if ( m_Nepenthes->getConfig()->getValInt("nepenthes.downloadmanager.replace_local_ips")==1 )
			m_ReplaceLocalIps = true;
	} catch ( ... ) {
		logCrit("Could not find %s in config file\n","moduledir, moduleconfigdir");
		return false;
	}
	return true;
}

bool  DownloadManager::Exit()
{
    return true;
}

/*
 * these cool makros are taken from the clamav mailing list  
 * 
 */
#ifndef BIG_ENDIAN
	#define SWAP_ORDER(x) (x)
#else
	#define SWAP_ORDER(x) ( \
		((x & 0xff) << 24) | \
		((x & 0xff00) << 8) | \
		((x & 0xff0000) >> 8 ) | \
		((x & 0xff000000) >> 24 ))
#endif

#define PACKADDR(a, b, c, d) SWAP_ORDER((((uint32_t)(a) << 24) | ((b) << 16) | ((c) << 8) | (d)))
#define MAKEMASK(bits)	SWAP_ORDER(((uint32_t)(0xffffffff << (32-bits))))

ip_range_t DownloadManager::m_irLocalRanges[] =   
{
//		taken from http://www.rfc-editor.org/rfc/rfc3330.txt
//				ip address					netmask 			comment
	{ PACKADDR( 0   ,   0 ,   0 ,   0 ), MAKEMASK(  8 ) }, // "This" Network					[RFC1700, page 4]
	{ PACKADDR( 10  ,   0 ,   0 ,   0 ), MAKEMASK(  8 ) }, // Private-Use Networks				[RFC1918]
	{ PACKADDR( 14  ,   0 ,   0 ,   0 ), MAKEMASK(  8 ) }, // Public-Data Networks				[RFC1700, page 181]
//	{ PACKADDR( 24  ,   0 ,   0 ,   0 ), MAKEMASK(  8 ) }, // Cable Television Networks         
	{ PACKADDR( 39  ,   0 ,   0 ,   0 ), MAKEMASK(  8 ) }, // Reserved but subject to allocation[RFC1797]
	{ PACKADDR( 127 ,   0 ,   0 ,   0 ), MAKEMASK(  8 ) }, // Loopback							[RFC1700, page 5]
	{ PACKADDR( 128 ,   0 ,   0 ,   0 ), MAKEMASK( 16 ) }, // Reserved but subject to allocation
	{ PACKADDR( 169 , 254 ,   0 ,   0 ), MAKEMASK( 16 ) }, // Link Local                                   --
	{ PACKADDR( 172 ,  16 ,   0 ,   0 ), MAKEMASK( 12 ) }, // Private-Use Networks				[RFC1918]
	{ PACKADDR( 191 , 255 ,   0 ,   0 ), MAKEMASK( 16 ) }, // Reserved but subject to allocation
	{ PACKADDR( 192 ,   0 ,   0 ,   0 ), MAKEMASK( 24 ) }, // Reserved but subject to allocation
	{ PACKADDR( 192 ,   0 ,   2 ,   0 ), MAKEMASK( 24 ) }, // Test-Net
	{ PACKADDR( 192 ,  88 ,  99 ,   0 ), MAKEMASK( 24 ) }, // 6to4 Relay Anycast				[RFC3068]
	{ PACKADDR( 192 , 168 ,   0 ,   0 ), MAKEMASK( 16 ) }, // Private-Use Networks				[RFC1918]
	{ PACKADDR( 198 ,  18 ,   0 ,   0 ), MAKEMASK( 15 ) }, // Network Interconnect Device Benchmark Testing	[RFC2544]
	{ PACKADDR( 223 , 255 , 255 ,   0 ), MAKEMASK( 24 ) }, // Reserved but subject to allocation  
	{ PACKADDR( 224 ,   0 ,   0 ,   0 ), MAKEMASK(  4 ) }, // Multicast							[RFC3171]
	{ PACKADDR( 240 ,   0 ,   0 ,   0 ), MAKEMASK(  4 ) }, // Reserved for Future Use			[RFC1700, page 4]


/*	old and discusable
	{ PACKADDR( 0  , 0  , 0  , 0   ), MAKEMASK(  8 )}, //   0.000.000.000 / 255.000.000.000 
	{ PACKADDR( 1  , 0  , 0  , 0   ), MAKEMASK(  8 )}, //   1.000.000.000 / 255.000.000.000  
	{ PACKADDR( 2  , 0  , 0  , 0   ), MAKEMASK(  8 )}, //   2.000.000.000 / 255.000.000.000  
	{ PACKADDR( 5  , 0  , 0  , 0   ), MAKEMASK(  8 )}, //   5.000.000.000 / 255.000.000.000  
	{ PACKADDR( 7  , 0  , 0  , 0   ), MAKEMASK(  8 )}, //   7.000.000.000 / 255.000.000.000  
	{ PACKADDR( 10 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  10.000.000.000 / 255.000.000.000 
	{ PACKADDR( 23 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  23.000.000.000 / 255.000.000.000  
	{ PACKADDR( 27 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  27.000.000.000 / 255.000.000.000  
	{ PACKADDR( 31 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  31.000.000.000 / 255.000.000.000  
	{ PACKADDR( 36 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  36.000.000.000 / 255.000.000.000  
	{ PACKADDR( 37 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  37.000.000.000 / 255.000.000.000  
	{ PACKADDR( 39 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  39.000.000.000 / 255.000.000.000  
	{ PACKADDR( 41 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  41.000.000.000 / 255.000.000.000  
	{ PACKADDR( 42 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  42.000.000.000 / 255.000.000.000  
	{ PACKADDR( 49 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  49.000.000.000 / 255.000.000.000  
	{ PACKADDR( 50 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  50.000.000.000 / 255.000.000.000  
	{ PACKADDR( 73 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  73.000.000.000 / 255.000.000.000  
	{ PACKADDR( 74 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  74.000.000.000 / 255.000.000.000  
	{ PACKADDR( 75 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  75.000.000.000 / 255.000.000.000  
	{ PACKADDR( 76 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  76.000.000.000 / 255.000.000.000  
	{ PACKADDR( 77 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  77.000.000.000 / 255.000.000.000  
	{ PACKADDR( 78 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  78.000.000.000 / 255.000.000.000  
	{ PACKADDR( 79 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  79.000.000.000 / 255.000.000.000  
	{ PACKADDR( 89 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  89.000.000.000 / 255.000.000.000  
	{ PACKADDR( 90 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  90.000.000.000 / 255.000.000.000  
	{ PACKADDR( 91 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  91.000.000.000 / 255.000.000.000  
	{ PACKADDR( 92 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  92.000.000.000 / 255.000.000.000  
	{ PACKADDR( 93 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  93.000.000.000 / 255.000.000.000  
	{ PACKADDR( 94 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  94.000.000.000 / 255.000.000.000  
	{ PACKADDR( 95 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  95.000.000.000 / 255.000.000.000  
	{ PACKADDR( 96 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  96.000.000.000 / 255.000.000.000  
	{ PACKADDR( 97 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  97.000.000.000 / 255.000.000.000  
	{ PACKADDR( 98 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  98.000.000.000 / 255.000.000.000  
	{ PACKADDR( 99 , 0  , 0  , 0   ), MAKEMASK(  8 )}, //  99.000.000.000 / 255.000.000.000  
	{ PACKADDR( 100, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 100.000.000.000 / 255.000.000.000  
	{ PACKADDR( 101, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 101.000.000.000 / 255.000.000.000  
	{ PACKADDR( 102, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 102.000.000.000 / 255.000.000.000  
	{ PACKADDR( 103, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 103.000.000.000 / 255.000.000.000  
	{ PACKADDR( 104, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 104.000.000.000 / 255.000.000.000  
	{ PACKADDR( 105, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 105.000.000.000 / 255.000.000.000  
	{ PACKADDR( 106, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 106.000.000.000 / 255.000.000.000  
	{ PACKADDR( 107, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 107.000.000.000 / 255.000.000.000  
	{ PACKADDR( 108, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 108.000.000.000 / 255.000.000.000  
	{ PACKADDR( 109, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 109.000.000.000 / 255.000.000.000  
	{ PACKADDR( 110, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 110.000.000.000 / 255.000.000.000  
	{ PACKADDR( 111, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 111.000.000.000 / 255.000.000.000  
	{ PACKADDR( 112, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 112.000.000.000 / 255.000.000.000  
	{ PACKADDR( 113, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 113.000.000.000 / 255.000.000.000  
	{ PACKADDR( 114, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 114.000.000.000 / 255.000.000.000  
	{ PACKADDR( 115, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 115.000.000.000 / 255.000.000.000  
	{ PACKADDR( 116, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 116.000.000.000 / 255.000.000.000  
	{ PACKADDR( 117, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 117.000.000.000 / 255.000.000.000  
	{ PACKADDR( 118, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 118.000.000.000 / 255.000.000.000  
	{ PACKADDR( 119, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 119.000.000.000 / 255.000.000.000  
	{ PACKADDR( 120, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 120.000.000.000 / 255.000.000.000  
	{ PACKADDR( 121, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 121.000.000.000 / 255.000.000.000  
	{ PACKADDR( 122, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 122.000.000.000 / 255.000.000.000  
	{ PACKADDR( 123, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 123.000.000.000 / 255.000.000.000  
	{ PACKADDR( 127, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 127.000.000.000 / 255.000.000.000  
	{ PACKADDR( 169, 254, 0  , 0   ), MAKEMASK( 16 )}, // 169.254.000.000 / 255.255.000.000 
	{ PACKADDR( 172, 16 , 0  , 0   ), MAKEMASK( 12 )}, // 172.016.000.000 / 255.015.000.000  
	{ PACKADDR( 173, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 173.000.000.000 / 255.000.000.000  
	{ PACKADDR( 174, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 174.000.000.000 / 255.000.000.000  
	{ PACKADDR( 175, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 175.000.000.000 / 255.000.000.000  
	{ PACKADDR( 176, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 176.000.000.000 / 255.000.000.000  
	{ PACKADDR( 177, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 177.000.000.000 / 255.000.000.000  
	{ PACKADDR( 178, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 178.000.000.000 / 255.000.000.000  
	{ PACKADDR( 179, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 179.000.000.000 / 255.000.000.000  
	{ PACKADDR( 180, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 180.000.000.000 / 255.000.000.000  
	{ PACKADDR( 181, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 181.000.000.000 / 255.000.000.000  
	{ PACKADDR( 182, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 182.000.000.000 / 255.000.000.000  
	{ PACKADDR( 183, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 183.000.000.000 / 255.000.000.000  
	{ PACKADDR( 184, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 184.000.000.000 / 255.000.000.000  
	{ PACKADDR( 185, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 185.000.000.000 / 255.000.000.000  
	{ PACKADDR( 186, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 186.000.000.000 / 255.000.000.000  
	{ PACKADDR( 187, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 187.000.000.000 / 255.000.000.000  
	{ PACKADDR( 189, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 189.000.000.000 / 255.000.000.000  
	{ PACKADDR( 190, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 190.000.000.000 / 255.000.000.000  
	{ PACKADDR( 192, 0  , 2  , 0   ), MAKEMASK( 24 )}, // 192.000.002.000 / 255.255.255.000  
	{ PACKADDR( 192, 168, 0  , 0   ), MAKEMASK( 16 )}, // 192.168.000.000 / 255.255.000.000 
	{ PACKADDR( 197, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 197.000.000.000 / 255.000.000.000  
	{ PACKADDR( 198, 18 , 0  , 0   ), MAKEMASK( 15 )}, // 198.018.000.000 / 255.127.000.000 
	{ PACKADDR( 223, 0  , 0  , 0   ), MAKEMASK(  8 )}, // 223.000.000.000 / 255.000.000.000  
	{ PACKADDR( 224, 0  , 0  , 0   ), MAKEMASK(  4 )}, // 224.000.000.000 /   X.000.000.000 
*/	
};


void DownloadManager::doList()
{
	list <DownloadHandlerTuple>::iterator dhandler;
	logInfo("=--- %-69s ---=\n","DownloadManager");
	uint32_t i=0;
	for(dhandler = m_DownloadHandlers.begin();dhandler != m_DownloadHandlers.end();dhandler++,i++)
	{
		logInfo("  %i) %5s %-8s %s\n",i,dhandler->m_Protocol.c_str() ,dhandler->m_Handler->getDownloadHandlerName().c_str(), dhandler->m_Handler->getDownloadHandlerDescription().c_str());
	}
    logInfo("=--- %2i %-66s ---=\n",i, "DownloadHandlers registerd");

	for ( i = 0; i < sizeof(m_irLocalRanges) / sizeof(ip_range_t); i++ )
	{
		string range= inet_ntoa(*(in_addr *)&m_irLocalRanges[i].m_ulAddress);
		range += "/";
		range += inet_ntoa(*(in_addr *)&m_irLocalRanges[i].m_ulMask);
		logInfo("Ignoring %s \n", range.c_str());
	}
	logInfo("=--- %2i %-66s ---=\n\n",i, "Ranges ignored");

}


bool DownloadManager::isLocalAddress(uint32_t ulAddress)
{
	if ( !ulAddress || ulAddress == 0xFFFFFFFF )
		return false; // not an ip

	for ( uint32_t i = 0; i < sizeof(m_irLocalRanges) / sizeof(ip_range_t); i++ )
		if ( (ulAddress & m_irLocalRanges[i].m_ulMask) == m_irLocalRanges[i].m_ulAddress )
			return true;

	return false;
}

/**
 * gives the download to the handler who registerd the used protocol
 * 
 * @param down   the Download information
 * 
 * @return returns true if a handler was found, else false
 */
bool DownloadManager::downloadUrl(Download *down)
{

	// this is an event 
	SubmitEvent se(EV_DOWNLOAD,down);
	g_Nepenthes->getEventMgr()->handleEvent(&se);

	if (down->getDownloadUrl()->getPort() <= 0 || down->getDownloadUrl()->getPort() > 65536)
	{
		logWarn("malformed url 0<port<65536  , %s \n",down->getUrl().c_str());
		delete down;
		return false;
	}

	uint32_t ulAddress = inet_addr(down->getDownloadUrl()->getHost().c_str());

	logSpam("Checking Host %s for locality \n",down->getDownloadUrl()->getHost().c_str());
	if ( ulAddress  != INADDR_NONE )
	{	// address is either dns or invalid ip
		logSpam("Host %s is valid ip \n",down->getDownloadUrl()->getHost().c_str());
		bool bReplaceHost = false;
		if (isLocalAddress(ulAddress) == true)
		{ // local ip
			if (m_ReplaceLocalIps)
			{
				bReplaceHost = true;
				logInfo("Link %s  has local address, replacing with real ip \n",down->getUrl().c_str());

			}else
			{
				logDebug(" Address %s is local, we will not download \n",inet_ntoa( *(in_addr *)&ulAddress));
				delete down;
				return false;
			}
            
			
		}else
		{
			if (ulAddress == 0) // replace 0.0.0.0
			{
				bReplaceHost = true;
			}
		}

		if (bReplaceHost)
		{
/*			pDown->m_sUri  = pDown->m_pUri->m_protocol;
			pDown->m_sUri += "://";
			pDown->m_sUri += inet_ntoa( *(in_addr *)&pDown->m_ulAddress);
			pDown->m_sUri += "/";
			pDown->m_sUri += pDown->m_pUri->m_file;			 // fixme port
			pDown->m_pUri->m_host = inet_ntoa( *(in_addr *)&pDown->m_ulAddress);
*/	
			string sUrl =	down->getDownloadUrl()->getProtocol();
			sUrl += "://";
			uint32_t newaddr = down->getAddress();
			sUrl += inet_ntoa(*(in_addr *)&newaddr);
			down->getDownloadUrl()->setHost(newaddr);

#ifdef WIN32
			char *port = (char *)malloc(7);
			memset(port,0,7);
			_snprintf(port,7,":%i/",down->getDownloadUrl()->getPort());
            sUrl += port;
			free(port);
#else
			char *port;
			asprintf(&port,":%i/",down->getDownloadUrl()->getPort());
            sUrl += port;
			free(port);
#endif
			sUrl += down->getDownloadUrl()->getPath();
			down->setUrl(&sUrl);
			logInfo("Replaced Address, new URL is %s \n",sUrl.c_str());
		}
	}


	list <DownloadHandlerTuple>::iterator handler;
	for(handler = m_DownloadHandlers.begin(); handler != m_DownloadHandlers.end(); handler++)
	{
		if(handler->m_Protocol == down->getDownloadUrl()->getProtocol())
		{
			logInfo("Handler %s will download %s \n",handler->m_Handler->getDownloadHandlerName().c_str(),down->getUrl().c_str());
			handler->m_Handler->download(down);
			return true;
		}
	}

	logCrit("No Handler for protocoll %s \n",down->getDownloadUrl()->getProtocol().c_str());
	delete down;
	return false;
}


/**
 * download a file from a given url
 * the provied information is wrapped to a class Download
 * and given to downloadUrl(Download *)
 * 
 * @param url     the url to the file
 * @param address the hosts address who provided the url so we can replace 0.0.0.0 urls with his address
 * @param triggerline
 *                the triggerline
 * 
 * @return returns downloadUrl(Download *) return value
 */
bool DownloadManager::downloadUrl(char *url, uint32_t address, char *triggerline)
{
	Download *down = new Download(url,address,triggerline);
	
	return downloadUrl(down);
}




bool DownloadManager::downloadUrl(char *proto, char *user, char *pass, char *host, char *port, char *file, uint32_t address)
{
	string url = proto;
	 url += "://";
	 url +=  user;
	 url +=  ":";
	 url +=  pass;
	 url +=  "@";
	 url +=  host;
	 url +=  ":";
	 url +=  port;
	 url += "/";
	 url += file;

	Download *down = new Download((char *)url.c_str(),address,(char *)url.c_str());

	down->getDownloadUrl()->setProtocol(proto);
	down->getDownloadUrl()->setUser(user);
	down->getDownloadUrl()->setPass(pass);
	down->getDownloadUrl()->setHost(host);
	down->getDownloadUrl()->setPort(atoi(port));
	down->getDownloadUrl()->setPath(file);
	down->getDownloadUrl()->setFile(file);

	return downloadUrl(down);
}



/**
 * registers a downloadhandler with his ptr and his protocol
 * 
 * @param handler  pointer to the DownloadHandler
 * @param protocol the protocol, f.e. "http"
 * 
 * @return returns true if the downloadhandler could be registerd
 *         false if another downloadhandler already registerd the protocol
 */
bool DownloadManager::registerDownloadHandler(DownloadHandler * handler, const char * protocol)
{
	DownloadHandlerTuple dht;
	dht.m_Handler = handler;
	dht.m_Protocol = protocol;
	m_DownloadHandlers.push_back(dht);
	logInfo("Registerd %s as handler for protocol %-9s (%i protocols supported)\n",handler->getDownloadHandlerName().c_str(),protocol, m_DownloadHandlers.size());
	return true;
}


/**
 * delete a downloadhandler from the manager
 * 
 * @param protocol the protocols handler we want to remove from the downloadmanager
 */
void DownloadManager::unregisterDownloadHandler(const char * protocol)
{

	return;
}
