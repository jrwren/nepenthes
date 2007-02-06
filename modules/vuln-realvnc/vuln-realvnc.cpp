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

#include <ctype.h>

#include "vuln-realvnc.hpp"

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
 * creates a new VulnRealVNC Module, 
 * VulnRealVNC is an example for binding a socket & setting up the Dialogue & DialogueFactory
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
VulnRealVNC::VulnRealVNC(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-2";
	m_ModuleDescription = "eXample Module 2 -binding sockets & setting up a dialogue example-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "x-2 Factory";
	m_DialogueFactoryDescription = "eXample Dialogue Factory";

	g_Nepenthes = nepenthes;
}

VulnRealVNC::~VulnRealVNC()
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
bool VulnRealVNC::Init()
{
/*	if ( m_Config == NULL )
	{
		logCrit("I need a config\n");
		return false;
	}

	StringList sList;
	int32_t timeout;
	try
	{
		sList = *m_Config->getValStringList("x-2.ports");
		timeout = m_Config->getValInt("x-2.accepttimeout");
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
*/	

	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,5900,0,60,this);
	return true;
}

bool VulnRealVNC::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new VulnRealVNCDialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *VulnRealVNC::createDialogue(Socket *socket)
{
	return new RealVNCDialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}






const char *rfb_version_003_008 = "RFB 003.008\n";

/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the RealVNCDialogue, creates a new RealVNCDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
RealVNCDialogue::RealVNCDialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "RealVNCDialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	
	m_Socket->doRespond((char*)rfb_version_003_008,strlen(rfb_version_003_008));

	m_Buffer = new Buffer(512);

	m_State = VNC_HANDSHAKE;

	m_sendimage = false;

}

