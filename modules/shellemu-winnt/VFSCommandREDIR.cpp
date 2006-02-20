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
 
#include "VFSCommandREDIR.hpp"
#include "VFSNode.hpp"
#include "VFSDir.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "VFS.hpp"
#include "VFSFile.hpp"

using namespace nepenthes;
using namespace std;

VFSCommandREDIR::VFSCommandREDIR(VFSNode *parent,VFS *vfs)
{
	m_Name =	">";
	m_ParentNode = parent;
	m_Type = VFS_EXE;
	m_VFS = vfs;;
}

VFSCommandREDIR::~VFSCommandREDIR()
{

} 

int VFSCommandREDIR::run(vector<string> *paramlist)
{

	
	vector<string>	params = *paramlist;
	vector<string>::iterator it = params.begin();

	if (params.size() == 0)
	{
		logWarn("%s","VFS Command REDIR with out arg \n");
		return 0;
	}

	VFSFile *file = m_VFS->getCurrentDir()->getFile((char *)&*it->c_str());
	if (file == NULL)
    {
		logInfo("Creating new file '%s' \n",&*it->c_str());
		file = m_VFS->getCurrentDir()->createFile((char *)&*it->c_str(),0,0);
	}else
	{
		logInfo("truncating file '%s' \n",&*it->c_str());
		file->truncateFile();
	}
	
	file->addData((char *)m_VFS->getStdOut()->c_str(),m_VFS->getStdOut()->size());
	file->addData("\n",1);
	logInfo("file is '%.*s' \n",file->getSize(),(char *)file->getData());
	m_VFS->freeStdout();

/*	list <string> slist = *paramlist;
	list <string>::iterator it;
	for (it=slist.begin();it!=slist.end();it++)
	{
		logInfo("REDIR dest '%s' \n",(*it).c_str());
	}
	logInfo("REDIR Content %s\n",m_VFS->getStdOut().c_str());
	*ret = "";
*/
	return 0;
}
