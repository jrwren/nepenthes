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

#ifndef HAVE_PGDOWNLOADCONTEXT_HPP
#define HAVE_PGDOWNLOADCONTEXT_HPP

#include <string>
#include <stdint.h>

#include "Download.hpp"
#include "DownloadBuffer.hpp"

#include <cstdlib>
#include <cstring>

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


	struct benc_key_comp
	{
		bool operator()(string s1, string s2) const
		{
			unsigned int size = s1.size();
			if (s2.size() < size)
				size = s2.size();

			return(memcmp(s1.data(),s2.data(),size) < 0);
		}
	};


	class PGDownloadContext
	{
	public:
		PGDownloadContext(Download *down);

		~PGDownloadContext();
		static PGDownloadContext *unserialize(const char *path);
		uint32_t    serialize();
		bool        remove();
		string      getHashMD5();
		string      getHashSHA512();
		string      *getUrl();
		string 		getRemoteHost();
		string 		getLocalHost();
		string  	*getFileContent();
		uint32_t 	getFileSize();
		pg_submit_state getState();
		void 		setState(pg_submit_state s);

	private:
		PGDownloadContext(string md5, string sha512, string url, string remote, string local, string file, string path);

		string          m_hash_md5;
		string          m_hash_sha512;
		string          m_Url;

		string          m_RemoteHost;
		string          m_LocalHost;

		string          m_FileContent;

		string 			m_FilePath;
		pg_submit_state m_State;
	};
}

#endif
