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

#include "log-attack.hpp"

#define NAME "LogAttack"
#define SECONDS_PER_TICK 15

/* Helper Functions */

bool
save_to_file ( string directory, string filename, void *data, size_t datalen )
{
	FILE *out;
	int retval;
	string destination = directory + "/" + filename;
	struct stat st;

	retval = mkdir(directory.c_str(), 0777);

	if ( ! (retval == 0 || errno == EEXIST) )
	{
		logCrit("%s: failed to create directory '%s' in save_to_file: %s\n",
				NAME, directory.c_str(), strerror(errno));
	}

	retval = stat(destination.c_str(), &st);

	if ( retval == 0 )
	{
		utimes(destination.c_str(), NULL);
	}
	else
	{
		switch ( errno )
		{
		case ENOENT:
			if ( (out = fopen(destination.c_str(), "w+")) == NULL )
			{
				return false;
			}

			if ( datalen && fwrite(data, datalen, 1, out) != 1 )
			{
				logCrit("%s: failed to write to '%s' in save_to_file: %s\n",
					NAME, destination.c_str(), strerror(errno));

				retval = fclose(out);
				assert( retval == 0 );

				return false;
			}

			retval = fclose(out);
			assert( retval == 0 );
			break;

		default:
			logCrit("%s: failed stat '%s' in save_to_file: %s\n",
				NAME, destination.c_str(), strerror(errno));
			return false;
		}
	}

	return true;
}

using namespace nepenthes;

Nepenthes *g_Nepenthes;

/* AttackCtx */

AttackCtx::AttackCtx ( void )
{
}

AttackCtx::~AttackCtx ( void )
{
	list <AttackSubCtx *>::iterator ctx;

	for ( ctx=m_Ctxs.begin(); ctx!=m_Ctxs.end(); ctx++ )
		delete (*ctx);
}

void
AttackCtx::addCtx ( AttackSubCtx *ctx )
{
	m_Ctxs.push_back(ctx);
}

int
AttackCtx::getActiveCtxCount ( void )
{
	list <AttackSubCtx *>::iterator ctx;
	int count = 0;

	for ( ctx=m_Ctxs.begin(); ctx!=m_Ctxs.end(); ctx++ )
		if ( (*ctx)->getState() != AttackSubCtx::STATE_INACTIVE )
			count++;

	return count;
}

int
AttackCtx::getCtxCount ( void )
{
	return m_Ctxs.size();
}

bool
AttackCtx::isComplete ( void )
{
	return ( this->getActiveCtxCount() > 0 ) ? false : true;
}

bool
AttackCtx::log ( FILE *out, struct config *config )
{
	list <AttackSubCtx *>::iterator ctx;
	bool wantTraffic = true;

	assert( ! m_Ctxs.empty() );

	/*
	** Go through all of the SubCtxs to figure out if we should dump the
	** traffic for this attack. We will dump traffic unless one or more
	** SubCtxs asks us not to. If any SubCtx explicitly asks us to dump
	** traffic, we will.
	*/
        for ( ctx = m_Ctxs.begin(); ctx != m_Ctxs.end(); ctx++ )
	{
		switch ( (*ctx)->getWantTraffic() )
		{
		case AttackSubCtx::WANT_NO:
			wantTraffic = false;
			break;
		case AttackSubCtx::WANT_UNSET:
			break;
		case AttackSubCtx::WANT_YES:
			wantTraffic = true;
			goto DONE;
			break;
		default:
			assert( false );
			break;
		}
	}

DONE:

        for ( ctx = m_Ctxs.begin(); ctx != m_Ctxs.end(); ctx++ )
		if ( ! (*ctx)->log(out, config, wantTraffic) )
			return false;

	return true;
}

void
AttackCtx::logState ( void )
{
	list <AttackSubCtx *>::iterator ctx;

	logInfo("AttackCtx: Contains %d contexts, %d active:\n",
		m_Ctxs.size(), this->getActiveCtxCount());

	for ( ctx=m_Ctxs.begin(); ctx!=m_Ctxs.end(); ctx++ )
		(*ctx)->logState();
}

/* AttackSubCtx */

AttackSubCtx::AttackSubCtx ( State state )
{
	int retval;

	m_State = state;
	m_WantTraffic = WANT_UNSET;

	retval = gettimeofday(&m_Started, NULL);
	assert( retval == 0 );
}

AttackSubCtx::State
AttackSubCtx::getState ( void )
{
	return m_State;
}

string
AttackSubCtx::getStateString ( void )
{
	switch ( this->getState() )
	{
		case STATE_ACTIVE:
			return "ACTIVE";

		case STATE_INACTIVE:
			return "INACTIVE";

		default:
			assert(false);
			break;
	}
}

AttackSubCtx::WantLevel
AttackSubCtx::getWantTraffic ( void )
{
	return m_WantTraffic;
}

void
AttackSubCtx::setState ( AttackSubCtx::State state )
{
	int retval;

	assert( m_State != state );

	if ( state == STATE_INACTIVE )
	{
		retval = gettimeofday(&m_Finished, NULL);
		assert( retval == 0 );
	}

	m_State = state;
}

void
AttackSubCtx::setWantTraffic ( WantLevel value )
{
	m_WantTraffic = value;
}

