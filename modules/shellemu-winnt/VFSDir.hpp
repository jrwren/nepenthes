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

#include <list>
#include <string>

#include "VFSNode.hpp"
#include <stdint.h>

using namespace std;

namespace nepenthes
{
	class VFSDir;
	class VFSFile;

	class VFSCommand;

	class VFSDir: public VFSNode
	{
	public:
		VFSDir(VFSNode *parentnode,char *name);
		~VFSDir();
		virtual VFSDir* getDirectory(char *dirname);
		virtual VFSDir *createDirectory(char *dirname);
		virtual VFSFile *getFile(char *filename);

		virtual VFSFile *createFile(char *name, char *data, uint32_t len);
		virtual list <VFSNode *> *getList();
		virtual VFSCommand *createCommand(VFSCommand *command);
	};

}
