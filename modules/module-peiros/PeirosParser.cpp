/*
 * Peiros, the ScriptGen & Argos & nepenthes coordination suite.
 * (c) 2006 by Institute Eurecom
 * Written by Georg Wicherski, <georg-wicherski@pixel-house.net>
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
 */

#include <ctype.h>
#include <stdio.h>
#include "PeirosParser.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#include "module-peiros.hpp"


namespace peiros
{


bool PeirosParser::parseData(const char * data, uint32_t length)
{
	logPF();
	m_inBuffer.append(data, length);	
	m_malformed = false;
	
	while(!m_malformed && parseRequest());
	
	return !m_malformed;
}

bool PeirosParser::parseRequest()
{
	logPF();
	if(m_current.command.empty())
	{		
		if(m_inBuffer.find("\r\n\r\n") == m_inBuffer.npos)
			return false;
			
		m_current.appendedLength = 0;
			
		if(!parseCommand() || !parseHeaders())
		{
			m_malformed = true;
			return false;
		}

		if(!m_current.appendedLength)
		{
			m_requests.push_back(m_current);
			return true;
		}
	}
	
	if(m_current.appendedLength)
	{	
		if(m_inBuffer.size() < m_current.appendedLength)
			return false;
			
		m_current.appendedData = m_inBuffer.substr(0, m_current.appendedLength);
		m_requests.push_back(m_current);
		m_inBuffer.erase(0, m_current.appendedLength);
	}
	
	m_current.command.erase();
	m_current.headers.clear();
	
	return true;
}

bool PeirosParser::parseCommand()
{
	logPF();
	const char * c = m_inBuffer.data();
	uint16_t cnt = 0;
	bool inResource = false;
	
	m_current.command.erase();
	m_current.resource.erase();
	
	while(* c != '\r')
	{
		if(* c == ' ')
		{				
			inResource = true;
		}
		else if(* c == '\n' || * c == '\t' || !isprint(* c))
		{
			return false;
		}
		else
		{
			if(inResource)
				m_current.resource.push_back(* c);
			else
				m_current.command.push_back(* c);
		}
		
		++c;
		++cnt;
	}
	
	if(* (++c) != '\n')
		return false;
	
	m_inBuffer.erase(0, cnt + 2);
	return true;
}

bool PeirosParser::parseHeaders()
{
	logPF();
	const char * c = m_inBuffer.data();
	uint16_t cnt = 0;
	string header, value;
	bool loop = true;
	enum
	{
		PPS_SEEK_HEADER,
		PPS_READ_HEADER,
		PPS_SEEK_VAL,
		PPS_READ_VAL,
		PPS_SEEK_NEWLINE,
	} status = PPS_SEEK_HEADER;
	
	while(loop)
	{
		if(!isprint(* c) && !isspace(* c))
			return false;
			
		switch(status)
		{
			case PPS_SEEK_HEADER:
				if(* c == '\r')
					loop = false;
				else if(!isspace(* c))
				{
					header.erase();
					header.push_back(* c);
					status = PPS_READ_HEADER;
				}
				
				break;
				
			case PPS_READ_HEADER:
				if(* c == ':')
					status = PPS_SEEK_VAL;
				else
					header.push_back(* c);
					
				break;
				
			case PPS_SEEK_VAL:
				if(!isspace(* c))
				{
					value.erase();
					value.push_back(*c);
					status = PPS_READ_VAL;
				}
				
				break;
				
			case PPS_READ_VAL:
				if(* c == '\r')
					status = PPS_SEEK_NEWLINE;
				else
					value.push_back(* c);
				
				break;
					
			case PPS_SEEK_NEWLINE:
				if(* c != '\n')
					return false;
					
				status = PPS_SEEK_HEADER;
				
				if(header == "Content-length")
					m_current.appendedLength = atoi(value.c_str());
				else				
					m_current.headers[header] = value;
		}
		
		++c;
		++cnt;
	}
	
	m_inBuffer.erase(0, cnt);
	
	if(m_inBuffer.substr(0, 1) != "\n")
		return false;
		
	m_inBuffer.erase(0, 1);
	return true;
}

bool PeirosParser::hasRequest()
{
	logPF();
	return !m_requests.empty();
}

PeirosRequest PeirosParser::getRequest()
{
	logPF();
	PeirosRequest res = m_requests.front();
	m_requests.pop_front();
	return res;
}


string PeirosParser::renderRequest(PeirosRequest * request)
{
	logPF();
	string result = request->command;
	
	if(!request->resource.empty())
		result += " " + request->resource;
		
	result += "\r\n";
	
	for(map<string, string, PeirosStringComparator>::iterator i = request->headers.begin(); i != request->headers.end(); ++i)
		result += i->first + ": " + i->second + "\r\n";
	
	if(!request->appendedData.empty())
	{
		char * lengthHeader;
		
		asprintf(&lengthHeader, "Content-length: %u\r\n", (int)request->appendedData.size());
		result += lengthHeader;
		free(lengthHeader);
	}
	
	result += "\r\n";
	
	if(!request->appendedData.empty())
		result += request->appendedData;
		
	return result;
}


}