/* DNSCtx */

DNSCtx::DNSCtx ( DNSQuery *query, State state ) :
AttackSubCtx(state)
{
	m_Hostname = query->getDNS();
	m_Query = query;
	m_Success = false;
}

bool
DNSCtx::log ( FILE *outfile, struct config *config, bool wantTraffic )
{
	int retval;

	retval = fprintf(outfile, "DNSReq|LUID=%p|Result=%s|Start=%ld.%ld|Finish=%ld.%ld|Hostname=%s|Addr=%s\n",
		this,
		m_Success ? "SUCCESS" : "FAILURE",
		m_Started.tv_sec,
		m_Started.tv_usec,
		m_Finished.tv_sec,
		m_Finished.tv_usec,
		m_Hostname.c_str(),
		m_Success ? inet_ntoa(*(in_addr *)&m_Address) : "");

	if ( retval <= 0 )
		return false;

	return true;
}

void
DNSCtx::logState ( void ) 
{
	logInfo("+ DNSCtx: %s\n", this->getStateString().c_str());
}

void
DNSCtx::setAddress ( uint32_t address )
{
	/* XXX: We should be recording all of the responses. */
	m_Address = address;
	m_Success = true;
}

/* DownloadCtx */

DownloadCtx::DownloadCtx ( Download *download, State state ) :
AttackSubCtx(state)
{
	m_Trigger		= download->getTriggerLine();
	m_URL			= download->getUrl();
	m_InitiatingSocketCtx	= NULL;
	m_Result		= RESULT_FAILED;
	m_SubmittingSocketCtx	= NULL;
}

void
DownloadCtx::setPayloadInfo ( Download *download, bool filtered )
{
	m_MD5 = download->getMD5Sum();
	m_SHA512 = download->getSHA512Sum();

	m_Result = ( filtered ? RESULT_FILTERED : RESULT_SUCCEEDED );
}

string
DownloadCtx::getResultString ( void )
{
	switch ( m_Result )
	{
	case RESULT_FAILED:
		return "FAILED";
		break;

	case RESULT_FILTERED:
		return "FILTERED";
		break;

	case RESULT_SUCCEEDED:
		return "SUCCEEDED";
		break;

	default:
		assert( false );
		return "?";
		break;
	}
}

bool
DownloadCtx::log ( FILE *outfile, struct config *config, bool wantTraffic )
{
	char *isockStr, *ssockStr;
	int retval;

	retval = asprintf(&isockStr, "%p", m_InitiatingSocketCtx);

	if ( retval == -1 )
		return false;

	retval = asprintf(&ssockStr, "%p", m_SubmittingSocketCtx);

	if ( retval == -1 )
	{
		free(isockStr);
		return false;
	}

	retval = fprintf(outfile, "Download|LUID=%p|Result=%s|Start=%ld.%ld|Finish=%ld.%ld|ISock=%s|SSock=%s|MD5=%s|SHA512=%s|Trigger=%s|URL=%s\n",
		this,
		this->getResultString().c_str(),
		m_Started.tv_sec,
		m_Started.tv_usec,
		m_Finished.tv_sec,
		m_Finished.tv_usec,
		m_InitiatingSocketCtx == NULL ? "" : isockStr,
		m_SubmittingSocketCtx == NULL ? "" : ssockStr,
		m_MD5.length()     ? m_MD5.c_str()     : "",
		m_SHA512.length()  ? m_SHA512.c_str()  : "",
		m_Trigger.length() ? m_Trigger.c_str() : "",
		m_URL.length()     ? m_URL.c_str()     : "");


	free(isockStr);
	free(ssockStr);

	if ( retval <= 0 )
		return false;

	return true;
}

void
DownloadCtx::logState ( void ) 
{
	logInfo("+ DownloadCtx: %s\n", this->getStateString().c_str());
}

void
DownloadCtx::setInitiatingSocketCtx ( SocketCtx *socketCtx )
{
	m_InitiatingSocketCtx = socketCtx;
}

void
DownloadCtx::setSubmittingSocketCtx ( SocketCtx *socketCtx )
{
	m_SubmittingSocketCtx = socketCtx;
}


/* ShellcodeCtx */

ShellcodeCtx::ShellcodeCtx ( SocketCtx *injectingSocketCtx, string md5sum, string trigger, bool knownAttack, AttackSubCtx::State state ) :
AttackSubCtx(state)
{
	m_InjectingSocketCtx	= injectingSocketCtx;
	m_MD5			= md5sum;
	m_KnownAttack		= knownAttack;
	m_SuccessfullyEmulated	= false;
	m_Trigger		= trigger;
}

bool
ShellcodeCtx::log ( FILE *outfile, struct config *config, bool wantTraffic )
{
	char *isockStr;
	int retval;

	retval = asprintf(&isockStr, "%p", m_InjectingSocketCtx);

	if ( retval == -1 )
		return false;

	retval = fprintf(outfile, "Shellcode|LUID=%p|Start=%ld.%ld|Finish=%ld.%ld|Type=%s|Emulation=%s|Handler=%s|ISock=%s|MD5=%s|Trigger=%s\n",
		this,
		m_Started.tv_sec,
		m_Started.tv_usec,
		m_Finished.tv_sec,
		m_Finished.tv_usec,
		m_KnownAttack ? "KNOWN" : "UNKNOWN",
		m_SuccessfullyEmulated ? "SUCCESS" : "FAILURE",
		m_HandlerName.c_str(),
		m_InjectingSocketCtx == NULL ? "" : isockStr,
		m_MD5.length()     ? m_MD5.c_str()     : "",
		m_Trigger.length() ? m_Trigger.c_str() : "");

	free(isockStr);

	if ( retval <= 0 )
		return false;

	return true;


}

