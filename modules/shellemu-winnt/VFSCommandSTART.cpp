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
 
#include "VFSCommandSTART.hpp"
#include "VFSNode.hpp"
#include "VFSDir.hpp"
#include "VFSFile.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "VFS.hpp"
#include "DownloadManager.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_shell

using namespace nepenthes;
using namespace std;

VFSCommandSTART::VFSCommandSTART(VFSNode *parent,VFS *vfs)
{
	m_Name =	"start.exe";
	m_ParentNode = parent;
	m_Type = VFS_EXE;
	m_VFS = vfs;
}

VFSCommandSTART::~VFSCommandSTART()
{

} 
/*
Startet ein eigenes Fenster, um ein bestimmtes Programm oder einen Befehl
auszuführen.

START ["Titel"] [/Dpath] [/I] [/MIN] [/MAX] [/SEPARATE | /SHARED]
      [/LOW | /NORMAL | /HIGH | /REALTIME] | /ABOVENORMAL | /BELOWNORMAL]
      [/WAIT] [/B] [Befehl/Programm]
      [Parameter]

  "Titel"     Der Titel des neuen Fensters.
  Pfad        Startverzeichnis.
  B           Startet Anwendung, ohne ein neues Fenster zu öffnen. Die
              Anwendung ignoriert STRG+C. Wenn die Anwendung nicht selbständig
              STRG+C überprüft, ist STRG+UNTBR die einzige Möglichkeit, um die
              Anwendung abzubrechen.
  I           Die CMD.EXE beim Aufruf übergebene Umgebung soll die neue
              aktuelle Umgebung sein.
  MIN         Startet das Fenster minimiert.
  MAX         Startet das Fenster maximiert.
  SEPARATE    Startet 16-Bit-Windows-Programm in separatem Speicherbereich.
  SHARED      Startet 16-Bit-Windows-Programm in gemeinsamen Speicherbereich.
  LOW         Startet Anwendung in IDLE-Prioritätsklasse.
  NORMAL      Startet Anwendung in der NORMAL-Prioritätsklasse.
  HIGH        Startet Anwendung in der HIGH-Prioritätsklasse.
  REALTIME    Startet Anwendung in der REALTIME-Prioritätsklasse.
  ABOVENORMAL Startet Anwendung in der ABOVENORMAL-Prioritätsklasse.
  BELOWNORMAL Startet Anwendung in der BELOWNORMAL-Prioritätsklasse.
  WAIT        Startet die Anwendung und wartet auf das Ende.
  Befehl/Programm
              Wenn ein interner Befehl oder eine Batchdatei verwendet wird,
              wird das neue CMD-Fenster mit Parameter /K gestartet.
              Das bedeutet, dass das Fenster erhalten bleibt, nachdem der
              Befehl ausgeführt wurde.
              Wenn dies nicht der Fall ist, wird das Programm entweder
              als Anwendung in einem Fenster oder als Konsolenanwendung
              gestartet.
  Parameter   Parameter, die an den Befehl oder das Programm übergeben werden
              sollen.


Wenn die Befehlserweiterungen aktiviert sind, wird der Aufruf von externen
Befehlen durch die Befehlszeile oder den START-Befehl folgendermaßen verändert:

Nicht-ausführbare Dateien können durch ihre Zuordnung zu einem Dateityp
    einfach durch Eingabe des Namens als Befehl aufgerufen werden (z.B. würde
    WORD.DOC die Anwendung starten, die der .DOC-Erweiterung zugeordnet ist).
    Weitere Informationen, wie Sie diese Zuordnungen innerhalb eines Befehls-
    skripts ändern, finden Sie beim ASSOC- bzw. FTYPE-Befehl.

Wird eine grafische 32-Bit-Anwendung aufgerufen, wartet CMD.EXE nicht auf
    das Beenden dieser Anwendung, sondern kehrt sofort zur Eingabeauforderung
    zurück. Das gilt NICHT, wenn diese Anwendung aus einem Befehlsskript
    heraus aufgerufen wird.

Wenn eine Befehlszeile ausgeführt wird, die mit den Zeichen CMD ohne
    Erweiterung oder Pfadname beginnt, wird CMD durch den Wert der
    COMSPEC-Variablen ersetzt. Damit wird vermieden, dass man versehentlich
    eine andere, ggf. zufällige Version der CMD.EXE verwendet.

Wenn eine Befehlszeile ausgeführt wird, die mit Zeichen beginnt, die keine
    Dateierweiterung enthalten, dann verwendet CMD.EXE den Wert der Umgebungs-
    variablen PATHEXT, um zu bestimmen, welche Erweiterungen in welcher
    Reihenfolge verwendet werden. Der Standardwert für die PATHEXT-Variable
    ist:

        .COM;.EXE;.BAT;.CMD

    Dabei ist die Syntax dieselbe wie in der PATH-Variablen, das heißt, die
    einzelnen Einträge werden durch Semikolon getrennt.

Wenn mit dem START-Befehl ein Programm bzw. Befehl aufgerufen wird und keine
entsprechende Dateierweiterung gefunden wird, aber der Name ohne Erweiterung
einem Verzeichnisnamen entspricht, wird der Explorer mit diesem Pfad
aufgerufen. Von der Befehlszeile ausgeführt entspricht das einem CD /D mit
diesem Pfad.
*/

int32_t VFSCommandSTART::run(vector<string> *paramlist)
{ 
	logPF();
	vector <string> params = *paramlist;
//	vector <string>::iterator it,jt;

	uint32_t i;
	string command;

	for(i=0;i<params.size();i++)
	{
		if (params[i][0] == '\"' )
		{
			logDebug("New commandprompt title is %s\n",params[i].c_str());
			continue;
		}else
		if ( params[i][0] == '/' )
		{
			logDebug("command parameter %s\n",params[i].c_str());
			continue;
		}else
		{
			if (i != params.size())
			{
				command += params[i];
			}
			
		}
	}

	command += " & ";
	m_VFS->addStdIn(&command);
	return 0;
}
