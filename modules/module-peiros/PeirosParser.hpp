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


#ifndef __INCLUDE_PeirosParser_hpp
#define __INCLUDE_PeirosParser_hpp

#include <stdint.h>
#include <string>
#include <map>
#include <list>
using namespace std;


namespace peiros
{


struct PeirosStringComparator
{
	bool operator()(string a, string b)
	{
		return a < b;
	}
};


struct PeirosRequest
{
	string command, resource;
	map<string, string, PeirosStringComparator> headers;
	string appendedData;
	uint32_t appendedLength;
};


class PeirosParser
{
public:
	bool parseData(const char * buffer, uint32_t length);
	
	bool hasRequest();
	PeirosRequest getRequest();
	
	string renderRequest(PeirosRequest * request);
	
protected:
	bool parseRequest();
	bool parseCommand();
	bool parseHeaders();
	
private:
	string m_inBuffer;
	bool m_malformed;
	
	list<PeirosRequest> m_requests;
	PeirosRequest m_current;
};


}


#endif // __INCLUDE_PeirosParser_hpp