void
ShellcodeCtx::logState ( void )
{
	logInfo("+ ShellcodeCtx: %s\n", this->getStateString().c_str());
}

void
ShellcodeCtx::setHandlerName ( string name )
{
	m_HandlerName = name;
}

void
ShellcodeCtx::setSuccessfullyEmulated ( bool value )
{
	m_SuccessfullyEmulated = value;
}


/* SocketCtx */

SocketCtx::SocketCtx ( Socket *socket, State state ) :
AttackSubCtx(state)
{
	m_BytesRX	= 0;
	m_BytesTX	= 0;
	m_ChunksRX	= 0;
	m_ChunksTX	= 0;
	m_Connected	= false;
	m_LocalHost	= socket->getLocalHost();
	m_LocalPort	= socket->getLocalPort();
	m_RemoteHost	= socket->getRemoteHost();
	m_RemotePort	= socket->getRemotePort();
	m_State		= state;

	switch ( socket->getType() & (ST_TCP|ST_UDP) )
	{
		case ST_TCP:
			m_Protocol = PROTO_TCP;
			break;

		case ST_UDP:
			m_Protocol = PROTO_UDP;
			break;

		default:
			assert( false );
			break;
	}

	switch ( socket->getType() & (ST_ACCEPT|ST_BIND|ST_CONNECT) )
	{
		case ST_ACCEPT:
			m_Connected = true;
			m_Type = TYPE_INCOMING;
			break;

		case ST_BIND:
			m_Type = TYPE_LISTENER;
			break;

		case ST_CONNECT:
			m_Type = TYPE_OUTGOING;
			break;

		default:
			assert( false );
			break;
	}
}

void
SocketCtx::addMessageRX ( Message *msg )
{
	m_BytesRX += msg->getSize();

	/*
	** Some events on the socket can cause zero-byte Messages to be
	** generated, which can indicate errors on the socket. We don't want to
	** count these as chunks.
	*/
	if ( msg->getSize() )
	{
		m_ChunksRX++;
		m_DataRX.add(msg->getMsg(), msg->getSize());
		m_DataOrder.push_back(msg->getSize() | DIRECTION_RX);
	}
}

void
SocketCtx::addMessageTX ( Message *msg )
{
	m_BytesTX += msg->getSize();
	m_ChunksTX++;
	m_DataTX.add(msg->getMsg(), msg->getSize());
	m_DataOrder.push_back(msg->getSize() | DIRECTION_TX);
}

bool
SocketCtx::log ( FILE *outfile, struct config *config, bool wantTraffic )
{
	string LocalHostString, RemoteHostString, RXhash, TXhash;
	int retval;

	LocalHostString	= inet_ntoa(*(in_addr *)&m_LocalHost);
	RemoteHostString= inet_ntoa(*(in_addr *)&m_RemoteHost);

	RXhash = g_Nepenthes->getUtilities()->md5sum(
		(char *) m_DataRX.getData(),
		m_DataRX.getSize()
	);

	TXhash = g_Nepenthes->getUtilities()->md5sum(
		(char *) m_DataTX.getData(),
		m_DataTX.getSize()
	);

	if ( wantTraffic )
		this->logTraffic(config, RXhash);

	retval = fprintf(outfile, "Socket|LUID=%p|Start=%ld.%ld|Finish=%ld.%ld|Status=%s|Proto=%s|Type=%s|Local=%s:%d|Remote=%s:%d|RX=%d,%d,%s|TX=%d,%d,%s|Dumpfile=%s\n",
		this,
		m_Started.tv_sec,
		m_Started.tv_usec,
		m_Finished.tv_sec,
		m_Finished.tv_usec,
		m_Connected ? "CONNECTED" : "UNCONNECTED",
		m_Protocol == PROTO_TCP ? "TCP" :
			m_Protocol == PROTO_UDP ? "UDP" :
			"?",
		m_Type == TYPE_INCOMING ? "INCOMING" :
			m_Type == TYPE_LISTENER ? "LISTENER" :
			m_Type == TYPE_OUTGOING ? "OUTGOING" :
			"?",
		LocalHostString.c_str(),
		m_LocalPort,
		RemoteHostString.c_str(),
		m_RemotePort,
		m_ChunksRX,
		m_BytesRX,
		RXhash.c_str(),
		m_ChunksTX,
		m_BytesTX,
		TXhash.c_str(),
		wantTraffic ? RXhash.c_str() : "");

	if ( retval <= 0 )
		return false;

	return true;
}

void
SocketCtx::logState ( void )
{
	logInfo("+ SocketCtx: %s\n", this->getStateString().c_str());
}

