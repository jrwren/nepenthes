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

#include "x-7.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"

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
 * creates a new X7 Module, 
 * X7 is an example for using raw sockets
 * 
 * 
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
X7::X7(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-7";
	m_ModuleDescription = "eXample Module 7 -raw socket usage-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "x-7 Factory";
	m_DialogueFactoryDescription = "eXample Dialogue Factory";

	g_Nepenthes = nepenthes;
}

X7::~X7()
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
bool X7::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();

	m_Nepenthes->getSocketMgr()->createRAWSocketUDP(0,53,0,45,this);
// 	m_Nepenthes->getSocketMgr()->createRAWSocketTCP(32821,0,45,this);
	return true;
}

bool X7::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new X7Dialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *X7::createDialogue(Socket *socket)
{
	return new X7Dialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}







/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the X7Dialogue, creates a new X7Dialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
X7Dialogue::X7Dialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "X7Dialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;
}

X7Dialogue::~X7Dialogue()
{

}



void dns_decode_name(char *name, char **buf)
{
  int32_t i, k, len, j;

  i = k = 0;
  while( **buf ){
         len = *(*buf)++;
         for( j = 0; j<len ; j++)
	      name[k++] = *(*buf)++;
         name[k++] = '.';
  }
  name[k-1] = *(*buf)++;
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel X7Dialogue::incomingData(Message *msg)
{
/*	if(!(msg->getSocket()->getType() & ST_RAW_UDP))
	{
		logCrit("GOT NON UDP Packet %i \n",msg->getSize());
		return CL_DROP;
	}

	logInfo("got dns foobar %i \n",msg->getSize());

	dns_header *dns = (dns_header *)msg->getMsg();

	logSpam(" OPCODE is %i \n",dns->flag_opcode);
	logSpam("\t dns->transid %2x \n",dns->transid);
	logSpam("\t dns->flag_opcode %i \n",dns->flag_opcode);

	switch(dns->flag_qr)
	{
	case DNS_QRFLAG_QUERY:
		logSpam("%s","DNS Packet is a Query\n");
		{

		}
		break;

	case DNS_QRFLAG_RESPONSE:
		logSpam("%s","DNS Packet is a Response\n");
		{
			char *rr = (char *)dns;
                        rr +=12;
			dns_rr_t *header = (dns_rr_t *)rr;

			g_Nepenthes->getUtilities()->hexdump((byte *)rr,msg->getSize()-12);
			char name[256];
			for (uint32_t i=1;i<=ntohs(dns->number_questions);i++)
			{
				dns_decode_name(name,(char **)&rr);
                logSpam("Question %02i/%02i %s \n",i,ntohs(dns->number_questions),name);
				rr +=2;	// type
				rr +=2;	// class
			}

			for (uint32_t i=1;i<=ntohs(dns->number_answers);i++)
			{
				if ( *rr & 0xC0)
				{// compressed reply
					dns_rr_t *rrh = header;
                    dns_decode_name(name,(char **)&rrh);
					rr +=2;// ((char *)rrh - (char *)header);
				}else
				{
                	dns_decode_name(name,(char **)&rr);
				}

				

				uint16_t type = *(uint16_t *)rr;;
				rr +=2;	// type

				
				rr +=2;	// class
				rr +=4;	// ttl
				
				 uint16_t datalen = *(uint16_t *)rr;
				rr +=2;	// datalen

				if (ntohs(type) == DNS_QUERYTYPE_A)
				{
					logSpam("Answer %02i/%02i %s datalen %i ip %s  len \n",i,ntohs(dns->number_answers),name, ntohs(datalen), inet_ntoa(*(in_addr *)rr));	
					rr +=ntohs(datalen);	// the datalen
				}
				else
				if (ntohs(type) == DNS_QUERYTYPE_CNAME)
				{
					char cname[256];
					char *rrh = rr;
					dns_decode_name(cname,(char **)&rrh);
					logSpam("Answer %02i/%02i %s datalen %i cname %s  len \n",i,ntohs(dns->number_answers),name, ntohs(datalen), cname);
					rr +=ntohs(datalen);	// the datalen
				}

                
				

			}


		}
		break;

	}

	int32_t j=0;
	for (int32_t i=0;i<dns->number_questions;i++)
	{
		logSpam("DNS %s \n",(char *)dns+sizeof(dns_header)+j);
		j+=strlen((char *)dns+sizeof(dns_header)+j);
	}
*/
	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel X7Dialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

/**
 * Dialogue::handleTimeout(Message *)
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel X7Dialogue::handleTimeout(Message *msg)
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
ConsumeLevel X7Dialogue::connectionLost(Message *msg)
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
ConsumeLevel X7Dialogue::connectionShutdown(Message *msg)
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
        *module = new X7(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
