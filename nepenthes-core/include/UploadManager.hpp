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

#ifndef HAVE_UPLOADMANAGER_HPP
#define HAVE_UPLOADMANAGER_HPP




#include <string>
#include <list>
#include <stdint.h>



#include "Manager.hpp"

#define REG_UPLOAD_HANDLER(handler,protocol) g_Nepenthes->getUploadMgr()->registerUploadHandler(handler,protocol)

using namespace std;

namespace nepenthes
{
	class UploadQuery;
	class UploadHandler;
	class UploadCallback;

	struct UploadHandlerTuple
    {
        UploadHandler * m_Handler;
        string m_Protocol;
    };

	/**
	 * sometimes we have to upload data
	 * the UploadManager provides a interface for everybody who wants to upload something.
	 * UploadHandler 's register here, and provide support for different protocols.
	 * 
	 * the UploadManager will give the new created UploadQuery to the fitting UploadHandler and
	 * the UploadHandler will call the UploadQueries UploadCallback when he is done.
	 */
	class UploadManager : public Manager
    {
    public:
        UploadManager(Nepenthes *nepenthes);
        virtual ~UploadManager();
        virtual bool uploadUrl(UploadQuery *up);  
		virtual bool uploadUrl(char *url, char *payload, uint32_t playloadlen, UploadCallback *callback=NULL, void *obj=NULL);
        virtual bool registerUploadHandler(UploadHandler *handler, const char * protocol);
        virtual void unregisterUploadHandler(const char *protocol);

		bool Init();
		bool Exit();
		void doList();
    protected:
        list <UploadHandlerTuple> m_UploadHandlers;
	};
}

#endif
