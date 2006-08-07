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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "SubmitHandler.hpp"
#include "SQLCallback.hpp"

#include "Download.hpp"
#include "DownloadBuffer.hpp"

using namespace std;

namespace nepenthes
{

	class SQLHandler;

	typedef enum {
		PG_NULL,
		PG_SAMPLE_EXISTS,
		PG_SAMPLE_ADD,
		PG_INSTANCE_ADD
	} pg_submit_state;


	class PGDownloadContext
	{
	public:
		PGDownloadContext(Download *down)
		{
			m_hash_md5 		= down->getMD5Sum();
			m_hash_sha512	= down->getSHA512Sum();
			m_Url			= down->getUrl();	

			uint32_t host   = down->getRemoteHost();
			m_RemoteHost    = inet_ntoa(*(struct in_addr *)&host);

			host = down->getLocalHost();
			m_LocalHost		= inet_ntoa(*(struct in_addr *)&host);	

			m_FileContent	= string(down->getDownloadBuffer()->getData(),down->getDownloadBuffer()->getSize());

			m_State = PG_NULL;
		}

		~PGDownloadContext()
		{

		}

		string getHashMD5()
		{
			return m_hash_md5;
		}

		string getHashSHA512()
		{
			return m_hash_sha512;
		}

		string *getUrl()
		{
			return &m_Url;
		}

		string getRemoteHost()
		{
			return m_RemoteHost;
		}

		string getLocalHost()
		{
			return m_LocalHost;
		}

		string 	*getFileContent()
		{
			return &m_FileContent;
		}

		uint32_t getFileSize()
		{
			return m_FileContent.size();
		}

		pg_submit_state getState()
		{
			return m_State;
		}

		void setState(pg_submit_state s)
		{
			m_State = s;
		}

	private:
		string 			m_hash_md5;
		string 			m_hash_sha512;
		string 			m_Url;

		string			m_RemoteHost;
		string			m_LocalHost;

		string 			m_FileContent;
        pg_submit_state m_State;
	};


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

	private:
		SQLHandler 			*m_SQLHandler;

		list <PGDownloadContext *> m_OutstandingQueries;

		string m_Server;
		string m_DB;
		string m_User;
		string m_Pass;
		string m_Options;
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;

