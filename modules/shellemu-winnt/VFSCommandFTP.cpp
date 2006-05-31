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
 
#include "VFSCommandFTP.hpp"
#include "VFSNode.hpp"
#include "VFSDir.hpp"
#include "VFSFile.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "VFS.hpp"
#include "DownloadManager.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"
#include "Download.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_shell

using namespace nepenthes;
using namespace std;

VFSCommandFTP::VFSCommandFTP(VFSNode *parent,VFS *vfs)
{
	m_Name =	"ftp.exe";
	m_ParentNode = parent;
	m_Type = VFS_EXE;
	m_VFS = vfs;
}

VFSCommandFTP::~VFSCommandFTP()
{

} 


/*
C:\>ftp -h

Überträgt Dateien zu und von einem Computer, der den TFTP-Dienst ausführt
(auch Daemon genannt).

FTP kann interaktiv verwendet werden.

FTP [-v] [-d] [-i] [-n] [-g] [-s:Dateiname] [-a] [-w:Fenstergröße] [-A]
    [Host]

  -v             Unterdrückt das Anzeigen der Rückmeldungen von
                 Remoteservern.
  -n             Unterdrückt das automatische Anmelden nach dem ersten
                 Verbindungsaufbau.
  -i             Deaktiviert die interaktive Eingabe, während mehrere
                 Dateien übertragen werden.
  -d             Aktiviert Debugging.
  -g             Deaktiviert "Globbing" des Dateinamens (siehe auch GLOB-
                 Befehl).
  -s:Dateiname   Gibt eine Textdatei an, die FTP-Befehle enthält. Die
                 Befehle werden nach dem Starten von FTP automatisch
                 ausgeführt.
  -a             Verwendet eine beliebige lokale Schnittstelle, wenn
                 Datenverbindungen gebunden werden.
  -A             Meldet den Benutzer als "Anonymus" an.
  -w:Puffergröße Überschreibt die Standardgröße des Übertragungspuffers
                 von 4096.
  Host           Gibt den Hostnamen oder die IP-Adresse des Remotehosts
                 an, zu dem eine Verbindung hergestellt wird.

Hinweis:
  - Die Befehle "mget" und "mput" akzeptieren y/n/q für yes/no/quit.
  - Verwenden Sie STRG+C zum Abbrechen von Befehlen.
*/  


