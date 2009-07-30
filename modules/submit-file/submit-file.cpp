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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "submit-file.hpp"
#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"
#include "DownloadBuffer.hpp"
#include "Config.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sub | l_hlr

Nepenthes *g_Nepenthes;

FileSubmitHandler::FileSubmitHandler(Nepenthes *nepenthes)
{
	m_ModuleName        = "submit-file";
	m_ModuleDescription = "module providing a file to file submitter";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_SubmitterName = "submit-file";
	m_SubmitterDescription = "store with md5sum as name in /tmp";

	g_Nepenthes = nepenthes;
}

FileSubmitHandler::~FileSubmitHandler()
{

}

bool FileSubmitHandler::Init()
{
	logPF();

	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	try
	{
		m_FilePath = m_Config->getValString("submit-file.path");
    } catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_SUBMIT_HANDLER(this);
	return true;
}

bool FileSubmitHandler::Exit()
{
	return true;
}

void FileSubmitHandler::Submit(Download *down)
{
	string path = m_FilePath +  down->getMD5Sum();

	struct stat s;
	int32_t retval;
	if ((retval = stat(path.c_str(),&s)) == 0)
	{
		logInfo("Already knowing file %s %i \n",path.c_str(),down->getDownloadBuffer()->getSize());
    	return;
	}
	switch (errno)
	{
	case ENOENT:
		{
			FILE *f ;
			if ((f = fopen(path.c_str(),"w+")) == NULL)
			{
				logCrit("Could not open file %s .. %s \n",path.c_str(),strerror(errno));
				return;
			}

//			size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
			size_t size;
			if ((size = fwrite(down->getDownloadBuffer()->getData(),down->getDownloadBuffer()->getSize(),1,f)) != 1)
			{
				logCrit("writing to file %s failed %i <-> %i\n",path.c_str(),size,down->getDownloadBuffer()->getSize());
			}
			logDebug("wrote file %s %i to disk \n",path.c_str(),down->getDownloadBuffer()->getSize());
			fclose(f);
			break;
		}
	default:
		logDebug("stat error on file %s (%s) \n",path.c_str(),strerror(errno));
	}
//	m_Nepenthes->getUtilities()->hexdump((byte *)down->getDownloadBuffer()->getData(),down->getDownloadBuffer()->getSize());
}


void FileSubmitHandler::Hit(Download *down)
{
	return;
}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if(version == MODULE_IFACE_VERSION)
	{
		*module = new FileSubmitHandler(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}
