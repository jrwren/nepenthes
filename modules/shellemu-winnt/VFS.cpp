/********************************************************************************
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


#include <ctype.h>

#include "VFS.hpp"
#include "VFSNode.hpp"
#include "VFSDir.hpp"
#include "VFSFile.hpp"

#include "VFSCommandDir.hpp"
#include "VFSCommandECHO.hpp"
#include "VFSCommandREDIR.hpp"
#include "VFSCommandRREDIR.hpp"
#include "VFSCommandTFTP.hpp"
#include "VFSCommandFTP.hpp"
#include "VFSCommandCMD.hpp"
#include "VFSCommandSTART.hpp"
#include "VFSCommandRCP.hpp"

#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_shell

VFS::VFS()
{

	m_StdOut = "";
	
}

VFS::~VFS()
{
	while (m_CommandDirs.size() > 0)
	{
		logSpam("Deleting dir %s \n",m_CommandDirs.front()->getName().c_str());
		m_CommandDirs.pop_front();
	}

	while (m_Nodes.size() > 0)
	{
		logSpam("Deleting Node %s \n",m_Nodes.front()->getPath().c_str());
		delete m_Nodes.front();
		m_Nodes.pop_front();
	}

}

bool VFS::Init(Dialogue *dia)
{
	m_Dialogue = dia;

	VFSDir *cdir = new VFSDir(NULL,(char *)"c:");
	m_Nodes.push_back(cdir);
	VFSDir *wdir = cdir->createDirectory((char *)"WINNT");
	VFSDir *sdir = wdir->createDirectory((char *)"System32");
	m_CurrentDir = sdir;

	m_CommandDirs.push_back(wdir);
	m_CommandDirs.push_back(sdir);

	VFSCommand *vcdir = new VFSCommandDir(sdir, this);
	sdir->createCommand(vcdir);

	VFSCommand *vcecho = new VFSCommandECHO(sdir, this);
	sdir->createCommand(vcecho);

	VFSCommand *vcredir = new VFSCommandREDIR(sdir, this);
	sdir->createCommand(vcredir);

	VFSCommand *vcrredir = new VFSCommandRREDIR(sdir, this);
	sdir->createCommand(vcrredir);


	VFSCommand *vctftp = new VFSCommandTFTP(sdir, this);
	sdir->createCommand(vctftp);
	
	VFSCommand *vcftp = new VFSCommandFTP(sdir, this);
	sdir->createCommand(vcftp);

	VFSCommand *vccmd = new VFSCommandCMD(sdir, this);
	sdir->createCommand(vccmd);

	VFSCommand *vcstart = new VFSCommandSTART(sdir, this);
	sdir->createCommand(vcstart);

	VFSCommand *vcrcp = new VFSCommandRCP(sdir, this);
	sdir->createCommand(vcrcp);

	return true;
}

Dialogue	*VFS::getDialogue()
{
	return m_Dialogue;
}

string VFS::execute(string *input)
{
	if (input->size() <= 0)
		return getDir();

	m_StdIn += *input;

	while(m_StdIn.size() > 0)
	{

		/* beispiel fuer den block 'line reissen'
			"echo foo bar ^> >> foo; echo "bar foo" > foo;"
			wird geparsed als lines
			
			1 "echo foo bar ^> "
			2 ">> foo"
			3 " echo "bar foo" "
			4 "> foo"
		*/
		
		string line;
		uint32_t linelen;
		uint32_t i;
		bool hasredir=false;	// line starts with "> .. "or ">> .."
		bool escaped = false;
		bool haschar = false;

		i=0;
		while (i<m_StdIn.size())
		{

			if ( m_StdIn[i] == '>'  )
			{
				if (escaped == false)
				{
					if ( haschar == true )
					{
						logDebug("breaking here %i line %i \n",i,__LINE__);
						break;
					} else
					{
						hasredir = true;
					}
				}else
				{
					escaped = false;
				}

			}else
			if (m_StdIn[i] == '\n' )
			{
				
				i++;
				logDebug("breaking here %i line %i \n",i,__LINE__);
				break;
			}
			else
			if ( ( m_StdIn[i] == ';' && hasredir == true ) || m_StdIn[i] == '&')
			{
				if (escaped == false)
				{
                    i++;
					logDebug("breaking here %i line %i \n",i,__LINE__);
					break;
				}else
				{
					escaped = false;
				}
			} 
			else
			if (m_StdIn[i] == '^')
			{
				if (escaped == false)
				{
                   	escaped = true;
				}else
				{
					escaped = false;
				}
			}
			else
			{
				haschar=true;
				escaped=false;
			}
				
            i++;
		}

		linelen = i;

		
		line = m_StdIn.substr(0,linelen);
		logDebug("Line (%i) is '%s' \n",line.size(),line.c_str());
		m_StdIn = m_StdIn.substr(linelen,m_StdIn.size() - linelen);
		if (line[line.size()-1] == ';' || line[line.size()-1] == '&')
			line[line.size()-1] = '\0';

		string newline;
		escaped = false;

		i=0;
		int j=0;
		while (i<line.size())
		{
			if ( escaped == true)
			{
				escaped = false;
                newline += line[i];
				j++;
			}else
			{
				if (line[i] == '^')
				{
					escaped = true;
				}else
				{
					newline += line[i];
					escaped = false;
				}
			}
			i++;
		}
			
		logSpam("LINE %s\n",line.c_str());
		logSpam("ESCL %s\n",newline.c_str());

		line = newline;

		/* beispiel fuer den block 'befehl suchen' 
			line ist " echo dong"
			befehl ist "echo"
		*/


		i=0;
		haschar=false;
		uint32_t commandstart=0;
		uint32_t commandstopp=0;
		hasredir=false;
		while (i<line.size())
		{
			if (line[i] == ' ' && haschar == true)
			{
				logDebug("breaking here %i \n",i);
				break;
			}

			if (hasredir == true && line[i] != '>')
			{
				logDebug("breaking here %i \n",i);
				break;
			}

			if (isgraph(line[i]))
			{
				if (haschar == false)
					commandstart = i;

            	haschar=true;
			}

			if (line[i] == '>')
			{
				hasredir = true;
			}

			i++;
		}

		commandstopp=i;
		string command = line.substr(commandstart,commandstopp-commandstart);
		i=0;
		while (i<command.size())
		{
			if (command[i] == '\r' || command[i] == '\n')
				command[i] = '\0';
			i++;
		}
		command = command.c_str();

		logDebug("Command (%i) is '%s'\n",command.size(),command.c_str());

		/* beispiel fuer den block 'parameter finden'

		line ist 'echo "1. foo bar ^" " 2 "3 dong " '
		parameter sind
			'1. foo bar ^" '
			'2'
			'3 dong '
		*/
		string params;
		params = line.substr(commandstopp,line.size()-commandstopp);
		i=0;
		while (i<params.size())
		{
			if (params[i] == '\r' || params[i] == '\n')
				params[i] = '\0';
			i++;
		}
		logDebug("Params (%i)is '%s' \n",params.size(),params.c_str());



		vector <string> paramlist;
		i=0;
		haschar = false;
        uint32_t wordstart=0;
		uint32_t wordstopp=0;

		while(i<=params.size())
		{
			if (( ( params[i] == ' ' || params[i] == '\0') && haschar == true) )
			{
				wordstopp = i;
				string word = params.substr(wordstart,wordstopp-wordstart);
				logDebug("Word is %i %i '%s' \n",wordstart,wordstopp,word.c_str());
				paramlist.push_back(word);
				haschar = false;
			}else
			if (isgraph(params[i]) && haschar == false)
			{
				haschar = true;
				wordstart = i;
			}
			i++;
		}

		/* parameter ersetzen 
		 %SYSTEM% -> c:\WINNT\System32 etc pp
		*/

		/* beispiel parameter unescapen 
		'^" ' ->> '" '
		*/


		/* command suchen und starten mit der parameterliste 
		 * wenn command nicht gefunden schreiben auf m_StdErr
		 *
		 */

		bool foundcommand = false;

		list <VFSDir *>::iterator cdir;
		list <VFSNode *>::iterator cfile;
		for (cdir = m_CommandDirs.begin(); cdir != m_CommandDirs.end() && foundcommand == false; cdir++)
		{
			logSpam("Checking dir %s for command %s \n",(*cdir)->getName().c_str(), command.c_str());
			list <VFSNode *> dirlist = *(*cdir)->getList();
			for (cfile = dirlist.begin(); cfile != dirlist.end() && foundcommand == false; cfile++)
			{

				string altercommand = command + ".exe";
				if ((*cfile)->getType() == VFS_EXE && 
					( strcasecmp((*cfile)->getName().c_str(),command.c_str()) == 0 || 
					  strcasecmp((*cfile)->getName().c_str(),altercommand.c_str()) == 0 )
					)
				{
					logSpam("found command '%s' <-> '%s' \n",(*cfile)->getName().c_str(), command.c_str());
					((VFSCommand *)(*cfile))->run(&paramlist);
                    logSpam("response buffer '%s' (%i) \n",m_StdOut.c_str(),m_StdOut.size());
					foundcommand = true;
				}
			}
		}

		if ( foundcommand == false )
		{

			logSpam("look into current dir %s\n",m_CurrentDir->getName().c_str());
			list <VFSNode *> dirlist = *m_CurrentDir->getList();
			for ( cfile = dirlist.begin(); cfile != dirlist.end() && foundcommand == false; cfile++ )
			{
//				printf("FILE '%s' '%s' \n",command.c_str(), (*cfile)->getName().c_str());
				string altercommand = command + ".bat";
				if  ( 
					   (*cfile)->getType() == VFS_FILE && 
					   ( strcasecmp((*cfile)->getName().c_str(),command.c_str()) == 0 || 
						 strcasecmp((*cfile)->getName().c_str(),altercommand.c_str()) == 0 )
					 )
				{
					logSpam("found command '%s' <-> '%s' \n",(*cfile)->getName().c_str(), command.c_str());
					printf("adding %.*s to stdin\n",(int)((VFSFile *)(*cfile))->getSize(),((VFSFile *)(*cfile))->getData());
					string addme(((VFSFile *)(*cfile))->getData(),((VFSFile *)(*cfile))->getSize());
					m_StdIn += addme;
					logSpam("response buffer '%s' (%i) \n",m_StdOut.c_str(),m_StdOut.size());

					((VFSFile *)(*cfile))->truncateFile();
					foundcommand = true;
				}
			}

		}

		/* m_Stderr auf m_Stdout adden
		 * m_StdOut zurueckgeben
		 */
//		return m_StdOut;
		

	}
	return m_StdOut;
}

string VFS::getDir()
{
//	return m_Current;
	return NULL;
}

string *VFS::getStdOut()
{
	return &m_StdOut;
}

void VFS::addStdOut(string *s)
{
	m_StdOut += *s;
}

void VFS::freeStdout()
{
	m_StdOut = "";
}


string *VFS::getStdIn()
{
	return &m_StdIn;
}

void VFS::addStdIn(string *s)
{
	m_StdIn = *s + m_StdIn;
}


string *VFS::getStdErr()
{
	return &m_StdErr;
}

VFSDir *VFS::getCurrentDir()
{
	return m_CurrentDir;
}
