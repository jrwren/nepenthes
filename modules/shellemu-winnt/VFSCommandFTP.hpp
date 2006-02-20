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

#include "VFSCommand.hpp"

namespace nepenthes
{
	typedef enum 
	{
		NEXT_IS_SOMETHING,
		NEXT_IS_HOST,
		NEXT_IS_PORT,
		NEXT_IS_USER,
		NEXT_IS_PASS,
		NEXT_IS_FILE
	} ftp_command_state;

	class VFSCommandFTP : public VFSCommand
	{
	public:
		VFSCommandFTP(VFSNode *parent,VFS *vfs);
		~VFSCommandFTP();
    	int run(vector<string> *paramlist);
	};
}