RealVNCDialogue::~RealVNCDialogue()
{
	delete m_Buffer;

	size_t offset;

	logCrit("VNCCommandSession '%s'\n",m_TypedChars.c_str());	

    if ( (offset = m_TypedChars.find("cmd")) < m_TypedChars.size()  ||
		 (offset = m_TypedChars.find("echo")) < m_TypedChars.size() ||
		 (offset = m_TypedChars.find("tftp")) < m_TypedChars.size() 

		 )
	{
		string command = m_TypedChars.substr(offset,m_TypedChars.size() - offset);
		logCrit("command offset %i '%s'\n",(int)offset,command.c_str());
		
	}else
    if ( (offset = m_TypedChars.find("http://")) < m_TypedChars.size() ||
			 (offset = m_TypedChars.find("ftp://")) < m_TypedChars.size() )
	{
			string command = m_TypedChars.substr(offset,m_TypedChars.size() - offset);
			logCrit("download offset %i '%s'\n",(int)offset,command.c_str());
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
ConsumeLevel RealVNCDialogue::incomingData(Message *msg)
{
	m_Buffer->add(msg->getMsg(),msg->getSize());

	

	if ( m_State == VNC_HANDSHAKE )
	{
	
		logSpam("VNC_HANDSHAKE\n");
//		g_Nepenthes->getUtilities()->hexdump((byte *)m_Buffer->getData(),m_Buffer->getSize());
		if (m_Buffer->getSize() >= strlen(rfb_version_003_008) && 
			memcmp(m_Buffer->getData(),rfb_version_003_008,strlen(rfb_version_003_008)) == 0)
		{
//			logSpam("got rfb_version_003_008\n");
			m_Buffer->cut(strlen(rfb_version_003_008));
			static const char auth_offer[2] = {  01, 02 };
			m_Socket->doWrite((char *)auth_offer,2);

			m_State = VNC_AUTH;
		}
	}

	if ( m_State == VNC_AUTH)
	{
    	logSpam("VNC_AUTH\n");
//		g_Nepenthes->getUtilities()->hexdump((byte *)m_Buffer->getData(),m_Buffer->getSize());
		if (m_Buffer->getSize() >= 1 )
		{
			if (1)// *(char *) (m_Buffer->getData()) == 1)
			{
				m_Buffer->cut(1);

				unsigned int auth_accept = 0;
				m_Socket->doWrite((char *)&auth_accept,4);

				m_State = VNC_SHARED_DESKTOP;
			}
		}
	}
//		break;

	if ( m_State == VNC_SHARED_DESKTOP)
	{
    	logSpam("VNC_SHARED_DESKTOP\n");
		if ( m_Buffer->getSize() >= 1 )
		{
			m_Buffer->cut(1);

			static const char vnc_isupport[] = {
				0x04, 0x3a, 0x02, 0xff, 0x20, 0x18, 0x00, 0x01, 
				0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x10, 0x08, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 
				0x46, 0x4f, 0x4f, 0x42, 0x41, 0x52, 0x2d, 0x4d, 
				0x55, 0x54, 0x54, 0x45, 0x52};
			m_Socket->doWrite((char *)vnc_isupport,sizeof(vnc_isupport));
            m_State = VNC_ONLINE;

		}
	}
//		break;

	if ( m_State == VNC_ONLINE )
	{
    	logSpam("VNC_ONLINE\n");
		{
			bool needmore=false;

			while ( m_Buffer->getSize() >= 1 && needmore == false)
			{
				unsigned char c = *(char *)m_Buffer->getData();

				switch ( c )
				{
				case 0:
					logSpam("ClientReq:  set pixel format\n");
					if (m_Buffer->getSize() >= 20 )
                    {
						m_Buffer->cut(20);
					}else
					{
						needmore = true;
					}
					break;

				case 1:
					logSpam("ClientReq: Fix Color Map\n");
					break;

				case 2:
					{

						logSpam("ClientReq: set encodings\n");
						uint16_t num = * (uint16_t *) ((byte *)m_Buffer->getData() + 2);
						num = ntohs(num);
						logSpam("set encodings = %i\n",num);
						if ( m_Buffer->getSize() >= (uint32_t)(4 + num * 4) )
						{
							m_Buffer->cut(4 + num * 4);
						}
						else
						{
							needmore = true;
						}
					}
					break;

				case 3:
					logSpam("ClientReq: framebufferupdate request\n");
					if (m_Buffer->getSize() >= 10 )
					{
						m_Buffer->cut(10);
					}else
					{
						needmore = true;
					}
					if (m_sendimage == false)
					{
						m_Socket->doWrite((char *)vnc_image,sizeof(vnc_image));
						m_sendimage = true;
					}

					break;

				case 4:
//					logSpam("ClientReq: KeyEvent\n");
//					g_Nepenthes->getUtilities()->hexdump((byte *)m_Buffer->getData(),m_Buffer->getSize());
					if (m_Buffer->getSize() >= 8 )
					{
						uint32_t skey;
						skey = *(uint32_t *)((byte *)m_Buffer->getData()+4);

						byte *key;
						key = (byte *)&skey;

						byte *updown = ((byte *)m_Buffer->getData()+1);


//						printf("updown '%x'\n",(unsigned int)*updown);
						if ( *updown != 0 )
						{
							if ( key[2] == 0 )
							{
								if ( isalpha( key[3] )  || isspace( key[3] ) || isprint( key[3] ) )
								{
									logSpam("key %c\n",key[3]);
									m_TypedChars += key[3];
									logSpam("Session is %s\n",m_TypedChars.c_str());
								}else
								{
									logSpam("key %02x\n",key[3]);
								}
							}
							else
							if ( key[2] == 0xff )
							{
								switch (key[3])
								{

								case 0x0d:	// return 
									logSpam("key [return]\n");
									m_TypedChars += "\n";
									break;

								case 0x08: // BackSpace   
									logSpam("key [backspace]\n");
									break;

								case 0x09: // Tab         
									logSpam("key [tab]\n");
									break;

								case 0x1b: // Escape      
									logSpam("key [esc]\n");
									break;

								case 0x63: // Insert 
									logSpam("key [insert]\n");
									break;

								case 0xff: // Delete    
									logSpam("key [delete]\n");
									break;

								case 0x50: // Home        
									logSpam("key [home]\n");
									break;

								case 0x57: // End     
									logSpam("key [end]\n");
									break;

								case 0x55: // Page Up
									logSpam("key [page up]\n");
									break;

								case 0x56: // Page Down 
									logSpam("key [page down]\n");
									break;

								case 0x51: // Left
									logSpam("key [left]\n");
									break;

								case 0x52: // Up      
									logSpam("key [up]\n");
									break;

								case 0x53: // Right   
									logSpam("key [right]\n");
									break;

								case 0x54: // Down    
									logSpam("key [down]\n");
									break;

								case 0xbe: // F1      
									logSpam("key [f1]\n");
									break;

								case 0xbf: // F2      
									logSpam("key [f2]\n");
									break;

								case 0xc0: // F3
									logSpam("key [f3]\n");
									break;

								case 0xc1: // F4      
									logSpam("key [f4]\n");
									break;

								case 0xc2: // F5
									logSpam("key [f5]\n");
									break;

								case 0xc3: // F6      
									logSpam("key [f6]\n");
									break;

								case 0xc4: // F7  
									logSpam("key [f7]\n");
									break;

								case 0xc5: // F8    
									logSpam("key [f8]\n");
									break;

								case 0xc6: // F9      
									logSpam("key [f9]\n");
									break;

								case 0xc7: // F10     
									logSpam("key [f10]\n");
									break;

								case 0xc8: // F11     
									logSpam("key [f11]\n");
									break;

								case 0xc9: // F12     
									logSpam("key [f12]\n");
									break;

								case 0xe1: // Shift   
									logSpam("key [shift]\n");
									break;

								case 0xe3: // Control
									logSpam("key [control]\n");
									break;

								case 0xe7: // Meta
									logSpam("key [meta]\n");
									break;

								case 0xe9: // Alt    
									logSpam("key [alt]\n");
									break;

								case 0xeb:	// Windows key
									logSpam("key [windows]\n");
									break;

								default:
									logSpam("key [unknown] code %02x\n",key[3]);

								}
							}
						}
						m_Buffer->cut(8);
					}else
					{
						needmore = true;
					}
					break;

				case 5:
					if (m_Buffer->getSize() >= 6 )
					{
						m_Buffer->cut(6);
					}else
					{
						needmore = true;
					}
					break;


				case 6:
					logSpam("ClientReq: CutEvent\n");
//					g_Nepenthes->getUtilities()->hexdump((byte *)m_Buffer->getData(),m_Buffer->getSize());
					if (m_Buffer->getSize() >= 8 )
					{
						uint32_t cpbytes;
						cpbytes = *(uint32_t *)((byte *)m_Buffer->getData()+4);
						cpbytes = ntohl(cpbytes);
						logSpam("c&p %i bytes\n",cpbytes);

						if ( m_Buffer->getSize() >= 8 + cpbytes)
						{
							string cp = string((char*)m_Buffer->getData()+8,cpbytes);
							logInfo("c&p %s\n",cp.c_str());
							m_TypedChars += cp;

							m_Buffer->cut(8 + cpbytes);
						}else
						{
							needmore = true;
						}
					}else
					{
						needmore = true;
					}
					break;

				default:
					logCrit("Unknown VNC Command, out of sync?\n");
					needmore = true;
						
				}
			}
		}
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
ConsumeLevel RealVNCDialogue::outgoingData(Message *msg)
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
ConsumeLevel RealVNCDialogue::handleTimeout(Message *msg)
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
ConsumeLevel RealVNCDialogue::connectionLost(Message *msg)
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
ConsumeLevel RealVNCDialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}




extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new VulnRealVNC(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
