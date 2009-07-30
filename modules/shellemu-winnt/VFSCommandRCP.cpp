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
 
#include <stdio.h>

#include "VFSCommandRCP.hpp"
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

#include <cstring>

using namespace nepenthes;
using namespace std;

VFSCommandRCP::VFSCommandRCP(VFSNode *parent,VFS *vfs)
{
	m_Name =	"rcp.exe";
	m_ParentNode = parent;
	m_Type = VFS_EXE;
	m_VFS = vfs;
}

VFSCommandRCP::~VFSCommandRCP()
{

} 


/*
Kopiert Dateien auf Computer bzw. von Computern, die RCP-Dienst ausführen.

RCP [-a|-b] [-h] [-r] [Host][.Benutzer:]Quelle [Host][.Benutzer:]
    Pfad\Ziel

  -a                 Gibt ASCII-Übertragungsmodus an. Dieser Modus setzt
                     jedes EOL-Zeichen (End of Line) in ein CR-Zeichen
                     (Carriage Return) für UNIX, und in ein CR/LF-
                     Zeichen (Carriage Return/Line Feed) für PCs um.
                     Dies ist der Standardübertragungsmodus.
  -b                 Gibt Binärübertragungsmodus an.
  -h                 Überträgt versteckte Dateien.
  -r                 Kopiert den Inhalt aller Unterverzeichnisse;
                     Ziel muss ein Verzeichnis sein.
  Host               Gibt den lokalen Host oder den Remotehost an. Wird
                     Host als IP-Adresse angegeben, muss auch der Benutzer
                     angegeben werden.
  .Benutzer:         Gibt einen Benutzernamen an, der anstelle des
                     aktuellen Benutzernamens verwendet werden soll.
  Quelle             Gibt die zu kopierenden Dateien an.
  Pfad\Ziel          Gibt den Pfad bezüglich des Anmeldeverzeichnisses
                     auf dem Remotehost an. Verwenden Sie bei Remotepfaden
                     die Escapezeichen (\ , " oder ') zur Angabe von
                     Platzhalterzeichen auf dem Remotehost.

*/
int32_t VFSCommandRCP::run(vector<string> *paramlist)
{ // rcp -b 82.24.130.196.thebuz:msnn.exe msnn.exe
	logPF();
	vector <string> slist = *paramlist;
	vector <string>::iterator it;
	uint8_t	downloadflags=0;

	for(it=slist.begin();it!=slist.end();it++)
	{
// FTP [-v] [-d] [-i] [-n] [-g] [-s:Dateiname] [-a] [-w:Fenstergr��e] [-A]     [Host]

		logDebug("rcp.exe param %s \n",&*it->c_str());
		if (strncmp(&*it->c_str(),"-a",2) == 0)	
			continue;
		else
		if (strncmp(&*it->c_str(),"-b",2) == 0) // binary mode
		{
			downloadflags |= DF_TYPE_BINARY;
			continue;
		}
		else
		if (strncmp(&*it->c_str(),"-h",2) == 0)	// hidden files
			continue;
		else
		if (strncmp(&*it->c_str(),"-r",2) == 0)	// recursive
			continue;
		else
		if (strncmp(&*it->c_str(),"-a",2) == 0)	// idiotic description i guess binding the port on any interface using active ftp
			continue;
		else
		{
			string host = "";
			string user = "";
			string file = "";

			string remotecopy = &*it->c_str();

			int stoppdoppel = remotecopy.find(":",0);
			if (stoppdoppel < 0)
			{
				logWarn("rcp.exe no : found in url\n");
				return 0;
			}
			host = remotecopy.substr(0,stoppdoppel);

			int stopppunkt = host.rfind(".",host.size());
			if (stopppunkt < 0)
			{
				logWarn("rcp.exe no . found in url\n");
				return 0;
			}

			host = host.substr(0,stopppunkt);
			user = remotecopy.substr(stopppunkt+1,stoppdoppel-stopppunkt-1);
			file = remotecopy.substr(stoppdoppel+1,remotecopy.size()-stoppdoppel);

			printf("  user %s host %s file %s\n",user.c_str(),host.c_str(),file.c_str());

			string url = "rcp://" + user + "@" + host + "/" + file;

			uint32_t remotehost = 0;
			uint32_t localhost = 0;

			if ( m_VFS->getDialogue()->getSocket() != NULL )
			{
				logSpam("VFSCommandRCP Setting Hosts %i %i\n",remotehost,localhost);
				remotehost = m_VFS->getDialogue()->getSocket()->getRemoteHost();
				localhost  = m_VFS->getDialogue()->getSocket()->getLocalHost();
			}

			logSpam("VFSCommandRCP LocalHost %s\n",inet_ntoa(*(in_addr *)&localhost));
			logSpam("VFSCommandRCP RemoteHost %s\n",inet_ntoa(*(in_addr *)&remotehost));

			g_Nepenthes->getDownloadMgr()->downloadUrl( localhost,
															(char *)url.c_str(),
															remotehost,
															"Download Initiated by Shell Command",
															downloadflags);
			return 0;

		}
	}
	return 0;
}
