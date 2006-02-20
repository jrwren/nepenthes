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

#ifndef HAVE_DOWNLOADMANAGER_HPP
#define HAVE_DOWNLOADMANAGER_HPP

#ifdef WIN32

#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <string>
#include <list>



#include "Manager.hpp"

#define REG_DOWNLOAD_HANDLER(handler,protocol) g_Nepenthes->getDownloadMgr()->registerDownloadHandler(handler,protocol)

using namespace std;

namespace nepenthes
{

	class DownloadHandler;
	class Nepenthes;
	class Download;
	class DownloadCallback;

	typedef struct
    {
        uint32_t m_ulAddress;
        uint32_t m_ulMask;
    } ip_range_t;

    struct DownloadHandlerTuple
    {
        DownloadHandler * m_Handler;
        string m_Protocol;
    };


	
	/**
	 * DownloadManager
	 * if you want to download something, ask for his assistance
	 * if you want the downloads result, provide a downloadcallback
	 */
    class DownloadManager : public Manager
    {
    public:
        DownloadManager(Nepenthes *nepenthes);
        virtual ~DownloadManager();
		bool isLocalAddress(uint32_t ulAddress);
        virtual bool downloadUrl(Download *down);  
        virtual bool downloadUrl(uint32_t localhost, char *url, uint32_t address, char *triggerline, uint8_t downloadflags, DownloadCallback *callback=NULL, void *obj=NULL);
		virtual bool downloadUrl(uint32_t localhost, char *proto, char *user, char *pass, char *host, char *port, char *file, uint32_t address, uint8_t downloadflags);

        virtual bool registerDownloadHandler(DownloadHandler * handler, const char * protocol);
        virtual void unregisterDownloadHandler(const char * protocol);

		bool Init();
		bool Exit();
		void doList();
    protected:
        list <DownloadHandlerTuple> m_DownloadHandlers;
		static ip_range_t m_irLocalRanges[];
		bool 	m_ReplaceLocalIps;
	};

}

#endif