int32_t VFSCommandFTP::run(vector<string> *paramlist)
{ 

	bool direktconnect = true;
	bool anonymouslogin = false;

	vector <string> slist = *paramlist;
	vector <string>::iterator it;
	string host = "nohostyet";
	string port = "21";
	string user = "nouseryet";
	string pass = "nopassyet";
	string getfile = "nofileyet";
	string path = "";

	string filename = "";
	uint8_t	downloadflags=0;


	for(it=slist.begin();it!=slist.end();it++)
	{
// FTP [-v] [-d] [-i] [-n] [-g] [-s:Dateiname] [-a] [-w:Fenstergr��e] [-A]     [Host]

		logDebug("ftp.exe param %s \n",&*it->c_str());
		if (strncmp(&*it->c_str(),"-v",2) == 0)	
			continue;
		else
		if (strncmp(&*it->c_str(),"-d",2) == 0) // debugging
			continue;
		else
		if (strncmp(&*it->c_str(),"-i",2) == 0)	// non interactive
			continue;
		else
		if (strncmp(&*it->c_str(),"-n",2) == 0)	
		{
			direktconnect = false;
			continue;
		}
		else
		if (strncmp(&*it->c_str(),"-a",2) == 0)	// idiotic description i guess binding the port on any interface using active ftp
			continue;
		else
        if (strncmp(&*it->c_str(),"-w:",3) == 0)	// window size foobar
			continue;
		else
		if (strncmp(&*it->c_str(),"-s:",3) == 0)
		{
			filename = it->substr(3,it->size()-2);
		}else
        if (strncmp(&*it->c_str(),"-A",2) == 0)	// anonymous login
		{
			anonymouslogin = true;
			user = "anonymous";
			pass = "guest";
		}
		else
			host = *it;
	}

	if (filename != "" )
	{
		logDebug("Filenameis %s\n",filename.c_str());
		VFSFile *file = m_VFS->getCurrentDir()->getFile((char *)filename.c_str());
		if (file == NULL)
		{
			logCrit("VFS FTP file %s not found\n",filename.c_str());
			return 0;
		}
		logDebug("file content is is \n%.*s\n",file->getSize(),(char *)file->getData());

		uint32_t i=0;
		int32_t linestart=0;
		int32_t linestopp=0;
		vector <string> ftpcommands;
		while(i<file->getSize())
		{
			if (memcmp(file->getData()+i,"\n",1) == 0)
			{

				i++;
				linestopp = i;

				logSpam(" line is '%.*s'\n",linestopp-linestart,file->getData()+linestart);
				string command( file->getData()+linestart,linestopp-linestart-1);
				ftpcommands.push_back(command);
				linestart = linestopp;
			}
			i++;
		}


		vector <string>::iterator jt;
		vector <string> paramlist;

		ftp_command_state state = NEXT_IS_SOMETHING;

		for ( jt=ftpcommands.begin();jt!=ftpcommands.end();jt++ )
		{
			string params(&*jt->c_str());
			i=0;
			bool haschar = false;
			uint32_t wordstart=0;
			uint32_t wordstopp=0;
			paramlist.clear();
			while ( i<=params.size() )
			{
				if ( ( ( params[i] == ' ' || params[i] == '\0') && haschar == true) )
				{
					wordstopp = i;
					string word = params.substr(wordstart,wordstopp-wordstart);
					logDebug("Word is %i %i '%s' \n",wordstart,wordstopp,word.c_str());
					paramlist.push_back(word);
					haschar = false;
				} else
					if ( isgraph(params[i]) && haschar == false )
				{
					haschar = true;
					wordstart = i;
				}
				i++;
			}



			switch (state)
			{

			case NEXT_IS_SOMETHING:
				if ( strncasecmp((char *)&*jt->c_str(),"open",4) == 0 )
				{
					logSpam("open line %s \n",&*jt->c_str());
					switch(paramlist.size())
					{
					case 1:
						state = NEXT_IS_HOST;
						break;
					default:
						host = paramlist[1];
						if ( paramlist.size() >=3 )
							port = paramlist[2];
						else
							port = "21";

						if(direktconnect == true && anonymouslogin == false)
						{
							state = NEXT_IS_USER;
						}else
						{
							state = NEXT_IS_SOMETHING;
						}

						break;

					}

					logDebug("ftp://%s:%s\n",host.c_str(),port.c_str());
				} else
				if ( strncasecmp((char *)&*jt->c_str(),"user",4) == 0 )
				{
					if ( direktconnect == true )
					{
						logCrit("VFS FTP State ERROR user\n");
					} else
					{

						switch ( paramlist.size() )
						{
						case 1:
							state = NEXT_IS_USER;
							break;

						case 2:
							user = paramlist[1];
							state = NEXT_IS_PASS;
							break;
						case 3:
							user = paramlist[1];
							pass = paramlist[2];
							break;
						}
						logDebug("ftp://%s:%s@%s:%s\n",user.c_str(),pass.c_str(),host.c_str(),port.c_str());
					}
				} else
				if ( strncasecmp((char *)&*jt->c_str(),"get",3) == 0 )
				{
					switch(paramlist.size())
					{
					case 1:
						state = NEXT_IS_FILE;
						logDebug("get without param, next arg is filename\n");
						break;
					default:
						getfile = paramlist[1];
//							logDebug("ftp://%s:%s@%s:%s/%s\n",user.c_str(),pass.c_str(),host.c_str(),port.c_str(),getfile.c_str());
						startDownload(host,port,user,pass,path,getfile,downloadflags);
					}
				}else
				if ( strncasecmp((char *)&*jt->c_str(),"binary",6) == 0 || 
					 strncasecmp((char *)&*jt->c_str(),"bin",3) == 0)
				{
					downloadflags |= DF_TYPE_BINARY;
				}else
				if ( strncasecmp((char *)&*jt->c_str(),"cd",2) == 0 )
				{
					switch ( paramlist.size() )
					{
					case 1:
						state = NEXT_IS_PATH;
						break;

					case 2:
						path = paramlist[1];
						break;
					}

				}

				break;



			case NEXT_IS_HOST:
				switch ( paramlist.size() )
				{

				case 1:
					host = paramlist[0];
					port = "21";
					break;

				case 2:
					host = paramlist[0];
					port = paramlist[1];
					break;
				}

				if(direktconnect == true && anonymouslogin == false )
				{
					state = NEXT_IS_USER;
				}else
				{
					state = NEXT_IS_SOMETHING;
				}
				break;


			case NEXT_IS_PORT:
				break;

			case NEXT_IS_USER:
				switch ( paramlist.size() )
				{
				case 1:
					user = paramlist[0];
					state = NEXT_IS_PASS;
					break;

				default:
					logCrit("Broken State NEXT_IS_USER\n");
					state = NEXT_IS_SOMETHING;
					break;
				}
				break;

			case NEXT_IS_PASS:
				pass = paramlist[0];
				state = NEXT_IS_SOMETHING;
				break;


			case NEXT_IS_FILE:
				getfile = paramlist[0];
				startDownload(host,port,user,pass,path,getfile,downloadflags);
				state = NEXT_IS_SOMETHING;
				break;

			case NEXT_IS_PATH:
				path = paramlist[0];
				state = NEXT_IS_SOMETHING;
				break;

			}

		}

	}
    return 0;
}

