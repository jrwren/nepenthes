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

#ifndef HAVE_VFS_HPP
#define HAVE_VFS_HPP


#include <list>
#include <string>

#include "Dialogue.hpp"

using namespace std;


namespace nepenthes
{

	class VFSNode;
	class VFSDir;
	

	class VFS
	{
	public:
		VFS();
		~VFS();
		bool Init(Dialogue *dia);
		string getDir();
		string execute(string *s);

		string *getStdOut();
        void 	addStdOut(string *s);
		void 	freeStdout();

		string *getStdIn();
		void 	addStdIn(string *s);
		
		string *getStdErr();

		Dialogue	*getDialogue();
		VFSDir 		*getCurrentDir();
	protected:
		list <VFSNode *> m_Nodes;
		list <VFSDir *> m_CommandDirs;
		VFSDir     *m_CurrentDir;

		string      m_StdIn;
		string 		m_StdOut;
		string 		m_StdErr;
		Dialogue	*m_Dialogue;
	};

}

#endif
