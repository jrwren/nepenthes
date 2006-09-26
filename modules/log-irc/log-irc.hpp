/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter
 * Copyright (C) 2006  Georg Wicherski
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

#include <string>

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"
#include "DNSCallback.hpp"
#include "LogHandler.hpp"

using namespace std;

namespace nepenthes
{
	typedef enum
	{
		LIRC_INIT,
		LIRC_NULL,
		LIRC_RESOLV_TOR,
		LIRC_RESOLV_IRC
	} log_irc_state;

	class IrcDialogue;

	class LogIrc : public Module , public DNSCallback , public LogHandler
	{
	public:
		LogIrc(Nepenthes *);
		~LogIrc();
		Dialogue *createDialogue(Socket *socket);
		bool Init();
		bool Exit();

        	bool dnsResolved(DNSResult *result);
		bool dnsFailure(DNSResult *result);

		void log(uint32_t mask, const char *message);
		bool setOwnership(int32_t uid, int32_t gid) { return true; }

		bool doStart();
		bool doStopp();
		bool doRestart();

		

		uint32_t getIrcIP();
		uint16_t getIrcPort();
		string getIrcNick();
		string getIrcIdent();
		string getIrcUserInfo();
		string getIrcChannel();
		string getIrcChannelPass();
		string getIrcUserModes();
		string getIrcPass();
		
		string getTorServer();
		string getIrcServer();
		
		string getConnectCommand();


		void setDialogue(IrcDialogue *);
		
		bool logMaskMatches(uint32_t mask);
		void setLogPattern(const char *patternString);

		bool useTor();
		
	private:
		log_irc_state m_State;

		bool m_UseTor;
		string m_TorServer;
		uint32_t m_TorIP;

		uint16_t m_TorPort;

		

		string m_IrcServer;
		uint32_t m_IrcIP;

		uint16_t m_IrcPort;


		string m_IrcPass;
		string m_IrcNick;
		string m_IrcIdent;
		string m_IrcUserInfo;
		string m_IrcUserModes;

		string m_IrcChannel;
		string m_IrcChannelPass;
		
		uint32_t m_LogPatternNumeric;
		
		string m_ConnectCommand;

		IrcDialogue *m_IrcDialogue;

	};

}
extern nepenthes::Nepenthes *g_Nepenthes;

