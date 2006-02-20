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


#include "VFSDir.hpp"
#include "VFSFile.hpp"
#include "LogManager.hpp"
#include "Nepenthes.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_shell

using namespace nepenthes;

VFSDir::VFSDir(VFSNode *parentnode, char *name)
{
	m_ParentNode = parentnode;
	m_Name 	= name;
	m_Type = VFS_DIR;
	VFSNode *parent = m_ParentNode;
	string path = name;
	while (parent != NULL)
	{
		path = "\\" + path;
		path = parent->getName() + path;
		parent = parent->getParent();
	}
	logSpam(" created dir %s \n",path.c_str());
	
}

VFSDir::~VFSDir()
{
	// clear list
	while (m_Nodes.size() > 0)
	{
		logSpam("Deleting Node %s \n",m_Nodes.front()->getPath().c_str());
		delete m_Nodes.front();
		m_Nodes.pop_front();
	}

}



VFSDir* VFSDir::createDirectory(char *dirname)
{
	VFSDir *dir = new VFSDir(this,dirname);
	m_Nodes.push_back(dir);
	return dir;
}

VFSDir* VFSDir::getDirectory(char *dirname)
{
	list <VFSNode *>::iterator it;
	for (it = m_Nodes.begin();it != m_Nodes.end();it++)
	{
		if ((*it)->getType() == VFS_DIR && strcasecmp((*it)->getName().c_str(),dirname) == 0)
		{
			return (VFSDir *)(*it);
		}
	}
	return NULL;
}


VFSFile *VFSDir::createFile(char *name,char *data, uint32_t len)
{
	VFSFile *file = new VFSFile(this,name,data,len);
	m_Nodes.push_back(file);
	return file;
}

VFSFile* VFSDir::getFile(char *filename)
{
	list <VFSNode *>::iterator it;
	for (it = m_Nodes.begin();it != m_Nodes.end();it++)
	{
		if ((*it)->getType() == VFS_FILE && strcasecmp((*it)->getName().c_str(),filename) == 0)
		{
			return (VFSFile *)(*it);
		}
	}
	return NULL;
}


VFSCommand *VFSDir::createCommand(VFSCommand *command)
{
	m_Nodes.push_back((VFSNode *)command);
	return command;
}

list <VFSNode *> *VFSDir::getList()
{
	return &m_Nodes;
}
