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

#include "VFSCommandTFTP.hpp"
#include "VFSNode.hpp"
#include "VFSDir.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "VFS.hpp"
#include "DownloadManager.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

using namespace nepenthes;
using namespace std;

VFSCommandTFTP::VFSCommandTFTP(VFSNode *parent,VFS *vfs)
{
	m_Name =	"tftp.exe";
	m_ParentNode = parent;
	m_Type = VFS_EXE;
	m_VFS = vfs;
}

VFSCommandTFTP::~VFSCommandTFTP()
{

} 

int VFSCommandTFTP::run(vector<string> *paramlist)
{ // "tftp.exe -i 84.60.21.184 get IExplore327.exe"

	vector <string> slist = *paramlist;


	string ip = slist[1];
	string file = slist[3];
	
	string url = "tftp://";
	url += ip;
	url += "/";
	url += file;

	logInfo("vfs command %s \n",url.c_str());

	unsigned long remotehost=0;
	if (m_VFS->getDialogue()->getSocket() != NULL)
	{
		remotehost = m_VFS->getDialogue()->getSocket()->getRemoteHost();
	}
	g_Nepenthes->getDownloadMgr()->downloadUrl((char *)url.c_str(),remotehost,(char *)url.c_str());
    return 0;
}
