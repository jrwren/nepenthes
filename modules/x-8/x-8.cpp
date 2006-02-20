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

#include "config.h"

#ifdef HAVE_GEOLOCATION

#include <ctype.h>

#include "x-8.hpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"
#include "Nepenthes.hpp"


#include "GeoLocationManager.hpp"
#include "GeoLocationResult.hpp"


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
 * creates a new X8 Module, 
 * X8 is an example for using the GeoLocationManager and GeoLocationCallback 's
 * 
 * 
 * @param nepenthes the pointer to our Nepenthes
 */
X8::X8(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-8";
	m_ModuleDescription = "eXample Module 8 -GeoLocationHandler eXample-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_DialogueFactoryName = "x-8 Factory";
	m_DialogueFactoryDescription = "eXample Dialogue Factory";

	g_Nepenthes = nepenthes;
}


/**
 * X8 destructor
 * does nothing
 */
X8::~X8()
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
bool X8::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	m_Nepenthes->getSocketMgr()->bindTCPSocket(0,10004,0,45,this);
	return true;
}

bool X8::Exit()
{
	return true;
}

/**
 * DialogueFactory::createDialogue(Socket *)
 * 
 * creates a new X8Dialogue
 * 
 * @param socket the socket the DIalogue has to use, can be NULL if the Dialogue can handle it
 * 
 * @return returns the new created dialogue
 */
Dialogue *X8::createDialogue(Socket *socket)
{
	return new X8Dialogue(socket);
//	return g_Nepenthes->getFactoryMgr()->getFactory("WinNTShell DialogueFactory")->createDialogue(socket);
}


/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the X8Dialogue, creates a new X8Dialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
X8Dialogue::X8Dialogue(Socket *socket)
{
	m_Socket = socket;
    m_DialogueName = "X8Dialogue";
	m_DialogueDescription = "eXample Dialogue";

	m_ConsumeLevel = CL_ASSIGN;

	m_Socket->doRespond("Welcome to geolocate Shell\n",strlen("Welcome to geolocate Shell\n"));
	m_RequestsPending = 0;
}

X8Dialogue::~X8Dialogue()
{

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
ConsumeLevel X8Dialogue::incomingData(Message *msg)
{
	char *message = strdup(msg->getMsg());
	char *freemsg = message;

	for(uint32_t i=0;i < strlen(message);i++)
	{
		if(!isgraph(message[i]) && message[i] != ' ')
		{
			message[i] = ' ';
		}
	}

	char *cmd = strsep(&message, " ");

	if( !strcmp(cmd, "geo") )
	{
		char *url;
		while( (url  = strsep(&message, " ")) != NULL)
		{
			if (strlen(url) > 3)
			{
				
            	uint32_t ip = inet_addr(url);
				g_Nepenthes->getGeoMgr()->addGeoLocation(this,ip,this);
				m_RequestsPending++;
			}
		}
		string sDeineMutter("hostip trying to resolve\n");
		msg->getResponder()->doRespond((char *)sDeineMutter.c_str(),sDeineMutter.size());

	}else
	if( !strcmp(cmd, "rand") )
	{
		char *num;
		num  = strsep(&message, " ");

		char ip[16];
		uint32_t a,b,c,d;

		uint32_t numbers = atoi(num);
		while (numbers > 0)
		{
			a = rand()%255;
			b = rand()%255;
			c = rand()%255;
			d = rand()%255;
			sprintf(ip,"%i.%i.%i.%i",a,b,c,d);

			uint32_t address = inet_addr(ip);
			g_Nepenthes->getGeoMgr()->addGeoLocation(this,address,this);
			m_RequestsPending++;
			numbers--;
		}
	}

	free(freemsg);
	return CL_ASSIGN;
}

/**
 * Dialogue::outgoingData(Message *)
 * 
 * @param msg the message
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel X8Dialogue::outgoingData(Message *msg)
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
ConsumeLevel X8Dialogue::handleTimeout(Message *msg)
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
ConsumeLevel X8Dialogue::connectionLost(Message *msg)
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
ConsumeLevel X8Dialogue::connectionShutdown(Message *msg)
{
	return CL_DROP;
}



/**
 * the GeoLocationQuery was successfull
 * 
 * @param result the GeoLocationResult with all information the GeoLocationHandler could gather
 */
void X8Dialogue::locationSuccess(GeoLocationResult *result)
{
	char ctx[16];
	m_RequestsPending--;
	logPF();

	string reply;
	uint32_t ip = result->getAddress();

	reply = "IP: ";
	reply += inet_ntoa(*(in_addr *)&ip);
	reply += "\n";

	reply += "\t Country: '";
	reply += result->getCountry();
	reply += "'\n";


	reply += "\t CountryShort: '";
	reply += result->getCountryShort();
	reply += "'\n";

	reply += "\t City: ";
	reply += result->getCity();
	reply += "\n";

	reply += "\t Context: ";
	sprintf(ctx,"%x",(uint32_t)result->getObject());
	reply += ctx;
	reply += "\n";

	reply += "\n";

	m_Socket->doRespond((char *)reply.c_str(),reply.size());
	if (m_RequestsPending == 0)
	{
		m_Socket->setStatus(SS_CLEANQUIT);
	}
}


/**
 * the GeoLocationQuery failed
 * 
 * @param result the result with the additional data onboard so we can at least say what failed
 */
void X8Dialogue::locationFailure(GeoLocationResult *result)
{

}


extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new X8(nepenthes);
        return 1;
    } else {
        return 0;
    }
}

#endif // HAVE_GEOLOCATION