/*
** XXX: This is much slower than it could be.
*/
bool
SocketCtx::logTraffic ( struct config *config, string RXhash )
{
	Buffer data;
	uint32_t offsetRX = 0, offsetTX = 0;

	while ( ! m_DataOrder.empty() )
	{
		uint32_t chunkInfo = m_DataOrder.front();
		uint32_t chunkLength = chunkInfo & (~(DIRECTION_RX|DIRECTION_TX));
		uint32_t orderedChunkInfo = ntohl(chunkInfo);

		data.add(&orderedChunkInfo, sizeof(uint32_t));

		if ( chunkInfo & DIRECTION_RX )
		{
			data.add(m_DataRX.getData(offsetRX), chunkLength);
			offsetRX += chunkLength;
		}
		else if ( chunkInfo & DIRECTION_TX )
		{
			data.add(m_DataTX.getData(offsetTX), chunkLength);
			offsetTX += chunkLength;
		}
		else
		{
			assert(false);
		}

		m_DataOrder.pop_front();
	}

	assert( offsetRX == m_DataRX.getSize() );
	assert( offsetTX == m_DataTX.getSize() );

	return save_to_file(config->logdirConnData, RXhash, data.getData(), data.getSize());
}

void
SocketCtx::setConnected ( bool value )
{
	m_Connected = value;
}

void
SocketCtx::setLocalPort ( uint16_t port )
{
	assert(m_LocalPort == 0);
	m_LocalPort = port;
}


/* LogAttack */

LogAttack::LogAttack ( Nepenthes *nepenthes )
{
	g_Nepenthes = nepenthes;

	m_ModuleName = NAME;
	m_ModuleDescription = "Logs detailed attack information.";
	m_ModuleRevision = "$Rev$";
	m_Nepenthes = nepenthes;

	m_EventHandlerName = NAME "/EventHandler";
	m_EventHandlerDescription = "Tracks several event types for accounting and reporting.";

	m_AttackCtx		= NULL;
	m_SocketCtx		= NULL;
	m_CurrentLogFile	= NULL;

	m_AttackCtxsCount = 0;

	m_CurrentLogFilename.clear();
}

LogAttack::~LogAttack ( void )
{
}

bool
LogAttack::attemptAttackCtxClosure ( AttackCtx *attackCtx )
{
	if ( attackCtx->isComplete() )
	{
		/*
		logInfo("AttackCtx completed, destroying:\n");
		attackCtx->logState();
		*/

		if ( ! this->log(attackCtx) )
		{
			logWarn("Failed to log an attack context.\n");
		}

		delete attackCtx;
		m_AttackCtxsCount--;
		return true;
	}
	else
	{
		/*
		logInfo("AttackCtx still contains active (%d/%d) SubCtxs:\n",
			attackCtx->getActiveCtxCount(),
			attackCtx->getCtxCount());
		attackCtx->logState();
		*/

		return false;
	}
}

void
LogAttack::createNewSocketCtx ( Socket *socket, SocketCtx::State state, AttackCtx *attackCtx )
{
	SocketCtx *socketCtx = new SocketCtx(socket, state);

	assert(m_SocketCtxs[socket] == NULL);
	m_SocketCtxs[socket] = socketCtx;

	if ( attackCtx )
	{
		assert(m_AttackCtxs[socketCtx] == NULL);
		m_AttackCtxs[socketCtx] = attackCtx;
		attackCtx->addCtx(socketCtx);
		// attackCtx->logState();
	}

	// logInfo("Created new SocketCtx, %s AttackCtx\n", attackCtx == NULL ? "NO" : "with");
}

bool
LogAttack::closeCurrentLogFile ( void )
{
	if ( m_CurrentLogFile != NULL )
	{
		if ( fclose(m_CurrentLogFile) != 0 )
		{
			logCrit("%s: failed to close the current log file '%s': %s\n",
				NAME, m_CurrentLogFilename.c_str(), strerror(errno));
		}

		m_CurrentLogFile = NULL;
		m_CurrentLogFilename.clear();
	}

	return true;
}

void
LogAttack::closeSocketCtx ( Socket *socket )
{
	AttackCtx *attackCtx;
	SocketCtx *socketCtx;

	socketCtx = this->getSocketCtx(socket);

	if ( socketCtx == NULL )
	{
		if ( socket->isBind() )
		{
			/*
			** This can happen when a bind() fails, and the socket
			** is closed/destroyed, as we never get the bind event,
			** but we do get the close event.
			*/
			return;
		}
		else
		{
			/*
			** We are closing a socket that we don't have a context
			** for -- this should not ever happen.
			*/
			assert(false);
		}
	}

	m_SocketCtxs.erase(socket);
	socketCtx->setState(AttackSubCtx::STATE_INACTIVE);

	attackCtx = getAttackCtx(socketCtx);

	if ( attackCtx )
		this->attemptAttackCtxClosure(attackCtx);
	else
		delete socketCtx;

	m_AttackCtxs.erase(socketCtx);
}

