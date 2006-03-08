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

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "SubmitHandler.hpp"
#include "DNSCallback.hpp"
#include "EventHandler.hpp"


using namespace std;

namespace nepenthes
{
	// wait 5 seconds before attempting reconnection the first time
	#define GOTEK_CTRL_SHORT_WAIT (5)
	// retry every 30 secs if the first reconnection fails
	#define GOTEK_CTRL_LONG_WAIT (30)


	struct GotekContext
	{
		unsigned char *m_FileBuffer;
		uint32_t m_FileSize;
		unsigned char	m_SHA512Hash[64];
		uint64_t	m_EvCID;
	};
	
	enum GotekSubmitHandlerStatus
	{
		GSHS_RESOLVING,
		GSHS_WAITING_SHORT,
		GSHS_WAITING_LONG,
		GSHS_CONNECTED,
	};


	class Socket;

	class GotekSubmitHandler : public Module , public SubmitHandler, public DNSCallback, public EventHandler
	{
	public:
		GotekSubmitHandler(Nepenthes *);
		~GotekSubmitHandler();
		bool Init();
		bool Exit();

		void Submit(Download *down);
		void Hit(Download *down);
		string getUser();
		unsigned char *getCommunityKey();

		void setSessionKey(uint64_t);

		bool dnsResolved(DNSResult *result);
		bool dnsFailure(DNSResult *result);

		void setSocket(Socket *);

		bool popGote();
		bool sendGote();
		
		void childConnectionLost();
		uint32_t handleEvent(Event *event);

		
	protected:
		Socket *m_CTRLSocket;
		string m_User;
		unsigned char *m_CommunityKey;

		uint64_t m_Sessionkey;
		string m_GotekHost;
		uint32_t m_GotekHostIP;

		uint16_t m_GotekPort;

		list <GotekContext *> m_Goten;
		
		GotekSubmitHandlerStatus m_ControlConnStatus;
	};

}

extern nepenthes::Nepenthes *g_Nepenthes;
extern nepenthes::GotekSubmitHandler *g_GotekSubmitHandler;
