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

#include "Buffer.hpp"
#include "Buffer.cpp"
#include "VFSFile.hpp"
#include "LogManager.hpp"
#include "Nepenthes.hpp"


using namespace nepenthes;
using namespace std;

VFSFile::VFSFile(VFSNode *parentnode, char *name, char *data, unsigned int len)
{
	m_ParentNode = parentnode;
	if (len == 0)
	m_Buffer = new Buffer(1024);
		else
	m_Buffer = new Buffer(data,len);
	m_Name = name;
	m_Type = VFS_FILE;
}

VFSFile::~VFSFile()
{
	logSpam("Deleting file %s \n",getPath().c_str());
	delete m_Buffer;
}

unsigned int VFSFile::addData(char *data, unsigned int len)
{
	m_Buffer->add(data,len);
	return 0;
}

char *VFSFile::getData()
{
 	return (char *)m_Buffer->getData();
}

void VFSFile::truncateFile()
{
	m_Buffer->clear();
}

unsigned int VFSFile::getSize()
{
	return m_Buffer->getSize();
}