bool
LogAttack::Init ()
{
	string logmode;

	m_ModuleManager = m_Nepenthes->getModuleMgr();

	if ( m_Config == NULL )
	{
		logCrit("%s: No configuration file present.", NAME);
		return false;
	}

	try
	{
		m_ConfigData.logAttack = m_Config->getValString("log-attack.log_attack");
		m_ConfigData.logdirConnData = m_Config->getValString("log-attack.logdir_connection_data");
		m_ConfigData.logdirFiles = m_Config->getValString("log-attack.logdir_files");
		logmode = m_Config->getValString("log-attack.logmode_connection_data");
	}
	catch ( ... )
	{
		logCrit("%s: there was an error obtaining the configuration.\n",
			NAME);
		return false;
	}

	/*
	** Initialize, normalize, and validate configuration.
	*/

	if ( logmode.compare("all") == 0 )
	{
		m_ConfigData.logModeConnData = LOG_ALL;
	}
	else if ( logmode.compare("limited") == 0 )
	{
		m_ConfigData.logModeConnData = LOG_LIMITED;
	}
	else if ( logmode.compare("none") == 0 )
	{
		m_ConfigData.logModeConnData = LOG_NONE;
	}
	else
	{
		logCrit("%s: the supplied logmode_connection_data was not valid.\n",
			NAME);

		return false;
	}

	if ( m_ConfigData.logAttack.compare(m_ConfigData.logAttack.size()-1, 1, "/") == 0 )
	{
		m_ConfigData.logModeAttack = LOG_TO_DIRECTORY;
	}
	else
	{
		m_ConfigData.logModeAttack = LOG_TO_FILE;
	}

	/*
	** Register required event types.
	*/

	m_Events.set(EV_DNS_QUERY_CREATED);
	m_Events.set(EV_DNS_QUERY_DESTROYED);
	m_Events.set(EV_DNS_QUERY_FAILURE);
	m_Events.set(EV_DNS_QUERY_SUCCESS);
	m_Events.set(EV_DNS_QUERY_STOP);
	m_Events.set(EV_DOWNLOAD);
	m_Events.set(EV_DOWNLOAD_DESTROYED);
	m_Events.set(EV_SHELLCODE);
	m_Events.set(EV_SHELLCODE_DONE);
	m_Events.set(EV_SHELLCODE_FAIL);
	m_Events.set(EV_SOCK_TCP_ACCEPT);
	m_Events.set(EV_SOCK_TCP_ACCEPT_STOP);
	m_Events.set(EV_SOCK_TCP_BIND);
	m_Events.set(EV_SOCK_TCP_CLOSE);
	m_Events.set(EV_SOCK_TCP_CONNECT);
	m_Events.set(EV_SOCK_TCP_CONNECT_REQ);
	m_Events.set(EV_SOCK_TCP_RX);
	m_Events.set(EV_SOCK_TCP_RX_STOP);
	m_Events.set(EV_SOCK_TCP_TX);
	m_Events.set(EV_SUBMISSION);
	m_Events.set(EV_SUBMISSION_DROPPED);
	m_Events.set(EV_TIMEOUT);

	REG_EVENT_HANDLER(this);

	m_Timeout = time(NULL) + SECONDS_PER_TICK;
	logInfo("%s: Loaded and initialized successfully.\n", NAME);

	return true;
}

bool
LogAttack::Exit ()
{
	this->closeCurrentLogFile();

	return true;
}

AttackCtx *
LogAttack::getAttackCtx ( AttackSubCtx *ctx )
{
        map < AttackSubCtx*, AttackCtx* >::iterator i;

	i = m_AttackCtxs.find(ctx);

        if ( i != m_AttackCtxs.end() )
                return i->second;

	return NULL;
}

DNSCtx *
LogAttack::getDNSCtx ( DNSQuery *query )
{
        map <DNSQuery*,DNSCtx*>::iterator i;

	i = m_DNSCtxs.find(query);

        if ( i != m_DNSCtxs.end() )
        {
                return i->second;
        }

        return NULL;
}

DownloadCtx *
LogAttack::getDownloadCtx ( Download *download )
{
        map <Download*,DownloadCtx*>::iterator i;

	i = m_DownloadCtxs.find(download);

        if ( i != m_DownloadCtxs.end() )
        {
                return i->second;
        }

        return NULL;
}

SocketCtx *
LogAttack::getSocketCtx ( Socket *socket )
{
        map < Socket *, SocketCtx * >::iterator i;

	i = m_SocketCtxs.find(socket);

        if ( i != m_SocketCtxs.end() )
        {
                return i->second;
        }

        return NULL;

}

