/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter
 * Copyright (C) 2008  Mark Schloesser
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

#include <ctype.h>

#include "vuln-ms08067.hpp"
#include "ms08067-shellcodes.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"
#include "ShellcodeHandler.hpp"
#include "Utilities.hpp"

#include "Config.hpp"

#include "Download.hpp"

#include "EventManager.hpp"
#include "SocketEvent.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

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
 * creates a new MS08067 Module, 
 * MS08067 is an example for binding a socket & setting up the Dialogue & DialogueFactory
 * 
 * 
 * it can be used as a shell emu to allow trigger commands 
 * 
 * 
 * sets the following values:
 * - m_DialogueFactoryName
 * - m_DialogueFactoryDescription
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
MS08067::MS08067(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-ms08067";
	m_ModuleDescription = "vuln module for ms08-067 vuln";
	m_ModuleRevision    = "$Rev: x $";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "ms08067 Factory";
	m_DialogueFactoryDescription = "vuln-ms08067 Factory";

	g_Nepenthes = nepenthes;
}

MS08067::~MS08067()
{

}


/**
 * Module::Init()
 * 
 * binds the port, adds the DialogueFactory to the Socket
 * 
 * @return returns true if everything was fine, else false
 *         false indicates a fatal error
 */
bool MS08067::Init()
{
	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	StringList sList;
	int32_t timeout;
	try
	{
		sList = *m_Config->getValStringList("vuln-ms08067.ports");
		timeout = m_Config->getValInt("vuln-ms08067.accepttimeout");
	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	uint32_t i = 0;
	while (i < sList.size())
	{
		m_Nepenthes->getSocketMgr()->bindTCPSocket(0,atoi(sList[i]),0,timeout,this);
		i++;
	}
	return true;
}

bool MS08067::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new MS08067Dialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *MS08067::createDialogue(Socket *socket)
{
	return new MS08067Dialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}







/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the MS08067Dialogue, creates a new MS08067Dialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
MS08067Dialogue::MS08067Dialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "MS08067Dialogue";
	m_DialogueDescription = "vuln-ms08067 Dialogue";

	m_ConsumeLevel = CL_UNSURE;
	m_Buffer = new Buffer(1024);
	assembled_dcerpc_data = new Buffer(1024);
	m_State = MS08067_STAGE1;
	stage7_sent_size = 0;
	dcerpc_bind_data_size = 0;
}

MS08067Dialogue::~MS08067Dialogue()
{
	delete m_Buffer;
}

/**
 * Dialogue::checkPacketValidity(Message *)
 * 
 * @param msg the Message the Socket received
 * 
 * @return bool
 */
bool MS08067Dialogue::checkPacketValidity(Message *msg)
{
	bool result = false;
	
	if (msg->getSize() > 32+4 && // netbios and smb header
		((char *)msg->getMsg())[0] == '\x00' && // smb message
		memcmp(smb_header_fix, (char *)msg->getMsg()+4, 4) == 0 && // smb fixed header
		ntohs(* ( (unsigned short *)msg->getMsg()+1 ) ) == (msg->getSize()-4) // message length okay?
	)
		result = true;

	return result;
}

/**
 * Dialogue::checkSMBCommand(Message *, smb_cmdcode)
 * 
 * @param msg the Message the Socket received.
 * @param cmdcode SMB command 
 * 
 * @return bool
 */
bool MS08067Dialogue::checkSMBCommand(Message *msg, smb_cmdcode cmdcode)
{
	if (((char *)msg->getMsg())[8] == cmdcode)
		return true;

	return false;
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * @param msg the Message the Socker received.
 * 
 * @return ConsumeLevel
 */
ConsumeLevel MS08067Dialogue::incomingData(Message *msg)
{

	logPF();

	 m_Buffer->add(msg->getMsg(),msg->getSize());

	 char reply[1024];
	 for (int32_t i=0;i<1024;i++)
	 {
		 reply[i] = rand()%255;
	 }

	 // state manipulation to make some dirty stuff work...

#ifdef REPLAYMODE
 	 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_TREECONNECT_ANDX))
 	 	m_State = MS08067_STAGE4;
