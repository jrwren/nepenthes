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

#ifndef HAVE_TFTPDOWNLOADHANDLER_HPP
#define HAVE_TFTPDOWNLOADHANDLER_HPP

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "DownloadHandler.hpp"
#include "DownloadManager.hpp"
#include "Dialogue.hpp"
#include "DialogueFactory.hpp"

using namespace std;

namespace nepenthes
{
	class TFTPDialogue;

	class TFTPDownloadHandler : public Module , public DownloadHandler , DialogueFactory
	{
	public:
		TFTPDownloadHandler(Nepenthes *);
		~TFTPDownloadHandler();

		bool Init();
		bool Exit();

		bool download(Download *down);

		Dialogue *createDialogue(Socket *socket);
	private:
		list <TFTPDialogue *> m_Downloads;
		unsigned int m_MaxFileSize;
		unsigned int m_MaxResends;
	};


	

}
extern nepenthes::Nepenthes *g_Nepenthes;
#endif