uint32_t
LogAttack::handleEvent ( Event *event )
{
	AttackCtx *attackCtx;
	DNSCtx *dnsCtx;
	DNSEvent *queryEvent;
	DownloadCtx *downloadCtx;
	MessageEvent *messageEvent;
	ShellcodeEvent *sce;
	SocketCtx *socketCtx, *parentSocketCtx;
	SocketEvent *se;
	SubmitEvent *submitEvent;
	string md5sum;

	switch( event->getType() )
	{
		case EV_DNS_QUERY_CREATED:
			// logInfo("%s: EV_DNS_QUERY_CREATED\n", NAME);
			queryEvent = (DNSEvent *) event;
			
			assert( m_AttackCtx != NULL );

			dnsCtx = new DNSCtx(queryEvent->getQuery(), AttackSubCtx::STATE_ACTIVE);
			m_DNSCtxs[queryEvent->getQuery()] = dnsCtx;

			m_AttackCtx->addCtx(dnsCtx);
			m_AttackCtxs[dnsCtx] = m_AttackCtx;

			break;

		case EV_DNS_QUERY_DESTROYED:
			// logInfo("%s: EV_DNS_QUERY_DESTROYED\n", NAME);
			queryEvent = (DNSEvent *) event;

			dnsCtx = this->getDNSCtx(queryEvent->getQuery());
			assert( dnsCtx != NULL );

			attackCtx = this->getAttackCtx(dnsCtx);
			assert( attackCtx != NULL );

			m_DNSCtxs.erase(queryEvent->getQuery());
			dnsCtx->setState(AttackSubCtx::STATE_INACTIVE);

			this->attemptAttackCtxClosure(attackCtx);
			m_AttackCtxs.erase(dnsCtx);

			break;

		case EV_DNS_QUERY_FAILURE:
			// logInfo("%s: EV_DNS_QUERY_FAILURE\n", NAME);
			queryEvent = (DNSEvent *) event;

			dnsCtx = this->getDNSCtx(queryEvent->getQuery());
			assert( dnsCtx != NULL );

			this->setAttackCtx( this->getAttackCtx(dnsCtx) );

			break;

		case EV_DNS_QUERY_SUCCESS:
			// logInfo("%s: EV_DNS_QUERY_QUERY_SUCCESS\n", NAME);
			queryEvent = (DNSEvent *) event;

			dnsCtx = this->getDNSCtx(queryEvent->getQuery());
			assert( dnsCtx != NULL );

			dnsCtx->setAddress(queryEvent->getResult()->getIP4List().front());

			this->setAttackCtx( this->getAttackCtx(dnsCtx) );

			break;

		case EV_DNS_QUERY_STOP:
			// logInfo("%s: EV_DNS_QUERY_RESPONSE_STOP\n", NAME);

			assert( m_AttackCtx != NULL );
			this->setAttackCtx(NULL);

			break;

		case EV_DOWNLOAD:

			// logInfo("%s: EV_DOWNLOAD\n", NAME);
			submitEvent = (SubmitEvent *) event;

			assert( m_AttackCtx != NULL );
			assert( m_SocketCtx != NULL );

			downloadCtx = new DownloadCtx(submitEvent->getDownload(), AttackSubCtx::STATE_ACTIVE);
			m_DownloadCtxs[submitEvent->getDownload()] = downloadCtx;

			m_AttackCtx->addCtx(downloadCtx);
			m_AttackCtxs[downloadCtx] = m_AttackCtx;

			downloadCtx->setInitiatingSocketCtx(m_SocketCtx);

			break;

		case EV_DOWNLOAD_DESTROYED:

			// logInfo("%s: EV_DOWNLOAD_DESTROYED\n", NAME);
			submitEvent = (SubmitEvent *) event;

			downloadCtx = this->getDownloadCtx(submitEvent->getDownload());
			assert( downloadCtx != NULL );

			attackCtx = this->getAttackCtx(downloadCtx);
			assert( attackCtx != NULL );

			m_DownloadCtxs.erase(submitEvent->getDownload());
			downloadCtx->setState(AttackSubCtx::STATE_INACTIVE);

			this->attemptAttackCtxClosure(attackCtx);
			m_AttackCtxs.erase(downloadCtx);

			break;

		case EV_SHELLCODE:

			// logInfo("%s: EV_SHELLCODE\n", NAME);
			sce = (ShellcodeEvent *) event;

			assert( m_SocketCtx != NULL );
			sce = (ShellcodeEvent *) event;

			md5sum = g_Nepenthes->getUtilities()->md5sum(
				sce->getMessage()->getMsg(),
				sce->getMessage()->getSize());

			m_ShellcodeCtx = new ShellcodeCtx(m_SocketCtx, md5sum, sce->getTrigger(), sce->getKnown(), AttackSubCtx::STATE_ACTIVE);

			assert( m_AttackCtx != NULL );
			m_AttackCtx->addCtx(m_ShellcodeCtx);

			break;

		case EV_SHELLCODE_DONE:

			// logInfo("%s: EV_SHELLCODE_DONE\n", NAME);
			sce = (ShellcodeEvent *) event;

			assert( m_ShellcodeCtx != NULL );
			m_ShellcodeCtx->setSuccessfullyEmulated(true);
			m_ShellcodeCtx->setHandlerName(sce->getShellcodeHandler()->getShellcodeHandlerName());
			m_ShellcodeCtx->setState(AttackSubCtx::STATE_INACTIVE);
			m_ShellcodeCtx = NULL;

			break;

		case EV_SHELLCODE_FAIL:

			// logInfo("%s: EV_SHELLCODE_FAIL\n", NAME);
			sce = (ShellcodeEvent *) event;

			assert( m_ShellcodeCtx != NULL );

			m_ShellcodeCtx->setSuccessfullyEmulated(false);
			m_ShellcodeCtx->setState(AttackSubCtx::STATE_INACTIVE);
			m_ShellcodeCtx = NULL;

			break;

		case EV_SOCK_TCP_ACCEPT:

			// logInfo("%s: EV_SOCK_TCP_ACCEPT\n", NAME);
			se = (SocketEvent *) event;

			parentSocketCtx = getSocketCtx(se->getParentSocket());
			assert( parentSocketCtx != NULL );
			parentSocketCtx->setConnected(true);

			if ( ( attackCtx = this->getAttackCtx(parentSocketCtx) ) == NULL )
			{
				attackCtx = new AttackCtx();
				m_AttackCtxsCount++;
			}

			this->createNewSocketCtx(se->getSocket(), AttackSubCtx::STATE_ACTIVE, attackCtx);

			socketCtx = this->getSocketCtx(se->getSocket());
			assert( socketCtx != NULL );

			m_AttackCtx = attackCtx;
			m_SocketCtx = socketCtx;

			break;

		case EV_SOCK_TCP_ACCEPT_STOP:
			// logInfo("%s: EV_SOCK_TCP_ACCEPT_STOP\n", NAME);

			this->setAttackCtx(NULL);

			assert( m_SocketCtx != NULL );
			m_SocketCtx = NULL;

			break;

		case EV_SOCK_TCP_BIND:
			// logInfo("%s: EV_SOCK_TCP_BIND\n", NAME);
			se = (SocketEvent *) event;

			this->createNewSocketCtx(se->getSocket(), AttackSubCtx::STATE_ACTIVE, m_AttackCtx);

			break;

		case EV_SOCK_TCP_CLOSE:
			// logInfo("%s: EV_SOCK_TCP_CLOSE\n", NAME);
			se = (SocketEvent *) event;

			this->closeSocketCtx(se->getSocket());

			break;

		/*
		** A requested connection has succeeded.
		*/
		case EV_SOCK_TCP_CONNECT:
			// logInfo("%s: EV_SOCK_TCP_CONNECT\n", NAME);
			se = (SocketEvent *) event;

			socketCtx = this->getSocketCtx(se->getSocket());
			assert( socketCtx != NULL );
			socketCtx->setConnected(true);
			socketCtx->setLocalPort(se->getSocket()->getLocalPort());

			break;

		/*
		** A connection request has been made.
		*/
		case EV_SOCK_TCP_CONNECT_REQ:

			// logInfo("%s: EV_SOCK_TCP_CONNECT_REQ\n", NAME);

			/*
			** We should always have an active attack context when
			** requesting an outgoing connection.
			*/
			assert( m_AttackCtx != NULL );

			se = (SocketEvent *) event;

			this->createNewSocketCtx(se->getSocket(), AttackSubCtx::STATE_ACTIVE, m_AttackCtx);

			break;

		case EV_SOCK_TCP_RX:

			// logInfo("%s: EV_SOCK_TCP_RX\n", NAME);
			messageEvent = (MessageEvent *) event;

			m_SocketCtx = this->getSocketCtx(messageEvent->getMessage()->getSocket());
			assert( m_SocketCtx != NULL );

			m_SocketCtx->addMessageRX(messageEvent->getMessage());

			m_AttackCtx = this->getAttackCtx(m_SocketCtx);
			assert( m_AttackCtx != NULL );

			break;

		case EV_SOCK_TCP_RX_STOP:

			// logInfo("%s: EV_SOCK_TCP_RX_STOP\n", NAME);

			this->setAttackCtx(NULL);

			assert( m_SocketCtx != NULL );
			m_SocketCtx = NULL;

			break;

		case EV_SOCK_TCP_TX:

			// logInfo("%s: EV_SOCK_TCP_TX\n", NAME);
			messageEvent = (MessageEvent *) event;

			socketCtx = this->getSocketCtx(messageEvent->getMessage()->getSocket());
			assert( socketCtx != NULL );

			socketCtx->addMessageTX(messageEvent->getMessage());

			if ( this->getAttackCtx(socketCtx) == NULL )
			{
				logInfo("No attack context for EV_SOCK_TCP_TX!\n");
				assert(false);
			}

			break;

		case EV_SUBMISSION:
		case EV_SUBMISSION_DROPPED:

			// logInfo("%s: EV_SUBMISSION\n", NAME);
			submitEvent = (SubmitEvent *) event;

			downloadCtx = this->getDownloadCtx(submitEvent->getDownload());
			assert( downloadCtx != NULL );

			// We don't want traffic for this attack anymore.
			//
			downloadCtx->setWantTraffic(AttackSubCtx::WANT_NO);

			downloadCtx->setPayloadInfo(submitEvent->getDownload(),
				(event->getType() == EV_SUBMISSION_DROPPED) );

			/*
			** If the submission wasn't dropped, save the file, or
			** update the timestamp if it already exists.
			*/
			if ( event->getType() == EV_SUBMISSION )
			{
				FILE *out;
				int retval;
				string destination = m_ConfigData.logdirFiles + "/" + submitEvent->getDownload()->getMD5Sum();
				struct stat st;

				retval = mkdir(m_ConfigData.logdirFiles.c_str(), 0777);

				if ( retval != 0 )
					retval = errno;

				switch ( retval )
				{
				case 0:
				case EEXIST:
					break;
				default:
					logCrit("%s: Failed to create file log directory '%s' during EV_SUBMISSION: %s\n",
							NAME, m_ConfigData.logdirFiles.c_str(), strerror(errno));
					break;
				}

				retval = stat(destination.c_str(), &st);

				if ( retval != 0 )
					retval = errno;

				switch ( retval )
				{
				case 0:
					utimes(destination.c_str(), NULL);
					break;

				case ENOENT:
					if ( (out = fopen(destination.c_str(), "w+")) == NULL )
					{
						break;
					}

					if ( fwrite(submitEvent->getDownload()->getDownloadBuffer()->getData(),
							submitEvent->getDownload()->getDownloadBuffer()->getSize(),
							1, out) != 1 )
					{
						logCrit("%s: Failed to write to '%s' during EV_SUBMISSION: %s\n",
							NAME, destination.c_str(), strerror(errno));
					}

					retval = fclose(out);
					assert( retval == 0 );
					break;

				default:
					logCrit("%s: stat() failed during EV_SUBMISSION: %s\n",
						NAME, strerror(errno));
					break;
				}
			}

			/*
			** It's okay for m_SocketCtx to be NULL (it will be for
			** downloads that were UDP-based), as we don't
			** currently track context for all sockets.
			*/
			downloadCtx->setSubmittingSocketCtx(m_SocketCtx);

			break;

		case EV_TIMEOUT:

			m_Timeout = time(NULL) + SECONDS_PER_TICK;

			if ( m_AttackCtxsCount )
			{
				logInfo("%s: %d AttackCtxs, %d DNSCtxs, %d DownloadCtxs, %d SocketCtxs\n",
					NAME, m_AttackCtxsCount,
					m_DNSCtxs.size(),
					m_DownloadCtxs.size(),
					m_SocketCtxs.size());
			}

			break;

		default:
			assert( false );
	}

	return 0;
}

