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



#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <time.h>
#include <errno.h>
#include <string.h>

#include <sstream>
#include <map>



#include "PGDownloadContext.hpp"
#include "Download.hpp"
#include "DownloadBuffer.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

extern "C"
{
	#include "bencoding.h"
}

using namespace std;
using namespace nepenthes;

PGDownloadContext::PGDownloadContext(Download *down)
{
	m_hash_md5      = down->getMD5Sum();
	m_hash_sha512   = down->getSHA512Sum();
	m_Url           = down->getUrl();   

	uint32_t host   = down->getRemoteHost();
	m_RemoteHost    = inet_ntoa(*(struct in_addr *)&host);

	host = down->getLocalHost();
	m_LocalHost     = inet_ntoa(*(struct in_addr *)&host);  

	m_FileContent   = string(down->getDownloadBuffer()->getData(),down->getDownloadBuffer()->getSize());

	m_State = PG_NULL;

	serialize();
}

PGDownloadContext::PGDownloadContext(string md5, string sha512, string url, string remote, string local, string file, string path)
{
	m_hash_md5 = md5;
	m_hash_sha512 = sha512;
	m_Url = url;
	m_RemoteHost = remote;
	m_LocalHost = local;
	m_FileContent = file;

	m_FilePath = path;
}

PGDownloadContext::~PGDownloadContext()
{

}


/**
 * 
 * @param path   path to the file
 * 
 * @return pointer to PGDownloadContext on success, else NULL
 */
PGDownloadContext *PGDownloadContext::unserialize(const char *path)
{
	struct stat s;
	if ( stat(path, &s) != 0 )
		return NULL;

	unsigned char *data = (unsigned char *)malloc(s.st_size);
	memset(data,0,s.st_size);
	FILE *f = fopen(path,"r");
	fread(data,1,s.st_size,f);
	fclose(f);

	Bencoding_Context *c = Bencoding_createContext();
	if ( Bencoding_decodeBuffer(c, data, s.st_size) != 0 )
	{
		logWarn("Error reading benc file %s %s\n",path,Bencoding_getErrorMessage(c));
		free(data);
		return NULL;
	}

	free(data);

	Bencoding_Item *item;

	map< string, string, benc_key_comp > bencvalues;

	string key;
	string value;

	int i;

	while ( (item = Bencoding_getNext(c)) != NULL )
	{
		switch ( item->m_type )
		{
		case Bencoding_TypeDict:
			printf("(dict)\n");
			for ( i = 0; i < item->m_dict.m_size; i++ )
			{
				key = string((char *)item->m_dict.m_keys[i].m_data,item->m_dict.m_keys[i].m_len);

				switch ( item->m_dict.m_values[i].m_type )
				{
				case Bencoding_TypeDict:
					break;

				case Bencoding_TypeInt:
					break;

				case Bencoding_TypeString:
					value = string((char *)item->m_dict.m_values[i].m_string.m_data,item->m_dict.m_values[i].m_string.m_len);
					break;

				case Bencoding_TypeList:
					break;
				}

				bencvalues[key] = value;

			}
			break;

		case Bencoding_TypeInt:
			break;

		case Bencoding_TypeString:
			break;

		case Bencoding_TypeList:
			break;

		}
	}

	PGDownloadContext *ctx = new PGDownloadContext(bencvalues["hash_md5"], 
					  bencvalues["hash_sha512"], 
					  bencvalues["url"], 
					  bencvalues["remote"], 
					  bencvalues["local"], 
					  bencvalues["file"], 
					  string(path));


	Bencoding_destroyContext(c);


	return ctx;

}

string  itos( long i )
{
	std::ostringstream s;
	s << i;
	return s.str();
}

/**
 * write class content bencoded to disc
 * 
 * @return on success, size written, -1 on error
 */
uint32_t PGDownloadContext::serialize()
{
	struct tm       t;
	time_t          stamp;
	time(&stamp);

	localtime_r(&stamp, &t);

	char filepath[1024];

	snprintf(filepath,1024,"%04d%02d%02d-%02d%02d%02d-0", 
			 t.tm_year + 1900, 
			 t.tm_mon + 1, 
			 t.tm_mday, 
			 t.tm_hour, 
			 t.tm_min, 
			 t.tm_sec);


	string fullpath = string("var/spool/submitpostgres/") + string(filepath);
	struct stat s;

	int i=1;
	while ( stat(fullpath.c_str(),&s) == 0 )
	{
		snprintf(filepath,1024,"%04d%02d%02d-%02d%02d%02d-%i", 
				 t.tm_year + 1900, 
				 t.tm_mon + 1, 
				 t.tm_mday, 
				 t.tm_hour, 
				 t.tm_min, 
				 t.tm_sec,
				 i);

		fullpath = string("var/spool/submitpostgres/") + string(filepath);
		i++;
	}

	FILE *f;

	if ( (f = fopen(fullpath.c_str(),"w")) == NULL )
	{
		logCrit("Could not open %s (%s)\n",fullpath.c_str(),strerror(errno));
		return 0;
	}

	m_FilePath = fullpath;

	/*
	(dict)
		url
			(string) -> http://
		remote 
			(string) -> 192.168.47.11
		local
			(string) -> 192.168.23.22
		hash_md5
			(string) -> HD6D8S0S...
		hash_sh512
			(string) -> HD6D8S0S...
		file 
			(string) -> <binaryfile>

	*/

	string benc = "";

	benc += "d";
	benc += "3:url";
	benc += itos(m_Url.size()) + ":" + m_Url;
	benc += "6:remote";
	benc += itos(m_RemoteHost.size()) + ":" + m_RemoteHost;
	benc += "5:local";
	benc += itos(m_LocalHost.size()) + ":" + m_LocalHost;
	benc += "8:hash_md5";
	benc += "32:" + m_hash_md5;
	benc += "11:hash_sha512";
	benc += "128:" + m_hash_sha512;
	benc += "4:file";
	benc += itos(m_FileContent.size()) + ":";
	benc.append(m_FileContent);
	benc += "e";


	fwrite(benc.data(),1,benc.size(),f);

	fclose(f);
	
	return 0;
}


/**
 * remove the file from disc
 * 
 * @return true on success,else false
 */
bool PGDownloadContext::remove()
{
	if (unlink(m_FilePath.c_str()) == 0)
		return true;
	logWarn("Could not remove %s (%s)\n",m_FilePath.c_str(),strerror(errno));
	return false;
}


string PGDownloadContext::getHashMD5()
{
	return m_hash_md5;
}

string PGDownloadContext::getHashSHA512()
{
	return m_hash_sha512;
}

string *PGDownloadContext::getUrl()
{
	return &m_Url;
}

string PGDownloadContext::getRemoteHost()
{
	return m_RemoteHost;
}

string PGDownloadContext::getLocalHost()
{
	return m_LocalHost;
}

string  *PGDownloadContext::getFileContent()
{
	return &m_FileContent;
}

uint32_t PGDownloadContext::getFileSize()
{
	return m_FileContent.size();
}

pg_submit_state PGDownloadContext::getState()
{
	return m_State;
}

void PGDownloadContext::setState(pg_submit_state s)
{
	m_State = s;
}
