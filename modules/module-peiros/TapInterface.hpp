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
 
#include "POLLSocket.hpp"
#include "Message.hpp"
#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"


#include <string>


#ifndef __INCLUDE_TapInterface_hpp
#define __INCLUDE_TapInterface_hpp

using namespace nepenthes;

class TapEncapsulator
{
public:
	virtual ~TapEncapsulator() { }
	virtual void encapsulatePacket(const char * buffer, uint16_t length) = 0;
};

class TapInterface : public POLLSocket
{
public:	
	TapInterface();
	
	bool Init(uint32_t netmask, bool manageRoute);
	bool Exit();
	
	bool wantSend();
	
	virtual int32_t doSend();
	virtual int32_t doRecv();
	int32_t doWrite(char *msg, uint32_t len);
	
	bool addAddress(uint32_t address);
	void removeAddress(uint32_t address);
	
	void setEncapsulator(TapEncapsulator * encapsulator);
	int getSocket();
	virtual int32_t getsockOpt(int32_t level, int32_t optname,void *optval,socklen_t *optlen);
	
private:
	TapEncapsulator * m_encapsulator;
	int m_fd;
	uint32_t m_netmask;
	std::string m_devname;
	uint8_t m_hwaddr[6];
	bool m_manageRoute;
};

#endif // __INCLUDE_TapInterface_hpp
