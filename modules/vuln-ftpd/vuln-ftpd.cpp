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

/* vuln-ftp written by Harald Lampesberger, contact harald.lampesberger@fork.at 
 * thx to the developers of nepenthes for the help! */

#include <ctype.h>

#include "vuln-ftpd.hpp"

#include "SocketManager.hpp"

#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "ShellcodeManager.hpp"

#include "Config.hpp"

#include "Download.hpp"

#include "Utilities.hpp"

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
 * creates a new FTPd Module, 
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
FTPd::FTPd(Nepenthes *nepenthes)
{
	m_ModuleName        = "vuln-ftp";
	m_ModuleDescription = "vuln-ftp simulates known vulnerabilities of some wellknown win32 ftp servers";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "vuln-ftp Factory";
	m_DialogueFactoryDescription = "FTPd Dialogue Factory";

	g_Nepenthes = nepenthes;
}

FTPd::~FTPd()
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
bool FTPd::Init()
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
		sList = *m_Config->getValStringList("vuln-ftp.ports");
		timeout = m_Config->getValInt("vuln-ftp.accepttimeout");
	} catch ( ... )
	{
		logCrit("Error setting needed vars, check your config\n");
		return false;
	}

	uint32_t i = 0;
	while ( i < sList.size() )
	{
		m_Nepenthes->getSocketMgr()->bindTCPSocket(0,atoi(sList[i]),0,timeout,this);
		i++;
	}
	return true;
}

bool FTPd::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new FTPdDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *FTPd::createDialogue(Socket *socket)
{
	return new FTPdDialogue(socket);
}







/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the FTPdDialogue, creates a new FTPdDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
FTPdDialogue::FTPdDialogue(Socket *socket)
{
	m_Socket = socket;
	m_DialogueName = "FTPdDialogue";
	m_DialogueDescription = "Dialogue to emulate various ftp daemon bugs";

	m_ConsumeLevel = CL_ASSIGN;
	m_Shellcode = new Buffer(4096);
	m_Buffer = new Buffer(4096);
	const char * banner1 = "220 ---freeFTPd 1.0---warFTPd 1.65---\r\n";    
	m_Socket->doRespond(banner1, strlen(banner1));
	m_state = FTP_NULL;
}

