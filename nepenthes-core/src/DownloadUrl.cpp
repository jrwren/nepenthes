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
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#include "DownloadUrl.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

 
portTable g_portTable[] = {
    { "ftp"		, 21 },
	{ "tftp"	, 69 },
    { "http"	, 80 },
    { "https"	, 443},
	{ "csend"	, 999}

};
 
/**
 * DownloadUrl constructor
 * 
 * will split the url in its parts
 * 
 * @param psurl  the url
 */
DownloadUrl::DownloadUrl(char *psurl)
{
    string      urlStr = psurl;
 
    if ((int32_t)urlStr.find("://") >= (int32_t)0)
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
 
//    if ((int32_t) urlStr.find("/") >= (int32_t)0)
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
            for( uint32_t i = 0; i < sizeof(g_portTable) / sizeof(portTable); i++ )
            {
                if( g_portTable[i].protocolName == m_protocol )
                    m_port = g_portTable[i].port;
            }
        }else
        {   // we got no information about protocol or shit, so we say port is 80
            m_port = 80;
        }
    }
 
    if ((int32_t)urlStr.find("/") >= (int32_t)0)
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
        if ((int32_t)m_path.rfind("/") >=(int32_t)0 )
        {
            m_dir  = m_path.substr(0,m_path.rfind("/")+1);
        }

    // file
        if ((int32_t)m_path.rfind("/") >=(int32_t)0 )
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

    uint32_t i;
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


/**
 * set used protocol
 * 
 * @param proto
 */
void DownloadUrl::setProtocol(char *proto)
{
	m_protocol = proto;
}

/**
 * get used protocol
 * 
 * @return returns protocol as string
 */
string          DownloadUrl::getProtocol()
{
	return m_protocol;
}

/**
 * set the user (to login somewhere)
 * 
 * @param user   the user
 */
void DownloadUrl::setUser(char *user)
{
	m_user = user;
}

string          DownloadUrl::getUser()
{
	return m_user;
}


/**
 * set the password (to login somewhere)
 * 
 * @param pass   the password
 */
void DownloadUrl::setPass(char *pass)
{
	m_pass = pass;
}

/**
 * get the password
 * 
 * @return returns the password as string
 */
string          DownloadUrl::getPass()
{
	return m_pass;
}


/**
 * set the auth
 * ( auth is in  form USER:PASS for curl )
 * 
 * @param auth   the auth
 */
void DownloadUrl::setAuth(char *auth)
{
	m_auth = auth;
}

/**
 * get the auth
 * 
 * @return returns the auth as string
 */
string          DownloadUrl::getAuth()
{
	return m_auth;
}



/**
 * set the remote's host
 * 
 * @param host   remotes ip address
 */
void 			DownloadUrl::setHost(uint32_t host)
{
	m_host = inet_ntoa(*(in_addr *)&host);
}

/**
 * set the remotes host
 * 
 * @param host   remote host domain
 */
void 			DownloadUrl::setHost(char *host)
{
	m_host = host;
}


/**
 * get the remotes host
 * 
 * @return returns remote host as string
 */
string          DownloadUrl::getHost()
{
	return m_host;
}


/**
 * set the port to use
 * 
 * @param port   the port
 */
void DownloadUrl::setPort(uint32_t port)
{
	m_port = port;
}

/**
 * get the port to use
 * 
 * @return returns the port to use
 */
uint32_t    DownloadUrl::getPort()
{
	return m_port;
}



/**
 * set the path to use
 * 
 * @param path   the path to use
 */
void DownloadUrl::setPath(char *path)
{
	m_path = path;
}

/**
 * get the path to use
 * 
 * @return returns the path to use as string without leading /
 */
string          DownloadUrl::getPath()
{
	return m_path;
}




/**
 * set the file to download
 * 
 * @param file   returns the file as string
 */
void DownloadUrl::setFile(char *file)
{
	m_file = file;
}

/**
 * get the file to download
 * 
 * @return the file as string
 */
string          DownloadUrl::getFile()
{
	return m_file;
}



/**
 * set the dir to use
 * 
 * @param dir    the dir
 */
void DownloadUrl::setDir(char *dir)
{
	m_dir = dir;
}

/**
 * get the dir to use
 * 
 * @return returns the dir to use as string
 */
string          DownloadUrl::getDir()
{
	return m_dir;
}


/**
 * set the anchor to use
 * 
 * @param anchor the anchor
 */
void DownloadUrl::setAnchor(char *anchor)
{
	m_anchor = anchor;
}

/**
 * get the anchor
 * 
 * @return returns the anchor as string
 */
string          DownloadUrl::getAnchor()
{
	return m_anchor;
}
