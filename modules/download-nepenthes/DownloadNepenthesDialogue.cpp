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

#include "DownloadNepenthesDialogue.hpp"
#include "download-nepenthes.hpp"

#include "Message.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Download.cpp"
#include "DownloadUrl.cpp"
#include "DownloadBuffer.cpp"

#include "Utilities.hpp"
#include "SubmitManager.hpp"

using namespace nepenthes;



/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the DownloadNepenthesDialogue, creates a new DownloadNepenthesDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
DownloadNepenthesDialogue::DownloadNepenthesDialogue(Socket *socket, DownloadNepenthes *parent)
{
	m_Socket = socket;
    m_DialogueName = "DownloadNepenthesDialogue";
	m_DialogueDescription = "somebody friends wants to send us a file, so should take care of it";

	m_ConsumeLevel = CL_ASSIGN;
	m_DownloadNepenthes = parent;

	m_State = DOWN_N_MD5SUM;

	m_Download = NULL;
}

DownloadNepenthesDialogue::~DownloadNepenthesDialogue()
{
	if (m_Download != NULL)
	{
		delete m_Download;
	}
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * a small and ugly shell where we can use
 * "download protocol://localction:port/path/to/file
 * to trigger a download
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel DownloadNepenthesDialogue::incomingData(Message *msg)
{
	switch (m_State)
	{
	case DOWN_N_MD5SUM:
		if (msg->getMsgLen() == 34)	// sizof md5sum +\r\n
		{
			//check if its a valid md5sum
			int32_t i;
			for (i=0;i<32;i++)
			{
                if (!isalnum(msg->getMsg()[i]))
				{
					logCrit("%s","client send us invalid md5sum, dropping\n");
					return CL_DROP;
				}
			}

			// check if we already got that file
			// int32_t stat(const char *file_name, struct stat *buf);

			string md5sum(msg->getMsg(),msg->getMsgLen());
			md5sum[32] = '\0';
			m_MD5Sum = md5sum;
			string filepath = m_DownloadNepenthes->getFilesPath() + "/" + md5sum;

			struct stat s;
			i=stat((char *)filepath.c_str(),&s);
			if (i != 0 && errno == ENOENT)
			{
				logInfo("client wants to send us a new file (%.*s), going on\n",32,msg->getMsg());
				m_Socket->doRespond("SENDFILE\r\n",strlen("SENDFILE\r\n"));
				m_State = DOWN_N_FILE;
				m_Download = new Download("nepenthes://",0,"nepenthes interfile transferr");
			}else
			{
				logInfo("we already know file %.*s, so we wont get it again\n",32,msg->getMsg());
				return CL_DROP;
			}
		}else
		{
			return CL_DROP;
		}
		break;
	case DOWN_N_FILE:
		m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getMsgLen());
		break;
	}
	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel DownloadNepenthesDialogue::outgoingData(Message *msg)
{
	return m_ConsumeLevel;
}

/**
 * Dialogue::handleTimeout(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel DownloadNepenthesDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionLost(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel DownloadNepenthesDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel DownloadNepenthesDialogue::connectionShutdown(Message *msg)
{
	// the download is done, check if the md5sum matches the md5sum we were given;
	string md5sum = g_Nepenthes->getUtilities()->md5sum(
		m_Download->getDownloadBuffer()->getData(),
		m_Download->getDownloadBuffer()->getLength());

	if (strncmp(m_MD5Sum.c_str(),md5sum.c_str(),32) != 0)
	{
		logInfo("file does not match its md5sum (%s <-> %s) \n",md5sum.c_str(),m_MD5Sum.c_str());
	}else
	{
		logInfo("new file %s is done\n",m_MD5Sum.c_str());
		g_Nepenthes->getSubmitMgr()->addSubmission(m_Download);
	}
	return CL_DROP;
}
