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


#include <fcntl.h>

#include "x-3.hpp"

#include "FILESocket.hpp"
#include "Download.hpp"
#include "DownloadUrl.hpp"
#include "DownloadBuffer.hpp"
#include "Message.hpp"
#include "LogManager.hpp"
#include "SubmitManager.hpp"


using namespace nepenthes;

/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;



/**
 * The Constructor
 * creates a new X3 Module, 
 * X3 is an example for a DownloadHandler 
 * who can "download" files from /dev/urandom
 * 
 * it uses the same dialogue/dialoguefactory techniques 
 * used in the download-tftp or download-csend downloadhandler
 * 
 * sets the following values:
 * - m_DownloadHandlerName
 * - m_DownloadHandlerDescription
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
X3::X3(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-3";
	m_ModuleDescription = "eXample Module 3 -download handler example-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DownloadHandlerName ="urandom download handler";
	m_DownloadHandlerDescription = "'download' files from /dev/urandom";

	m_DialogueFactoryName = "X3Dialogue";
	m_DialogueFactoryDescription = "creates a dialogue to download a file from a file";

	g_Nepenthes = nepenthes;
}


X3::~X3()
{
	logPF();
}


/**
 * Module::Init()
 * register as a DownloadHandler
 * 
 * @return true if everything was fine, else false
 */
bool X3::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	REG_DOWNLOAD_HANDLER(this,"file");
	return true;
}


/**
 * Module::Exit
 * unregister as a DownloadHandler
 * 
 * @return true if everything was fine, else false
 *         false indicates a fatal error
 */
bool X3::Exit()
{

	return true;
}


/**
 * DownloadHandler::download(Download *down)
 * 
 * whenever a file has to be downloaded using our protocol handle, 
 * this method will get the file.
 * 
 * creates a Socket, and adds the X3Dialogue to this Socket
 * 
 * @param down   the Download info for the file we got to pull
 * 
 * @return returns true if everything was fine, else false
 */
bool X3::download(Download *down)
{
	Socket *socket = m_Nepenthes->getSocketMgr()->openFILESocket((char *)down->getDownloadUrl()->getPath().c_str(),O_RDONLY);
	Dialogue *xdown = createDialogue(socket);
	((X3Download *)xdown)->setDownload(down);
	socket->addDialogue(xdown);

	return true;
}


/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new X3Download
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *X3::createDialogue(Socket *socket)
{
	return new X3Download(socket);
}




/**
 * constructor for the X3Download
 * 
 * sets ConsumeLevel CL_ASSIGN as it is usre its the only handler for the Socket
 * 
 * @param socket the socket the Dialogue has to use
 */
X3Download::X3Download(Socket *socket)
{
	m_DialogueName = "X3";
	m_DialogueDescription = "eXample 3 Dialogue - download a file from a file";

	m_Socket = socket;
	m_ConsumeLevel = CL_ASSIGN;
}


X3Download::~X3Download()
{
	logPF();
}


/**
 * to be able to store the Download * with the X3Download (a Dialogue)
 * we need this 
 * 
 * sets the protected m_Download to the given value
 * 
 * @param down   the Download * connected with this Dialogue
 */
void X3Download::setDownload(Download *down)
{
	m_Download = down;
}


/**
 * Dialogue::incomingData(Message *msg)
 * 
 * called whenever data is avalible on the Socket
 * while the Downloaded Files size is smaller than our limit, 
 * add it to the Buffer and return CL_ASSIGN,
 * else the file is ready, and we add it as a Submission
 * 
 * @param msg    the Message the Socket recv()'d
 * 
 * @return returns CL_ASSIGN while downloadsize is smaller than max_size
 *         else CL_DROP
 */
ConsumeLevel X3Download::incomingData(Message *msg)
{
	logInfo("read %i bytes from %s \n",msg->getMsgLen(), m_Download->getDownloadUrl()->getPath().c_str());
	m_Download->getDownloadBuffer()->addData(msg->getMsg(),msg->getMsgLen());
	if(m_Download->getDownloadBuffer()->getLength() < m_Download->getDownloadUrl()->getPort())
        return CL_ASSIGN;

	msg->getSocket()->getNepenthes()->getSubmitMgr()->addSubmission(m_Download);

	return CL_DROP;
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
ConsumeLevel X3Download::outgoingData(Message *msg)
{
	return CL_DROP;
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

ConsumeLevel X3Download::handleTimeout(Message *msg)
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

ConsumeLevel X3Download::connectionLost(Message *msg)
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

ConsumeLevel X3Download::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new X3(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