/*
** Log the supplied attack context.
*/
bool
LogAttack::log ( AttackCtx *ctx )
{
	char destination[4096];
	struct timeval current_time;

	/*
	** Figure out the current destination file.
	*/
	if ( gettimeofday(&current_time, NULL) != 0 )
		return false;

	strftime(destination, sizeof(destination), m_ConfigData.logAttack.c_str(),
		gmtime(&current_time.tv_sec));

	if ( m_ConfigData.logModeAttack == LOG_TO_DIRECTORY )
	{
		int retval;

		retval = mkdir(destination, 0777);

		if ( retval != 0 )
			retval = errno;

		switch ( retval )
		{
		case 0:
		case EEXIST:
			break;
		default:
			return false;
		}

		return this->logToDirectory(ctx, destination, current_time);
	}
	else
	{
		return this->logToFile(ctx, destination);
	}
}

bool
LogAttack::logToDirectory ( AttackCtx *ctx, string destination, struct timeval current_time )
{
	bool our_return = false;
	char *outname = NULL, *outname_tmp = NULL;
	int retval;

	retval = asprintf(&outname_tmp, "%s/%ld.%ld", destination.c_str(),
		current_time.tv_sec, current_time.tv_usec);

	if ( retval == -1 )
	{
		outname_tmp = NULL;
		goto out;
	}

	retval = asprintf(&outname, "%s.attack", outname_tmp);

	if ( retval == -1 )
	{
		outname = NULL;
		goto out;
	}

	if ( outname_tmp && outname )
	{
		FILE *out;

		if ( (out = fopen(outname_tmp, "w+")) == NULL )
			goto out;

		if ( ctx->log(out, &m_ConfigData) )
			our_return = true;

		retval = fclose(out);
		assert( retval == 0 );

		if ( rename(outname_tmp, outname) != 0 )
			our_return = false;
	}

out:

	if ( outname_tmp )
	{
		free(outname_tmp);
		outname_tmp = NULL;
	}

	if ( outname )
	{
		free(outname);
		outname = NULL;
	}

	return our_return;
}

