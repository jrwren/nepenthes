/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2007 Georg Wicherski <gw@mwcollect.org>
 * Copyright (C) 2005 Paul Baecher & Markus Koetter
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

#include "submit-mwserv.hpp"

#include "LogManager.hpp"
#include "EventManager.hpp"

#include "POLLSocket.cpp"
#include "Socket.cpp"



#if defined(__GNUG__)
	#define MY_COMPILER "g++"
#elif defined(__CYGWIN__)
	#define MY_COMPILER "cygwin"
#else	
	#define MY_COMPILER "unknown Compiler"
#endif

#if defined(__FreeBSD__)
#  define MY_OS "FreeBSD"
#elif defined(linux) || defined (__linux)
#  define MY_OS "Linux"
#elif defined (__MACOSX__) || defined (__APPLE__)
#  define MY_OS "Mac OS X"
#elif defined(__NetBSD__)
#  define MY_OS "NetBSD"
#elif defined(__OpenBSD__)
#  define MY_OS "OpenBSD"
#elif defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
#  define MY_OS "Windows"
#elif defined(CYGWIN)
#  define MY_OS "Cygwin\Windows"
#else
#  define MY_OS "Unknown OS"
#endif

#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
#  define MY_ARCH "Alpha"
#elif defined(__arm__)
#  if defined(__ARMEB__)
#    define MY_ARCH "ARMeb"
#  else 
#    define MY_ARCH "ARM"
#  endif 
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL)
#  define MY_ARCH "x86"
#elif defined(__x86_64__) || defined(__amd64__)
#  define MY_ARCH "x86_64"
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64)
#  define MY_ARCH "Intel Architecture-64"
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
#  if defined(__mips32__) || defined(__mips32)
#    define MY_ARCH "MIPS32"
#  else 
#    define MY_ARCH "MIPS"
#  endif 
#elif defined(__hppa__) || defined(__hppa)
#  define MY_ARCH "PA RISC"
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(_M_PPC) || defined(__PPC) || defined(__PPC__)
#  define MY_ARCH "PowerPC"
#elif defined(__THW_RS6000) || defined(_IBMR2) || defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2)
#  define MY_ARCH "RS/6000"
#elif defined(__sparc__) || defined(sparc) || defined(__sparc)
#  define MY_ARCH "SPARC"
#else
#  define MY_ARCH "Unknown Architecture"
#endif



namespace nepenthes
{


TransferSession::TransferSession(Type type, SubmitMwservModule * parent)
{
	m_type = type;
	m_parent = parent;
	
	m_sample.binary = 0;	
	m_multiHandle = 0;	
	m_postInfo = m_postInfoLast = 0;
	m_curlHandle = 0;

	m_Type |= ST_NODEL;
}

void TransferSession::transfer(TransferSample& sample, string url)
{
	m_sample = sample;
	
	if(!(m_curlHandle = curl_easy_init()) || !(m_multiHandle =
		curl_multi_init()))
	{
		logCrit("%s failed!\n", __PRETTY_FUNCTION__);
		return;
	}
	
	m_targetUrl = url;
	m_sample = sample;
	
	initializeHandle();
}

TransferSession::~TransferSession()
{
	Exit();
}

void TransferSession::initializeHandle()
{
	m_postInfo = m_postInfoLast = 0;
	
	curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "guid",
			CURLFORM_COPYCONTENTS, m_sample.guid.c_str(), CURLFORM_END);
	curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME,
		"maintainer", CURLFORM_COPYCONTENTS, m_sample.maintainer.c_str(),
		CURLFORM_END);
	curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "secret",
		CURLFORM_COPYCONTENTS, m_sample.secret.c_str(), CURLFORM_END);
	
	if(m_type != TST_HEARTBEAT)
	{
		curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "url",
			CURLFORM_COPYCONTENTS, m_sample.url.c_str(), CURLFORM_END);
		curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "sha512",
			CURLFORM_COPYCONTENTS, m_sample.sha512.c_str(), CURLFORM_END);
		curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "saddr",
			CURLFORM_COPYCONTENTS, m_sample.saddr.c_str(), CURLFORM_END);
		curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "daddr",
			CURLFORM_COPYCONTENTS, m_sample.daddr.c_str(), CURLFORM_END);
		
		if(m_type == TST_SAMPLE)
		{
			curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "data",
				CURLFORM_PTRCONTENTS, m_sample.binary, CURLFORM_CONTENTSLENGTH,
				m_sample.binarySize, CURLFORM_END);
		}
	}
	else
	{
		curl_formadd(&m_postInfo, &m_postInfoLast, CURLFORM_PTRNAME, "software",
			CURLFORM_COPYCONTENTS, "nepenthes " VERSION " (" MY_OS ", " MY_ARCH
			", " MY_COMPILER ")", CURLFORM_END);
	}
	
	curl_easy_setopt(m_curlHandle, CURLOPT_HTTPPOST, m_postInfo);
	curl_easy_setopt(m_curlHandle, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYHOST, false);
	curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, m_targetUrl.c_str());
	curl_easy_setopt(m_curlHandle, CURLOPT_USERAGENT,
		"nepenthes " VERSION " (" MY_OS ", " MY_ARCH ", " MY_COMPILER ")");
	curl_easy_setopt(m_curlHandle, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(m_curlHandle, CURLOPT_WRITEFUNCTION,
		TransferSession::readData);
		
	CURLMcode error;
	
	if((error = curl_multi_add_handle(m_multiHandle, m_curlHandle)))
		logCrit("Error adding easy to multi: %s\n", curl_multi_strerror(error));
	
	int handles = 0;
		
	while(curl_multi_perform(m_multiHandle, &handles) ==
		CURLM_CALL_MULTI_PERFORM && handles);
}