FTPdDialogue::~FTPdDialogue()
{
	delete m_Shellcode;
	delete m_Buffer;
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel FTPdDialogue::incomingData(Message *msg)
{
	const char* s_quit                = "221-Quit.\r\n221 Goodbye!\r\n";
	const char* s_user_ok             = "331 User OK, Password required\r\n";
	//char* s_unknown_command  	= "500-Unknown Command\r\n";
	const char* s_server_error        = "501 Server Error\r\n";
	const char* s_not_logged_in       = "530 You are not logged in\r\n";
	const char* s_auth_failed         = "530 Authentication failed, sorry\r\n";

	const char* cmd_user = "USER";
	const char* cmd_pass = "PASS";
	const char* cmd_quit = "QUIT";

	uint32_t threshold = 40;

	ConsumeLevel retval = CL_ASSIGN;
	ftp_exploit exploit_id;


	m_Buffer->add(msg->getMsg(),msg->getSize());

	// check, if last char of the Buffer equals \n

	uint32_t i = 0;
	bool buffercut=false;

//	g_Nepenthes->getUtilities()->hexdump((byte *) m_Buffer->getData(),m_Buffer->getSize());
	while ( i < m_Buffer->getSize() )
	{
		buffercut = false;
		if ( i > 0 && *((char *)m_Buffer->getData()+i) == '\n' )
		{
			string line((char *)m_Buffer->getData(), i);
			
			buffercut=true;
			m_Buffer->cut(i+1);

			i=0;
			

			switch ( m_state )
			{
			case FTP_NULL:
				if ( line.size () >  sizeof(cmd_user) && memcmp(line.c_str(), cmd_user, sizeof(cmd_user)) == 0 )
				{
					//user has sent data starting with cmd_user
					if ( line.size() > threshold )
					{
						//possible exploit was found
						logSpam("Recieved possible Exloit in USER field\n");

						// identify exploit
						exploit_id = identExploit(line);

						m_Shellcode->add((char *)line.c_str(), line.size());


						//logSpam ("Dump: \n %s\n", line.c_str());
						Message *Msg = new Message((char *)line.c_str(), line.size(),
												   m_Socket->getLocalPort(), 
												   m_Socket->getRemotePort(),m_Socket->getLocalHost(), 
												   m_Socket->getRemoteHost(), m_Socket, m_Socket);

						sch_result sch = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg, this->identExploitString(exploit_id));

						delete Msg;

						if ( sch == SCH_DONE )
						{
							m_state = FTP_DONE;
							retval =  CL_ASSIGN_AND_DONE;
						}
					} else
					{
						// username ok
						msg->getResponder()->doRespond(s_user_ok, strlen(s_user_ok));
						m_state = FTP_USER;
					}

				} else
				{
					//user has sent unusable junk
					msg->getResponder()->doRespond(s_not_logged_in, strlen(s_not_logged_in));

				}
				break;



			case FTP_USER:
				if ( line.size () >  sizeof(cmd_pass) && memcmp(line.c_str(), cmd_pass, sizeof(cmd_pass)) == 0 )
				{
					//user has sent data starting with cmd_pass
					if ( line.size() > threshold )
					{
						//possible exploit was found
						logSpam("Recieved possible Exloit in PASS field\n");

						// identify exploit
						exploit_id = identExploit(line);

						m_Shellcode->add((char *)line.c_str(), line.size());

						Message *Msg = new Message((char *)line.c_str(), line.size(),
												   m_Socket->getLocalPort(), 
												   m_Socket->getRemotePort(),m_Socket->getLocalHost(), 
												   m_Socket->getRemoteHost(), m_Socket, m_Socket);

						sch_result sch = g_Nepenthes->getShellcodeMgr()->handleShellcode(&Msg, this->identExploitString(exploit_id));

						delete Msg;
						if ( sch == SCH_DONE )
						{
							m_state = FTP_DONE;
							retval =  CL_ASSIGN_AND_DONE;
						}
					} else
					{
						// password-format ok, user does not get logged in ;)
						msg->getResponder()->doRespond(s_auth_failed, strlen(s_auth_failed));
						m_state = FTP_PASS;
					}

				} else
				{
					//user has sent unusable junk
					msg->getResponder()->doRespond(s_not_logged_in, strlen(s_not_logged_in));

				}
				break;



			case FTP_PASS:
				// User gets server errors, if he is trying to do smthg
				if ( line.size () >  sizeof(cmd_quit) && memcmp(line.c_str(), cmd_quit, sizeof(cmd_quit)) == 0 )
				{
					//user has sent data starting with cmd_quit
					msg->getResponder()->doRespond(s_quit, strlen(s_quit));
					m_state = FTP_DONE;
					retval = CL_DROP;
				} else
				{
					//user has sent unusable junk
					msg->getResponder()->doRespond(s_server_error, strlen(s_server_error));
				}

				m_Buffer->clear();
				break;


			case FTP_DONE:
				retval = CL_ASSIGN;
				break;
			}
		}

		if (buffercut == false)
		{
			i++;
		}
		


		
	}
	return retval;
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
ConsumeLevel FTPdDialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
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
ConsumeLevel FTPdDialogue::handleTimeout(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionLost(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel FTPdDialogue::connectionLost(Message *msg)
{
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel FTPdDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}

void FTPdDialogue::dump()
{
	logWarn("Unknown exploit %i bytes \n",m_Shellcode->getSize());
	HEXDUMP(m_Socket,(byte *) m_Shellcode->getData(), m_Shellcode->getSize());
}

ftp_exploit FTPdDialogue::identExploit(string line)
{

	if ( m_state == FTP_NULL )
	{
		// USER exploit

		// freeFTPd 1.08 exploit
		if ( line.size() > 1050 )
		{
			if ( memcmp(line.c_str() + 1013, "\xeb\x06", 2) == 0 )
			{
				logSpam("FreeFTPd 1.08 exploit detected\n");
				return FREEFTPD;
			}
		}

		// warFTPd 1.65 exploit
		// Win32 Opcode List
		const char* opcodes[3] = {"\xe2\x31\x02\x75", "\x54\x1d\xab\x71", "\x72\x93\xab\x71"}; 

		if ( line.size() > 500 )
		{
			for ( int i=0; i<3; i++ )
			{
				if ( memcmp(line.c_str() + 490,opcodes[i],4) == 0 )
				{
					logSpam("WarFTPd 1.65 USER exploit detected\n");
					return WARFTPD_USER;
				}
			}
		}
	}

	if ( m_state == FTP_USER )
	{
		// PASS exploit
		if ( line.size() > 600 )
		{
			if ( memcmp(line.c_str() + 563,"\xeb\x08\xeb\x08",4) == 0 )
			{
				logSpam("WarFTPd 1.65 PASS exploit detected\n");
				return WARFTPD_PASS;
			}
		}

	}
	logSpam("UNKNOWN exploit detected\n");

	return UNKNOWN;

}

const char *
FTPdDialogue::identExploitString ( ftp_exploit exploit )
{
	switch ( exploit )
	{
	case FREEFTPD:
		return "FreeFTPd 1.08";
		break;

	case WARFTPD_USER:
		return "WarFTPd 1.65 USER";
		break;

	case WARFTPD_PASS:
		return "WarFTPd 1.65 PASS";
		break;

	case UNKNOWN:

		if ( m_state == FTP_NULL )
		{
			return "Generic FTP USER";
		}
		else if ( m_state == FTP_USER )
		{
			return "Generic FTP PASS";
		}
		else
		{
			return "Generic FTP";
		}		

		break;

	default:
		assert( false );
		break;
	}
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new FTPd(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}