#endif
	 
	 // this one makes it possible to have multiple NTCreateAndX Requests in a session
	 if ( m_State > MS08067_STAGE5 && checkPacketValidity(msg) && checkSMBCommand(msg, SMB_NTCREATE_ANDX) )
		m_State = MS08067_STAGE5;

	 // write andx -> stage 9
	 if ( m_State == MS08067_STAGE8 && checkPacketValidity(msg) && checkSMBCommand(msg, SMB_READ_ANDX) )
		m_State = MS08067_STAGE9;

	 // SMB Transaction mode -> Branch to TRANS state
	 if ( m_State == MS08067_STAGE7 && checkPacketValidity(msg) && checkSMBCommand(msg, SMB_TRANS) )
		m_State = MS08067_STAGE8_TRANS;
	 // write andx -> stage 8
	 if ( m_State == MS08067_STAGE7 && checkPacketValidity(msg) && checkSMBCommand(msg, SMB_WRITE_ANDX) )
		m_State = MS08067_STAGE8;

	 // SMB Transaction mode -> Branch to TRANS state
	 if ( m_State == MS08067_STAGE6 && checkPacketValidity(msg) && checkSMBCommand(msg, SMB_TRANS) )
		m_State = MS08067_STAGE6_TRANS;
	 // read andx -> stage 7
	 if ( m_State == MS08067_STAGE6 && checkPacketValidity(msg) && checkSMBCommand(msg, SMB_READ_ANDX) )
		m_State = MS08067_STAGE7;

	 switch (m_State)
	 {
	 case MS08067_STAGE1:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_NEGPROT))
		 {
				 logDebug("Valid MS08-067 Stage #1 (bufsize %i)\n", m_Buffer->getSize());
				 m_State = MS08067_STAGE2;

				 memcpy(reply, negotiate_protocol_response, sizeof(negotiate_protocol_response)-1);
				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);

				string s = string((char *)m_Buffer->getData(), m_Buffer->getSize());

				// overlay on dialect struct
				string s2;
				s2.assign(s, 39, s.size());

				// now we go through the dialects to find our NT LM 0.12
				unsigned short count = 0;
				unsigned int startpos = 0;
				unsigned short ourdialect = 0;

				startpos = s2.find(string("\x02", 1), startpos); // dialect start

				while (startpos < s2.size() && startpos != string::npos)
				{
					count++;
					if (s2.find("\x4e\x54\x20\x4c\x4d\x20\x30\x2e\x31\x32\x00") == startpos +1)
						ourdialect = count-1;

					startpos = s2.find(string("\x02", 1), startpos+1); // dialect start
				}
				 memcpy(reply+37, &ourdialect, 2);

				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,sizeof(negotiate_protocol_response)-1);
				 return m_ConsumeLevel;
		 } else
			return CL_DROP;
		 break;

	 case MS08067_STAGE2:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_SESS_SETUP_ANDX))
		 {
				 logDebug("Valid MS08-067 Stage #2 (bufsize %i)\n", m_Buffer->getSize());

				 if (((char *)msg->getMsg())[37] == '\x75')
				 {
					logDebug("andx treeconnect!!\n");
				 	m_State = MS08067_STAGE5;
					 memcpy(reply, session_setup_response_andx_tree_connect, sizeof(session_setup_response_andx_tree_connect)-1);
					 // copy process and multiplex id
					 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
					 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);
					 m_Buffer->clear();
					 msg->getResponder()->doRespond(reply,sizeof(session_setup_response_andx_tree_connect)-1);
				 } else {
				 	m_State = MS08067_STAGE4;
					 memcpy(reply, session_setup_response, sizeof(session_setup_response)-1);
					 // copy process and multiplex id
					 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
					 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);
					 m_Buffer->clear();
					 msg->getResponder()->doRespond(reply,sizeof(session_setup_response)-1);
				 }

				 return m_ConsumeLevel;
		 }else
			 return CL_DROP;
		 break;

	 case MS08067_STAGE4:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_TREECONNECT_ANDX))
		 {
				 logDebug("Valid MS08-067 Stage #4 (bufsize %i)\n", m_Buffer->getSize());
				 m_State = MS08067_STAGE5;

				 memcpy(reply, treeconnect_response, sizeof(treeconnect_response)-1);
				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);
				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,sizeof(treeconnect_response)-1);
				 return m_ConsumeLevel;
		 }else
			 return CL_DROP;
		 break;

	 case MS08067_STAGE5:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_NTCREATE_ANDX))
		 {
				 logDebug("Valid MS08-067 Stage #5 (bufsize %i)\n", m_Buffer->getSize());
				 m_State = MS08067_STAGE6;

				 memcpy(reply, ntcreate_response, sizeof(ntcreate_response)-1);
				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);
				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,sizeof(ntcreate_response)-1);
				 return m_ConsumeLevel;
		 }else
			 return CL_DROP;
		 break;

	 case MS08067_STAGE6:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_WRITE_ANDX))
		 {
				 logDebug("Valid MS08-067 Stage #6 (bufsize %i)\n", m_Buffer->getSize());

				 memcpy(reply, write_response, sizeof(write_response)-1);
				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);
				 // copy data length
				 memcpy(reply+0x29, ((char *)m_Buffer->getData())+0x41, 2);

 				 dcerpc_bind_data_size += * ( (unsigned short *)((char *)m_Buffer->getData()+0x41));
 				 assembled_dcerpc_data->add((char *)m_Buffer->getData()+67, m_Buffer->getSize()-67);

				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,sizeof(write_response)-1);
				 // search memory for our vulnerable call uuid so we can return CL_ASSIGN

				 string s = string((char *)assembled_dcerpc_data->getData(), assembled_dcerpc_data->getSize());
				 if ( s.find(string(vuln_rpccall_uuid, 16)) != (unsigned int)(-1) )
				 {
						logDebug("This is our UUID bind -> CL_ASSIGN!! \n");
						assembled_dcerpc_data->clear();
						m_ConsumeLevel = CL_ASSIGN;
				 }
				 return m_ConsumeLevel;
		 }else 
			 return CL_DROP;
		 break;

	 case MS08067_STAGE6_TRANS:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_TRANS))
		 {
				 logDebug("Valid MS08-067 Stage #6 [TRANS] (bufsize %i)\n", m_Buffer->getSize());
				 m_State = MS08067_STAGE8_TRANS;

				 memcpy(reply, transaction_bind_ack, sizeof(transaction_bind_ack)-1);
				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);

 				 assembled_dcerpc_data->add((char *)m_Buffer->getData()+67, m_Buffer->getSize()-67);

				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,sizeof(transaction_bind_ack)-1);
				 // search memory for our vulnerable call uuid so we can return CL_ASSIGN

				 string s = string((char *)assembled_dcerpc_data->getData(), assembled_dcerpc_data->getSize());
				 if ( s.find(string(vuln_rpccall_uuid, 16)) != (unsigned int)(-1) )
				 {
						logDebug("This is our UUID bind -> CL_ASSIGN!! \n");
						assembled_dcerpc_data->clear();
						m_ConsumeLevel = CL_ASSIGN;
				 }
				 return m_ConsumeLevel;
		 }else 
			 return CL_DROP;
		 break;

	 case MS08067_STAGE7:
		 /* this is actually an ugly stage... we need a dcerpc bindack packet that is sent over smb.
		  * that packet can be split up into several responses...
		  * currently we compute the ack here - would be better to compute it once and store it.
		  * :(
		  * -- also we have many fixed byte positions that could be made relative to get prettier code...
		 */
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_READ_ANDX))
		 {
				 logDebug("Valid MS08-067 Stage #7 (bufsize %i)\n", m_Buffer->getSize());

				 unsigned short requested_size = * ( (unsigned short *)((char *)m_Buffer->getData()+47));
				 unsigned short to_send = requested_size;
				 unsigned char num_context_items = (dcerpc_bind_data_size-28)/44;
				 unsigned short temp = 0;

				 logDebug("context items received %d\n", num_context_items );

				 memcpy(reply, read_response, sizeof(read_response)-1);

				 //dcerpc_bind_data[] //len: 44; dcerpc_provider_rejection[] //len: 24 dcerpc_acceptance[] //len: 24
				 char calculated_rpc_data[1024];
				 memcpy(calculated_rpc_data, dcerpc_bind_data, 44);
				 temp = (44+(num_context_items*24));
				 memcpy(calculated_rpc_data+8, &temp, 2);
				 memcpy(calculated_rpc_data+40, &num_context_items, 1);
				 for (int i = 0; i < num_context_items-1; i++)
					memcpy(calculated_rpc_data+44+(i*24), dcerpc_provider_rejection, 24);
				 memcpy(calculated_rpc_data+44+((num_context_items-1)*24), dcerpc_acceptance, 24);

				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);

				 // append dcerpc_bind_data portion
				 if (stage7_sent_size < (44+(num_context_items*24)))
				 {
					if (requested_size > ((44+(num_context_items*24))-stage7_sent_size))
						to_send = (44+(num_context_items*24))-stage7_sent_size;
				 	memcpy(reply+64, calculated_rpc_data+stage7_sent_size, to_send);
					stage7_sent_size += to_send;
				 } else {
					m_Buffer->clear();
					logDebug("read andx request but already sent everything.... :(\n");
					return CL_DROP;
				 }

				 // set sent data length in packet
				 memcpy(reply+47, &to_send, 2);
				 temp = htons(60+to_send);
				 memcpy(reply+2, &temp, 2);
				 temp = to_send+1; // because of padding thing...
				 memcpy(reply+61, &temp, 2);

				 m_Buffer->clear();
				 msg->getResponder()->doRespond(reply,64+to_send);
				 return m_ConsumeLevel;
		 }else
			 return CL_DROP;
		 break;

	 case MS08067_STAGE8:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_WRITE_ANDX))
		 {
				 logDebug("Valid MS08-067 Stage #8 (bufsize %i)\n", m_Buffer->getSize());

				 memcpy(reply, write_response, sizeof(write_response)-1);
				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);

				 // copy data length
				 memcpy(reply+0x29, ((char *)m_Buffer->getData())+0x41, 2);

 				 assembled_dcerpc_data->add((char *)m_Buffer->getData()+67, m_Buffer->getSize()-67);
				 m_Buffer->clear();

				 msg->getResponder()->doRespond(reply,sizeof(write_response)-1);
				 return m_ConsumeLevel;
		 }else
			 return CL_DROP;
		 break;

	 case MS08067_STAGE8_TRANS:
		 if (checkPacketValidity(msg) && checkSMBCommand(msg, SMB_TRANS))
		 {
				 logDebug("Valid MS08-067 Stage #8 [TRANS] (bufsize %i)\n", m_Buffer->getSize());
				 m_State = MS08067_STAGE9;

				 memcpy(reply, transaction_call_error_response, sizeof(transaction_call_error_response)-1);
				 // copy process and multiplex id
				 memcpy(reply+30, ((char *)m_Buffer->getData())+30, 2);
				 memcpy(reply+34, ((char *)m_Buffer->getData())+34, 2);

 				 assembled_dcerpc_data->add((char *)m_Buffer->getData()+67, m_Buffer->getSize()-67);
				 m_Buffer->clear();

				 msg->getResponder()->doRespond(reply,sizeof(transaction_call_error_response)-1);
		 }else
			 return CL_DROP;

	 case MS08067_STAGE9:
		 {
		 // final stage -> should have shellcode in assembled_dcerpc_data

			 Message *Msg = new Message((char *)assembled_dcerpc_data->getData(), assembled_dcerpc_data->getSize(), msg->getLocalPort(), msg->getRemotePort(), msg->getLocalHost(), msg->getRemoteHost(), msg->getResponder(), msg->getSocket());
			  //g_Nepenthes->getUtilities()->hexdump((byte *)msg->getMsg(),msg->getSize());
			  sch_result result = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg);
			  delete Msg;
			  if (result == SCH_DONE )
			  {
				  m_State = MS08067_DONE;
				  m_ConsumeLevel = CL_ASSIGN_AND_DONE;
			  }
			  else
			  {
				logDebug("sch_result != SCH_DONE?\n");
			  }
           	  return m_ConsumeLevel;
		 }
		 break;
	 case MS08067_DONE:
		 break;
	 }

	 return CL_UNSURE;
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
ConsumeLevel MS08067Dialogue::outgoingData(Message *msg)
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
ConsumeLevel MS08067Dialogue::handleTimeout(Message *msg)
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
ConsumeLevel MS08067Dialogue::connectionLost(Message *msg)
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
ConsumeLevel MS08067Dialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}




#ifdef WIN32
extern "C" int32_t __declspec(dllexport)  module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#else
extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
#endif

{
	if (version == MODULE_IFACE_VERSION) {
        *module = new MS08067(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
