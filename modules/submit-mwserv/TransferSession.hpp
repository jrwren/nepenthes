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

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <string>
using namespace std;

#include "Nepenthes.hpp"
#include "Module.hpp"
#include "SubmitHandler.hpp"
#include "Download.hpp"

#include "POLLSocket.hpp"


namespace nepenthes
{


struct TransferSample
{	
	string guid;
	string maintainer;
	string secret;	
	
	string url;
	string saddr, daddr;
	string sha512;

	char * binary;
	unsigned int binarySize;
};


class SubmitMwservModule;

class TransferSession : public POLLSocket
{
public:
	enum Type
	{
		TST_INSTANCE,
		TST_SAMPLE,
		TST_HEARTBEAT,
	};
	
	TransferSession(Type type, SubmitMwservModule * parent);
	virtual ~TransferSession();
	
	enum Status
	{
		TSS_OK,
		TSS_UNKNOWN,
		TSS_HEARTBEAT,
		TSS_ERROR,
	};
	
	TransferSession::Status getTransferStatus();
	
	void transfer(TransferSample& sample, string url);	
	
	// POLLSocket	
	bool Init();
	bool Exit();

	bool wantSend();

	int32_t doSend();
	int32_t doRecv();
	int32_t getSocket();
	int32_t getsockOpt(int32_t level, int32_t optname,
		void *optval, socklen_t *optlen);
	bool checkTimeout();

protected:
	string m_targetUrl;
	TransferSample m_sample;
	
	CURL * m_curlHandle;
	CURLM * m_multiHandle;
	curl_httppost * m_postInfo, * m_postInfoLast;
	
	char * m_dataCopy;
	unsigned int m_dataSize;
	
	void initializeHandle();	
	void recreateWithSampleData();
	
	string m_buffer;
	
	Type m_type;
	SubmitMwservModule * m_parent;
	
	unsigned long m_heartbeatDelta;

private:
	static size_t readData(void *buffer, size_t size, size_t n, void *data);
};


}
