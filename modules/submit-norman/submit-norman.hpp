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
#include "SubmitHandler.hpp"
#include "EventHandler.hpp"
#include "EventHandler.cpp"

using namespace std;

namespace nepenthes
{
	class NormanContext
	{
	public:
		NormanContext(char *email,string filename, unsigned int filesize, char *filebuffer, char *md5sum)
		{
			m_Email = email;
			m_FileName = filename;
			m_FileSize = filesize;
			m_FileBuffer = (char *) malloc(filesize); 
			m_MD5Sum = md5sum;
			memcpy((char *)m_FileBuffer,(char *)filebuffer,filesize);

			m_FormPost = NULL;
			m_LastPtr = NULL;
			m_HeaderList = NULL;

			/* email field */
			curl_formadd(&m_FormPost,
						 &m_LastPtr,
						 CURLFORM_COPYNAME, "email",
						 CURLFORM_CONTENTTYPE, "form-data",
						 CURLFORM_COPYCONTENTS, email, // FIXME
						 CURLFORM_END);

			string name = "nepenthes-";
			name += md5sum;
			name += "-";
			name += filename;


			curl_formadd(&m_FormPost,
						 &m_LastPtr,
						 CURLFORM_COPYNAME, "upfile",
						 CURLFORM_BUFFER, name.c_str(),
						 CURLFORM_BUFFERPTR, m_FileBuffer,
						 CURLFORM_BUFFERLENGTH, filesize,
						 CURLFORM_END);

			char buf[] = "Expect:";
			m_HeaderList = curl_slist_append(m_HeaderList, buf);

		}
		~NormanContext()
		{
			free(m_FileBuffer);
			curl_formfree(m_FormPost);
        	curl_slist_free_all(m_HeaderList);

		}
		char *getEmail()
		{
			return (char *)m_Email.c_str();
		}
		char *getFileName()
		{
			return (char *)m_FileName.c_str();
		}
		unsigned int getFileSize()
		{
			return m_FileSize;
		}
		char *getMD5Sum()
		{
			return (char *)m_MD5Sum.c_str();
		}
		char *getFileBuffer()
		{
			return m_FileBuffer;
		}


		struct curl_httppost *m_FormPost;
		struct curl_httppost *m_LastPtr;
		struct curl_slist *m_HeaderList;

	protected:
		string m_Email;
		string m_FileName;
		char *m_FileBuffer;
		unsigned int m_FileSize;
		string m_MD5Sum;
	};


	class SubmitNorman : public Module , public SubmitHandler, public EventHandler
	{
	public:
		SubmitNorman(Nepenthes *);
		~SubmitNorman();
		bool Init();
		bool Exit();

		void Submit(Download *down);
		void Hit(Download *down);
		unsigned int handleEvent(Event *event);
		static size_t WriteCallback(char *buffer, size_t size, size_t nitems, void *userp);
	protected:
		CURLM * m_CurlStack;
		int 	m_Queued;
		string m_Email;
	};
}

extern nepenthes::Nepenthes *g_Nepenthes;

