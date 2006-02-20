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

#include <curl/curl.h>
#include <curl/types.h> /* new for v7 */
#include <curl/easy.h> /* new for v7 */

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "EventHandler.hpp"
#include "EventHandler.cpp"
#include "DownloadHandler.hpp"
#include "DownloadBuffer.hpp"
#include "Download.hpp"

using namespace std;

namespace nepenthes
{

	class CurlDownloadHandler : public Module , public EventHandler, public DownloadHandler
	{
	public:
		CurlDownloadHandler(Nepenthes *);
		~CurlDownloadHandler();
		bool Init();
		bool Exit();
		bool download(Download *down);
		unsigned int handleEvent(Event *event);

		static size_t WriteCallback(char *buffer, size_t size, size_t nitems, void *userp);
	protected:
		CURLM * m_CurlStack;
		int 	m_Queued;
		unsigned int m_MaxFileSize;
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;
