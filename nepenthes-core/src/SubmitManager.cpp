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

#ifdef WIN32

#else
#include <dirent.h>
#endif
	   
#include "SubmitManager.hpp"
#include "SubmitHandler.hpp"
#include "Nepenthes.hpp"
#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "Nepenthes.hpp"
#include "Utilities.hpp"

#include "LogManager.hpp"
#include "Config.hpp"

#include "EventManager.hpp"
#include "SubmitEvent.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sub | l_mgr

SubmitManager::SubmitManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}

SubmitManager::~SubmitManager()
{
}

bool SubmitManager::Init()
{
#ifdef WIN32
#else	
	m_MagicCookie = magic_open(MAGIC_CONTINUE|MAGIC_PRESERVE_ATIME);
	magic_load(m_MagicCookie,NULL);
#endif

	string FilesDir;
	
	try
	{
		if ( m_Nepenthes->getConfig()->getValInt("nepenthes.submitmanager.strictfiletype")==1 )
			m_StrictFileType = true;

		FilesDir = m_Nepenthes->getConfig()->getValString("nepenthes.submitmanager.filesdir");

    } catch ( ... ) {
        logCrit("%s","Could not find value in config file\n");
        return false;
    }


#ifdef WIN32

#else
	DIR *dirfiles = opendir(FilesDir.c_str());
	if (dirfiles == NULL)
	{
		logCrit("Could not open %s \n%s\n",FilesDir.c_str(),strerror(errno));
		m_Nepenthes->stop();
		return false;
	}

    struct dirent *dent=NULL;
    for (dent = readdir(dirfiles); dent != NULL; dent = readdir(dirfiles))
    {
        if ( (int32_t)dent->d_type == DT_REG)
        {
			if (strlen(dent->d_name) == 32)
			{
				logSpam("Adding %s to known files list \n",dent->d_name);
				string hash = dent->d_name;
				m_FileHashes.push_back(hash);
			}
		}
	}
	closedir(dirfiles);
#endif

    return true;
}

bool SubmitManager::Exit()
{

#ifdef WIN32
#else
	magic_close(m_MagicCookie);
#endif

	return true;
}

/**
 * gives a Download to add known SubmitHandlers
 * deletes the Download after that? FIXMEFIXME
 * 
 * @param down   the Download we want to sumbit
 */
void SubmitManager::addSubmission(Download *down)
{
//	logPF();

	if (down->getDownloadBuffer() == NULL || down->getDownloadBuffer()->getLength() == 0)
	{
		logWarn("download %s has 0 bytes size \n",down->getUrl().c_str());
		return;
	}

	string md5sum = m_Nepenthes->getUtilities()->md5sum(down->getDownloadBuffer()->getData(),
														down->getDownloadBuffer()->getLength());
	down->setMD5Sum(&md5sum);
//	logInfo("Submission has md5sum %s (%i bytes)\n",down->getMD5Sum().c_str(),
//													down->getDownloadBuffer()->getLength());


	unsigned char sha512[64];
	g_Nepenthes->getUtilities()->sha512((unsigned char *)down->getDownloadBuffer()->getData(),
										down->getDownloadBuffer()->getLength(),
										sha512);
	down->setSHA512(sha512);

// check file type
#ifdef WIN32

#else
	const char *filetype = magic_buffer(m_MagicCookie,
										down->getDownloadBuffer()->getData(),
										down->getDownloadBuffer()->getLength());

	down->setFileType((char *)filetype);

	logInfo("File %s has type %s \n",down->getMD5Sum().c_str(),filetype);
	if ( filetype != NULL )
	{
		if (strstr(filetype,"MS-DOS executable") == NULL && m_StrictFileType == true )
		{
			logWarn("dropping file %s as it is not executable\n",down->getMD5Sum().c_str());
			return;
		}
	}else
	{
		if (m_StrictFileType == true )
		{
			logWarn("dropping file %s as it has no filetype\n",down->getMD5Sum().c_str());
			return;
		}
	}
#endif

	SubmitEvent se(EV_SUBMISSION,down);

	m_Nepenthes->getEventMgr()->handleEvent(&se);

	bool knownfile=false;
	list <string>::iterator hash;
	for(hash = m_FileHashes.begin();hash != m_FileHashes.end();hash++)
	{
		if (*hash == md5sum)
		{
			knownfile=true;
		}
	}

	if ( knownfile == false )
	{
		m_FileHashes.push_back(md5sum);

		list <SubmitHandler *>::iterator submitter;
		for ( submitter = m_Submitters.begin();submitter != m_Submitters.end();submitter++ )
		{
			(*submitter)->Submit(down);
		}
	}else
	{
		list <SubmitHandler *>::iterator submitter;
		for ( submitter = m_Submitters.begin();submitter != m_Submitters.end();submitter++ )
		{
			(*submitter)->Hit(down);
		}
	}

}

/**
 * register a SubmitHandler
 * 
 * @param handler the ptr to the SubmitHandler to register
 * 
 * @return returns true if the handler could be registerd, 
 *         false if the handler was already registerd
 */
bool SubmitManager::registerSubmitter(SubmitHandler *handler)
{
	logPF();
	m_Submitters.push_back(handler);
	return true;
}

void SubmitManager::doList()
{
	list <SubmitHandler *>::iterator submitter;
	logInfo("=--- %-69s ---=\n","SubmitManager");
	int32_t i=0;
	for(submitter = m_Submitters.begin();submitter != m_Submitters.end();submitter++,i++)
	{
		logInfo("  %i) %-8s %s\n",i,(*submitter)->getSubmitterName().c_str(), (*submitter)->getSubmitterDescription().c_str());
	}
    logInfo("=--- %2i %-66s ---=\n\n",i, "Submit Handlers registerd");
}