bool
LogAttack::logToFile ( AttackCtx *ctx, string destination )
{
	const char *separator = "--\n";

	/*
	** If we have a log file open already, ensure that it matches
	** the current destination. Otherwise, invalidate it.
	*/
	if ( m_CurrentLogFilename.compare(destination) != 0 )
		this->closeCurrentLogFile();

	/*
	** If we don't have a log file open already, open it now.
	*/
	if ( m_CurrentLogFilename.size() == 0 )
	{
		if ( ( m_CurrentLogFile = fopen(destination.c_str(), "a") ) == NULL )
			return false;

		m_CurrentLogFilename = destination;
	}

	/*
	** Actually log the attack.
	*/
	bool our_return = ctx->log(m_CurrentLogFile, &m_ConfigData);

	if ( fwrite(separator, strlen(separator), 1, m_CurrentLogFile) != 1 )
		return false;

	if ( fflush(m_CurrentLogFile) != 0 )
		return false;

	return our_return;
}

void
LogAttack::setAttackCtx ( AttackCtx *ctx )
{
	if ( ctx == NULL )
	{
		m_AttackCtx = NULL;
	}
	else
	{
		if ( m_AttackCtx != NULL )
		{
			assert( m_AttackCtx == ctx );
		}
		else
		{
			m_AttackCtx = ctx;
		}
	}
}

extern "C" int32_t
module_init ( int32_t version, Module **module, Nepenthes *nepenthes )
{
	if ( version == MODULE_IFACE_VERSION )
	{
		*module = new LogAttack(nepenthes);
		return 1;
	}
	else
	{
		return 0;
	}
}

