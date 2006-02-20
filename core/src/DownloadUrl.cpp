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
 /* $id */

#ifdef WIN32

#else
#include <arpa/inet.h>
#endif

#include "DownloadUrl.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

typedef struct portTable
{
    const char      *protocolName;
    short           port;
};
 
portTable g_portTable[] = {
    { "ftp"		, 21 },
	{ "tftp"	, 69 },
    { "http"	, 80 },
    { "https"	, 443},
	{ "csend"	, 999}

};
 
DownloadUrl::DownloadUrl(char *psurl)
{
    string      urlStr = psurl;
 
    if ((int)urlStr.find("://") >= (int)0)
    {
//        logSpam("Found Proto %i\n", urlStr.find("://"));
        m_protocol = urlStr.substr(0, urlStr.find("://"));
        urlStr = urlStr.substr(urlStr.find("://") + string("://").size());
    }
    else
	{

        m_protocol = "";
	}
    
 
    // check for user/host data
    if( urlStr.find("@") != urlStr.npos )
    {
        m_user = urlStr.substr(0, urlStr.find("@"));
        urlStr = urlStr.substr(urlStr.find("@") + string("@").size());
 
        // pass as well?
        if( m_user.find(":") != m_user.npos )
        {
            m_pass = m_user.substr(m_user.find(":") + string(":").size());
            m_user = m_user.substr(0, m_user.find(":"));
        }
    }
 
//    if ((int) urlStr.find("/") >= (int)0)
    m_host = urlStr.substr(0, urlStr.find("/"));
//    else

 
    // port?
    if( m_host.find(":") != m_host.npos )
    {
//        fprintf(stderr,"Porty %s\n",m_host.substr(m_host.find(":") + string(":").size()).c_str());
        m_port =               atoi(m_host.substr(m_host.find(":") + string(":").size()).c_str());
        m_host = m_host.substr(0, m_host.find(":"));
    }
    else
    {
        // try to find the default port
		m_port = 80;

        if (m_protocol.size() != 0)
        {
            for( unsigned int i = 0; i < sizeof(g_portTable) / sizeof(portTable); i++ )
            {
                if( g_portTable[i].protocolName == m_protocol )
                    m_port = g_portTable[i].port;
            }
        }else
        {   // we got no information about protocol or shit, so we say port is 80
            m_port = 80;
        }
    }
 
    if ((int)urlStr.find("/") >= (int)0)
	{
        m_path = urlStr.substr(urlStr.find("/") + string("/").size());
	}
    else
    {
		m_path = "";
	}


    // dir ?
    if (m_path.size() > 0)
    {
        if ((int)m_path.rfind("/") >=(int)0 )
        {
            m_dir  = m_path.substr(0,m_path.rfind("/")+1);
        }

    // file
        if ((int)m_path.rfind("/") >=(int)0 )
        {
            m_file = m_path.substr(m_path.rfind("/")+1,m_path.size());
        }else
            if ( m_dir.size() == 0)
        {
                m_file = m_path;
        }
    }

	if (m_user.size() > 0 && m_pass.size() > 0)
	{
		m_auth = m_user + ":" + m_pass;
	}
/*
    if (m_protocol.size() != 0)
        fprintf(stderr,"Prot %s\n",m_protocol.c_str());
    
    if (m_host.size() != 0)
        fprintf(stderr,"Host %s\n",m_host.c_str());

    fprintf(stderr,"Port %i\n",m_port);

    if (m_path.size() != 0)
        fprintf(stderr,"Path %s\n",m_path.c_str());

    if(m_dir.size() != 0)
        fprintf(stderr,"Dir  %s\n",m_dir.c_str());

    if (m_file.size() != 0)
        fprintf(stderr,"File %s\n",m_file.c_str());
    else
    {
        m_file = "index.html";
        fprintf(stderr,"File HAX %s\n",m_file.c_str());
    }
*/
	if (m_file.size() == 0)
		m_file = "index.html";


}

DownloadUrl::~DownloadUrl()
{
	logPF();
}

bool DownloadUrl::checkUrl()
{
/*    if (m_host.size() == 0)
    {
        return false;
    }

    unsigned int i;
    for (i=0;i<m_host.size();i++)
    {
        if (isalnum(m_host[i]) == 0 && m_host[i] != '-' && m_host[i] != '_' && m_host[i] != '.' )
        {
//            fprintf(stderr,"Malformed url\n");
            return false;
        }
    }*/
    return true;

}


// proto
void DownloadUrl::setProtocol(char *proto)
{
	m_protocol = proto;
}

string          DownloadUrl::getProtocol()
{
	return m_protocol;
}

// user
void DownloadUrl::setUser(char *user)
{
	m_user = user;
}

string          DownloadUrl::getUser()
{
	return m_user;
}


// pass
void DownloadUrl::setPass(char *pass)
{
	m_pass = pass;
}

string          DownloadUrl::getPass()
{
	return m_pass;
}


// auth
void DownloadUrl::setAuth(char *auth)
{
	m_auth = auth;
}

string          DownloadUrl::getAuth()
{
	return m_auth;
}



// host
void 			DownloadUrl::setHost(unsigned long host)
{
	m_host = inet_ntoa(*(in_addr *)&host);
}

void 			DownloadUrl::setHost(char *host)
{
	m_host = host;
}


string          DownloadUrl::getHost()
{
	return m_host;
}


// port
void DownloadUrl::setPort(unsigned int port)
{
	m_port = port;
}

unsigned int    DownloadUrl::getPort()
{
	return m_port;
}



// path 
void DownloadUrl::setPath(char *path)
{
	m_path = path;
}

string          DownloadUrl::getPath()
{
	return m_path;
}




// file
void DownloadUrl::setFile(char *file)
{
	m_file = file;
}

string          DownloadUrl::getFile()
{
	return m_file;
}



// dir
void DownloadUrl::setDir(char *dir)
{
	m_dir = dir;
}

string          DownloadUrl::getDir()
{
	return m_dir;
}


// anchor
void DownloadUrl::setAnchor(char *anchor)
{
	m_anchor = anchor;
}

string          DownloadUrl::getAnchor()
{
	return m_anchor;
}
