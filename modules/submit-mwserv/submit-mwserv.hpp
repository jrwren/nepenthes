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

#include "Nepenthes.hpp"
#include "Module.hpp"
#include "SubmitHandler.hpp"
#include "EventHandler.hpp"
#include "Download.hpp"

#include "TransferSession.hpp"


#define DEFAULT_HEARTBEAT_DELTA 30
#define MAX_HEARTBEAT_DELTA 300


using namespace std;

namespace nepenthes
{


class SubmitMwservModule : public Module , public SubmitHandler,
	public EventHandler
{
public:
	SubmitMwservModule(Nepenthes * nepenthes);
	
	bool Init();
	bool Exit();
	
	void Submit(Download * download);
	void Hit(Download * download);

	uint32_t handleEvent(Event *event);
	
	void submitSample(TransferSample& sample);	
	void retrySample(TransferSample& sample);
	void scheduleHeartbeat(unsigned long delta);

protected:
	string m_url, m_guid, m_maintainer, m_secret;
	uint32_t m_inTransfer;
};


}

extern nepenthes::Nepenthes *g_Nepenthes;
