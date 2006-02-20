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

#include "submit-xmlrpc.hpp"

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

using namespace std;

namespace nepenthes
{

	struct HTTPHeaderValue
	{
		HTTPHeaderValue(string name, string value);
		~HTTPHeaderValue();
		string m_Name;
		string m_Value;
	};

	class HTTPHeader
	{
	public:
		HTTPHeader(char *header, uint32_t size);
		~HTTPHeader();
		const char *getValue(char *name);
		string getHeader();

	private:
		void parseHeaderLines();

	protected:
		list <HTTPHeaderValue> m_HeaderValues;
		list <string>			m_HeaderLines;



	};

}