//size_t function( void *ptr, size_t size, size_t nmemb, void *stream);
size_t TransferSession::readData(void *buffer, size_t s, size_t n, void *data)
{
	((TransferSession *) data)->m_buffer.append((const char *)buffer, s * n);	
	return s * n;
}

TransferSession::Status TransferSession::getTransferStatus()
{
	if(m_type != TST_HEARTBEAT)
	{
		if(m_buffer == "OK")
			return TSS_OK;
		else if(m_buffer == "UNKNOWN")
			return TSS_UNKNOWN;
		else
			return TSS_ERROR;
	}
	else
	{
		if(m_buffer.substr(0, 4) == "OK: ")
			return TSS_HEARTBEAT;
		else
			return TSS_ERROR;
	}
}

bool TransferSession::Init()
{
	return true;
}

bool TransferSession::Exit()
{
	if(m_multiHandle)
		curl_multi_remove_handle(m_multiHandle, m_curlHandle);
	
	if(m_postInfo)
		curl_formfree(m_postInfo);
	
	if(m_curlHandle)
		curl_easy_cleanup(m_curlHandle);
	
	if(m_multiHandle)
	{
		curl_multi_cleanup(m_multiHandle);
		m_multiHandle = 0;
	}
	
	if(m_sample.binary)
	{
		delete [] m_sample.binary;
		m_sample.binary = 0;
	}
	
	return true;
}

bool TransferSession::wantSend()
{		
	fd_set readSet, writeSet, errorSet;
	int maxFd = 0;
	CURLMcode error;
	FD_ZERO(&readSet); FD_ZERO(&writeSet); FD_ZERO(&errorSet);
	
	if((error = curl_multi_fdset(m_multiHandle, &readSet, &writeSet, &errorSet,
		&maxFd)))
	{
		logCrit("Obtaining write socket failed: %s\n",
			curl_multi_strerror(error));
		return false;
	}
	
	return FD_ISSET(maxFd, &writeSet);
}

int32_t TransferSession::doSend()
{
	return doRecv();
}

int32_t TransferSession::doRecv()
{		
	int handles = 0, queued = 0;
	
	while(curl_multi_perform(m_multiHandle, &handles) ==
		CURLM_CALL_MULTI_PERFORM && handles);
	
	CURLMsg * message;
		
	while((message = curl_multi_info_read(m_multiHandle, &queued)))
	{				
		if(message->msg == CURLMSG_DONE)
		{
			if(message->data.result)
			{
				logCrit("Connection to %s failed: %s [\"%s\"]\n",
					m_targetUrl.c_str(), curl_easy_strerror(message->
					data.result), m_buffer.c_str());
				
				if(m_type == TST_HEARTBEAT)
					m_parent->scheduleHeartbeat(DEFAULT_HEARTBEAT_DELTA);
				else
				{
					m_parent->retrySample(m_sample);
					m_sample.binary = 0;
				}
			}
			else
			{					
				switch(getTransferStatus())
				{
				case TransferSession::TSS_OK:
					logInfo("Transmitted %s to %s.\n", m_sample.url.c_str(),
						m_targetUrl.c_str());
					
					break;
				
				case TransferSession::TSS_UNKNOWN:
					logInfo("submit-mwserv: uploading data for %s\n",
						m_sample.url.c_str());
					
					m_parent->submitSample(m_sample);
					m_sample.binary = 0;
					
					break;
				
				case TransferSession::TSS_HEARTBEAT:
					{
						unsigned long delta = strtoul(m_buffer.substr(4).
							c_str(), 0, 0);
						logDebug("Next heartbeat in %u seconds.\n", delta);
						
						m_parent->scheduleHeartbeat(delta);
						
						break;
					}
				
				case TransferSession::TSS_ERROR:
					if(m_type == TST_HEARTBEAT)
						m_parent->scheduleHeartbeat(DEFAULT_HEARTBEAT_DELTA);
					
					logCrit("%s reported \"%s\"\n", m_targetUrl.c_str(),
						m_buffer.c_str());
					
					break;
				}
			}
			
			m_Type |= ~ST_NODEL;
			m_Status = SS_CLOSED;
		}
	}
	
	return 0;
}

int32_t TransferSession::getSocket()
{
	if(!m_multiHandle)
		return -1;

	fd_set readSet, writeSet, errorSet;
	int maxFd = 0;
	CURLMcode error;
	FD_ZERO(&readSet); FD_ZERO(&writeSet); FD_ZERO(&errorSet);
	
	if((error = curl_multi_fdset(m_multiHandle, &readSet, &writeSet, &errorSet,
		&maxFd)))
	{
		logCrit("Obtaining read socket failed: %s\n",
			curl_multi_strerror(error));
		return -1;
	}
	
	if(maxFd == -1)
		return -1;
	
	if(!FD_ISSET(maxFd, &readSet) && !FD_ISSET(maxFd, &writeSet) &&
		!FD_ISSET(maxFd, &errorSet))
	{
		logCrit("maxFd not in set: %i!\n", maxFd);
		return -1;
	}
	
	return maxFd;
}

int32_t TransferSession::getsockOpt(int32_t level, int32_t optname,
	void *optval, socklen_t *optlen)
{		
	return getsockopt(getSocket(), level, optname, optval, optlen);
}


}


bool TransferSession::checkTimeout()
{
	// if the connection is bad, give curl a chance to take care, so we can get rid of the connection
	if (getSocket() == -1)
		doRecv();
		
	return false;
}
