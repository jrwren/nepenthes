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

#include <map>

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "EventHandler.hpp"
#include "SQLCallback.hpp"


using namespace std;


#define AS_POSSIBLE_MALICIOUS_CONNECTION	0x00000
#define AS_DEFINITLY_MALICIOUS_CONNECTION	0x00001

#define AS_DOWNLOAD_OFFER 					0x00010
#define AS_DOWNLOAD_SUCCESS					0x00020


#define DT_DIALOGUE_NAME					0x00001
#define DT_SHELLCODEHANDLER_NAME			0x00002
#define DT_DOWNLOAD_URL						0x00004
#define DT_DOWNLOAD_HASH					0x00008


namespace nepenthes
{
	struct ltint
	{
		bool operator()(uint32_t s1, uint32_t s2) const
		{
			return s1<s2;
		}
	};

	class LSDetail
	{
	public:
		LSDetail(uint32_t host, int type, string data);
		string m_host;
		int m_type;
		string m_data;
	};

	class LSContext
	{
	public:
		LSContext();
        uint32_t m_attackID;
		list <LSDetail *> m_Details;
		bool m_closed;
		int m_severity;
	};


	typedef enum
	{
		LS_MODE_ANY,
		LS_MODE_LIST,
	} log_surfnet_mode;


	class ShellcodeHandler;

	class SQLHandler;

	/**
	 * LogSurfNET
	 * log to the surfnet IDS database
	 * 
     */
	class LogSurfNET : public Module , public EventHandler, public SQLCallback
	{
	public:
		LogSurfNET(Nepenthes *);
		~LogSurfNET();
		bool Init();
		bool Exit();

		uint32_t handleEvent(Event *event);

		bool sqlSuccess(SQLResult *result);
		bool sqlFailure(SQLResult *result);
		void sqlConnected();
		void sqlDisconnected();


	private:
		map <uint32_t, LSContext, ltint> m_SocketTracker;

		uint16_t		*m_Ports;
		uint16_t        m_MaxPorts;


		void handleTCPAccept(Socket *socket);
		void handleTCPclose(Socket *socket, uint32_t attackid);

		void handleDialogueAssignAndDone(Socket *socket, Dialogue *dia, uint32_t attackid);

		void handleShellcodeDone(Socket *socket, ShellcodeHandler *handler, uint32_t attackid);


		void handleDownloadOffer(uint32_t localhost, uint32_t remotehost,const char *url);
		void handleDownloadSuccess(uint32_t localhost, uint32_t remotehost,const char *url, const char *md5hash);

		SQLHandler 		*m_SQLHandler;
		log_surfnet_mode	m_RunningMode;
	};
	

}

extern nepenthes::Nepenthes *g_Nepenthes;
