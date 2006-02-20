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

#ifndef HAVE_VFSNODE_HPP
#define HAVE_VFSNODE_HPP

#include <list>
#include <string>

using namespace std;




typedef enum
{
	VFS_DIR,
	VFS_FILE,
	VFS_EXE
} vfs_type;


namespace nepenthes
{
	class VFSNode;
	class VFSNode
	{
	public:

		virtual ~VFSNode(){};
		string 	getName()
		{
			return m_Name;
		}
		VFSNode * getParent()
		{
			return m_ParentNode;
		}
		vfs_type getType()
		{
			return m_Type;
		}
		string getPath()
		{
			VFSNode *parent = m_ParentNode;
			string path = m_Name;
			while ( parent != NULL )
			{
				path = "\\" + path;
				path = parent->getName() + path;
				parent = parent->getParent();
			}
			return path;
		}
//	virtual string list();
	protected:
		VFSNode         *m_ParentNode;
		list <VFSNode *> m_Nodes;
		vfs_type        m_Type;
		string 			m_Name;

	};

}

#endif
