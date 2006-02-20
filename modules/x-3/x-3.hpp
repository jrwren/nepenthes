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
	class X3Download;

	/**
	 * X3
	 * eXample DownloadHandler
	 * 
	 * download files from /dev/urandom
	 * 
	 * url is file://localhost:64000//dev/urandom
	 * 
	 * great to debug SubmitHandler
	 * 
	 */
	class X3 : public Module , public DownloadHandler , DialogueFactory
	{
	public:
		X3(Nepenthes *);
		~X3();

		bool Init();
		bool Exit();

		bool download(Download *down);

		Dialogue *createDialogue(Socket *socket);
	private:
		list <X3Download *> m_Downloads;
	};


	/**
	 * X3Dialogue
	 * 
	 * will use a FILESocket to read from /dev/urandom
	 */
	class X3Download : public Dialogue
	{
	public:
		X3Download(Socket *socket);
		~X3Download();

		void setDownload(Download *down);
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
	private:
		Download *m_Download;
	};	

}

extern nepenthes::Nepenthes *g_Nepenthes;
