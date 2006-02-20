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
 
#include "VFSCommandDir.hpp"
#include "VFSNode.hpp"
#include "VFSDir.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_shell

using namespace nepenthes;
using namespace std;

VFSCommandDir::VFSCommandDir(VFSNode *parent,VFS *vfs)
{
	m_Name =	"dir";
	m_ParentNode = parent;
	m_Type = VFS_EXE;
}

VFSCommandDir::~VFSCommandDir()
{

} 

int32_t VFSCommandDir::run(vector<string> *paramlist)
{
/*	list <VFSNode *>::iterator node;
	list <VFSNode *> nodelist = *dir->getList();
	string rs;
	for (node = nodelist.begin();node != nodelist.end(); node++)
	{
		logSpam(" %i %s \n",(int32_t)(*node)->getType(), (*node)->getName().c_str());

		switch((*node)->getType())
		{
		case VFS_DIR:
			*ret += "dir  ";
			break;
		case VFS_FILE:
			*ret += "file ";
			break;
		case VFS_EXE:
			*ret += "bin  ";
			break;
		}
		*ret += (*node)->getName();
		*ret += "\n";
	}
*/	
	return 0;
} 

