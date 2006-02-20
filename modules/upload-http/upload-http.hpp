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
#include "UploadHandler.hpp"
#include "DNSCallback.hpp"

using namespace std;

namespace nepenthes
{

	class HTTPUploadHandler : public Module , public UploadHandler, public DNSCallback
	{
	public:
		HTTPUploadHandler(Nepenthes *);
		~HTTPUploadHandler();
		bool Init();
		bool Exit();

		bool upload(UploadQuery *up);

		bool dnsResolved(DNSResult *result);
		bool dnsFailure(DNSResult *result);
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;
