/********************************************************************************
 *
 * Copyright (C) 2008  Jason V. Miller
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
 *******************************************************************************/

#include "Buffer.hpp"
#include "Buffer.cpp"
#include "Config.hpp"
#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "DNSEvent.hpp"
#include "DNSQuery.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include "EventHandler.cpp"
#include "EventManager.hpp"
#include "LogManager.hpp"
#include "Message.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "Nepenthes.hpp"
#include "ShellcodeHandler.hpp"
#include "Socket.hpp"
#include "SocketEvent.hpp"
#include "SocketManager.hpp"
#include "SubmitEvent.hpp"
#include "Utilities.hpp"

#include <list>
#include <map>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>

/*
** General Helper Functions
*/
bool	save_to_file	( string, string, void *, size_t );

using namespace std;

namespace nepenthes
{
	class AttackCtx;

	class AttackSubCtx;
	class DNSCtx;
	class DownloadCtx;
	class ShellcodeCtx;
	class SocketCtx;

	class LogAttack;

	enum LogModeAttack
	{
		LOG_TO_DIRECTORY,
		LOG_TO_FILE,
	};

	enum LogModeConnData
	{
		LOG_ALL,
		LOG_LIMITED,
		LOG_NONE,
	};

	struct config
	{
		string		logAttack;
		string		logdirConnData;
		string		logdirFiles;
		LogModeAttack	logModeAttack;
		LogModeConnData	logModeConnData;
	};

	class AttackCtx
	{
	public:
		AttackCtx			( void );
		~AttackCtx			( void );
		void	addCtx			( AttackSubCtx * );
		int	getActiveCtxCount	( void );
		int	getCtxCount		( void );
		bool	log			( FILE *outfile, struct config * );
		void	logState		( void );
		bool	isComplete		( void );
	private:
		list	<AttackSubCtx *>	m_Ctxs;
	};

	class AttackSubCtx
	{
	public:
		enum State
		{
			STATE_ACTIVE,
			STATE_INACTIVE,
		};

		enum WantLevel
		{
			WANT_NO,
			WANT_UNSET,
			WANT_YES,
		};

		virtual		~AttackSubCtx	( void ) {};
		State		getState	( void );
		string		getStateString	( void );
		WantLevel	getWantTraffic	( void );
		virtual bool	log		( FILE *outfile, struct config *, bool ) = 0;
		virtual void	logState	( void ) = 0;
		void		setState	( State );
		void		setWantTraffic	( WantLevel );

	protected:
		AttackSubCtx ( State );
		WantLevel	m_WantTraffic;
		State		m_State;
		struct timeval	m_Finished;
		struct timeval	m_Started;

	private:
	};

	class DNSCtx : public AttackSubCtx
	{
	public:
		DNSCtx		( DNSQuery *query, AttackSubCtx::State );
		~DNSCtx		( void ) {};
		bool		log		( FILE *outfile, struct config *, bool );
		void		logState	( void );
		void		setAddress	( uint32_t address );
	private:
		DNSQuery	*m_Query;
		uint32_t	m_Address;
		string		m_Hostname;
		bool		m_Success;
	};

	class DownloadCtx : public AttackSubCtx
	{
	public:
		enum Result
		{
			RESULT_FAILED,
			RESULT_FILTERED,
			RESULT_SUCCEEDED,
		};

		DownloadCtx ( Download *download, AttackSubCtx::State );
		~DownloadCtx ( void ) {};
		bool		log			( FILE *outfile, struct config *, bool );
		void		logState		( void );
		void		setInitiatingSocketCtx	( SocketCtx * );
		void		setSubmittingSocketCtx	( SocketCtx * );
		void		setPayloadInfo		( Download *download, bool );
	protected:
		string		getResultString		( void );
	private:
		Result		m_Result;
		SocketCtx	*m_InitiatingSocketCtx;
		SocketCtx	*m_SubmittingSocketCtx;
		string		m_MD5;
		string		m_SHA512;
		string		m_Trigger;
		string		m_URL;
	};

#define DIRECTION_RX 0x10000000
#define DIRECTION_TX 0x20000000

	class SocketCtx : public AttackSubCtx
	{
	public:
		enum Protocol
		{
			PROTO_TCP,
			PROTO_UDP,
		};

		enum Type
		{
			TYPE_INCOMING,
			TYPE_LISTENER,
			TYPE_OUTGOING,
		};

		SocketCtx	( Socket *, AttackSubCtx::State );
		~SocketCtx	( void ) {};

		void		addMessageRX	( Message *msg );
		void		addMessageTX	( Message *msg );
		bool		log		( FILE *, struct config *, bool );
		void		logState	( void );
		bool		logTraffic	( struct config *, string );
		void		setConnected	( bool );
		void		setLocalPort	( uint16_t );

	private:
		uint32_t	m_BytesRX;
		uint32_t	m_BytesTX;
		uint32_t	m_ChunksRX;
		uint32_t	m_ChunksTX;
		bool		m_Connected;
		list <uint32_t>	m_DataOrder;
		Buffer		m_DataRX;
		Buffer		m_DataTX;
		uint32_t	m_LocalHost;
		uint16_t	m_LocalPort;
		Protocol	m_Protocol;
		uint32_t	m_RemoteHost;
		uint16_t	m_RemotePort;
		Type		m_Type;
	};

	class ShellcodeCtx : public AttackSubCtx
	{
	public:
		ShellcodeCtx	( SocketCtx *, string, string, bool, AttackSubCtx::State );
		~ShellcodeCtx	( void ) {};
		bool		log			( FILE *outfile, struct config *, bool );
		void		logState		( void );
		void		setHandlerName		( string );
		void		setSuccessfullyEmulated ( bool );
	private:
		SocketCtx	*m_InjectingSocketCtx;
		string		m_HandlerName;
		bool		m_KnownAttack;
		string		m_MD5;
		bool		m_SuccessfullyEmulated;
		string		m_Trigger;
	};

	class LogAttack : public Module , public EventHandler
	{
	public:

		LogAttack	( Nepenthes * );
		~LogAttack	( void );

		bool		Init		( void );
		bool		Exit		( void );
		uint32_t	handleEvent	( Event * );

	private:
		bool		attemptAttackCtxClosure	( AttackCtx *attackCtx );
		void		createNewSocketCtx	( Socket *, SocketCtx::State, AttackCtx * );
		bool		closeCurrentLogFile	( void );
		void		closeDownloadCtx	( DownloadCtx * );
		void		closeSocketCtx		( Socket * );
		AttackCtx *	getAttackCtx		( AttackSubCtx * );
		DNSCtx *	getDNSCtx		( DNSQuery * );
		DownloadCtx *	getDownloadCtx		( Download * );
		SocketCtx *	getSocketCtx		( Socket * );
		bool		log			( AttackCtx * );
		bool		logToDirectory		( AttackCtx *, string, struct timeval );
		bool		logToFile		( AttackCtx *, string );
		void		setAttackCtx		( AttackCtx * );

		map <AttackSubCtx*, AttackCtx *>	m_AttackCtxs;
		map <DNSQuery *, DNSCtx *>		m_DNSCtxs;
                map <Download *, DownloadCtx *>		m_DownloadCtxs;
                map <Socket *, SocketCtx *>		m_SocketCtxs;

		uint32_t	m_AttackCtxsCount;

		AttackCtx *	m_AttackCtx;
		ShellcodeCtx *	m_ShellcodeCtx;
		SocketCtx *	m_SocketCtx;

		FILE *		m_CurrentLogFile;
		string		m_CurrentLogFilename;

		struct config	m_ConfigData;
	};
}
