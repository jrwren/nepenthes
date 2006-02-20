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

#ifndef HAVE_SUBMITXMLRPC_HPP
#define HAVE_SUBMITXMLRPC_HPP

#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "SubmitHandler.hpp"

#include "DNSCallback.hpp"

#include "DNSResult.hpp"



#include "EventHandler.hpp"
#include "GeoLocationCallback.hpp"
#include "Utilities.hpp"

#include "UploadCallback.hpp"


using namespace std;

namespace nepenthes
{
#ifdef HAVE_GEOLOCATION
	class SubmitXMLRPC : public Module , public SubmitHandler, public GeoLocationCallback, public UploadCallback
#else
	class SubmitXMLRPC : public Module , public SubmitHandler, public UploadCallback
#endif
	{
	public:
		SubmitXMLRPC(Nepenthes *);
		~SubmitXMLRPC();
		bool Init();
		bool Exit();

		void Submit(Download *down);
		void Hit(Download *down);

#ifdef HAVE_GEOLOCATION
		void locationSuccess(GeoLocationResult *result);
		void locationFailure(GeoLocationResult *result);
#endif

		void uploadSuccess(UploadResult *up);
		void uploadFailure(UploadResult *up);

	protected:
		string m_XMLRPCServer;
	};
}
extern nepenthes::Nepenthes *g_Nepenthes;
extern nepenthes::SubmitXMLRPC *g_SubmitXMLRPC;

#endif