bool VFSCommandFTP::startDownload(string host, string port, string user, string pass, string path, string getfile, uint8_t downloadflags)
{
	logPF();
	string url;
	string file;

	if (path == "")
	{
		url = "ftp://" + user + ":" + pass + "@" + host+ ":" + port + "/" + getfile;
	}else
	{
		url = "ftp://" + user + ":" + pass + "@" + host+ ":" + port;

		int pathlen = path.size()-1;
		if (path[0] != '/' )
		{
			url += "/" + path;
			file += "/" + path;
		}
		if (path[pathlen] != '/')
		{
			url += "/";
			file += "/";
		}
		url += getfile;
		file += getfile;
	}
	 
	uint32_t remotehost = 0;
	uint32_t localhost = 0;

	if (m_VFS->getDialogue()->getSocket() != NULL)
	{
		logSpam("VFSCommandFTP Setting Hosts %i %i\n",remotehost,localhost);
		remotehost = m_VFS->getDialogue()->getSocket()->getRemoteHost();
		localhost  = m_VFS->getDialogue()->getSocket()->getLocalHost();

	}

	logSpam("VFSCommandFTP LocalHost %s\n",inet_ntoa(*(in_addr *)&localhost));
	logSpam("VFSCommandFTP RemoteHost %s\n",inet_ntoa(*(in_addr *)&remotehost));

	if (strstr(user.c_str(),"@") == NULL && strstr(pass.c_str(),"@") == NULL)
	{
		g_Nepenthes->getDownloadMgr()->downloadUrl(	localhost,
													(char *)url.c_str(),
													remotehost,
													(char *)url.c_str(),
													downloadflags);
	}else
	{
		g_Nepenthes->getDownloadMgr()->downloadUrl(	localhost,
													"ftp",
												   (char *)user.c_str(),
												   (char *)pass.c_str(), 
												   (char *)host.c_str(), 
												   (char *)port.c_str(), 
												   (char *)file.c_str(),
												   remotehost,
												   downloadflags); 
	}
	return true;
}
