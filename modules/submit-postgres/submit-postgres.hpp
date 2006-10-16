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
#include "SubmitHandler.hpp"
#include "SQLCallback.hpp"

#include "Download.hpp"
#include "DownloadBuffer.hpp"


#include "PGDownloadContext.hpp"

using namespace std;

namespace nepenthes
{

	class SQLHandler;


	/**
	 * SubmitPostgres
	 */
	class SubmitPostgres : public Module , public SubmitHandler, public SQLCallback
	{
	public:
		SubmitPostgres(Nepenthes *);
		~SubmitPostgres();
		bool Init();
		bool Exit();

		void Submit(Download *down);
		void Hit(Download *down);

		bool sqlSuccess(SQLResult *result);
		bool sqlFailure(SQLResult *result);

		void sqlConnected();
		void sqlDisconnected();

		string getSpoolPath();


	private:
		SQLHandler 			*m_SQLHandler;

		list <PGDownloadContext *> m_OutstandingQueries;

		string m_Server;
		string m_DB;
		string m_User;
		string m_Pass;
		string m_Options;

		string m_SpoolDir;
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;
extern nepenthes::SubmitPostgres *g_SubmitPostgres;
