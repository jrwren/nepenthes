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


#include "XMLRPCDialogue.hpp"
#include "submit-xmlrpc.hpp"

#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"

#include "Buffer.hpp"
#include "Buffer.cpp"

#include "Message.hpp"
#include "Message.cpp"

#include "Utilities.hpp"

#include "XMLRPCParser.hpp"
#include "XMLRPCContext.hpp"

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;



/*
HTTP/1.0 302 Found
Location: http://www.google.de/cxfer?c=PREF%3D:TM%3D1124725232:S%3DtUdZf7WJSCswHUUf&prev=/
Set-Cookie: PREF=ID=70e3a6c4694c9fcc:CR=1:TM=1124725232:LM=1124725232:S=8fh12WU7ov-yaVQE; expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com
Content-Type: text/html
Server: GWS/2.1
Content-Length: 214
Date: Mon, 22 Aug 2005 15:40:32 GMT
Connection: Keep-Alive

*/
/*
HTTPHeader::HTTPHeader(char *header, uint32_t size)
{
	if (strncmp(header,"HTTP",4) != 0)
	{
		logWarn("the provided data is not a valid http header %i bytes \n",size);
		g_Nepenthes->getUtilities()->hexdump((byte *)header,size);
		return;
	}

	char *start = header;
	char *stop = header;

    while (stop != header + size)
	{
        if(*stop == '\n')
		{
//			printf("Headerline '%.*s'\n",stop-start,start);
			stop++;
			m_HeaderLines.push_back(string(start,(uint32_t)(stop-start)));
			start = stop;
		}else
			stop++;
	}

//	logSpam("Header is \n\n%s\n",getHeader().c_str());
	parseHeaderLines();

}

HTTPHeader::~HTTPHeader()
{
	m_HeaderLines.clear();
	m_HeaderValues.clear();
}

const char *HTTPHeader::getValue(char *name)
{
	list <HTTPHeaderValue>::iterator it;
	for(it=m_HeaderValues.begin();it!=m_HeaderValues.end();it++)
	{
		if (it->m_Name == name)
		{
			return it->m_Value.c_str();
		}
	}
	return NULL;
}

string HTTPHeader::getHeader()
{
	string s;

	list <string>::iterator it;

	for(it=m_HeaderLines.begin();it!=m_HeaderLines.end();it++)
	{
		s+=*it;
	}
	return s;
}

void HTTPHeader::parseHeaderLines()
{
	list <string>::iterator it;
	for(it=m_HeaderLines.begin();it!=m_HeaderLines.end();it++)
	{

		string line = *it;

		uint32_t i=0;
		uint32_t nameoffset=0;
		string name = "";
		string value = "";
		while(i < line.size() )
		{

			if (line[i] == '\r' ) // drop \r
			{
                line[i] = ' ';
			}

			if (line[i] == ':')
			{
				if (nameoffset == 0)
				{
					
					name = line.substr(0,(int32_t)i);
					i+=2;	// skip ": "
					nameoffset=i;
				}
			}else
			if (line[i] == '\n')
			{
				if (nameoffset == 0)
				{
                    name = line.substr(0,i);
				}else
				{
//					i++;							// drop \n
					value = line.substr(nameoffset,i-nameoffset-1); 
				}
			}
			i++;
		}


		m_HeaderValues.push_back(HTTPHeaderValue(name,value));
//		printf("NAME \"%s\" -> \"%s\" \n",m_HeaderValues.back().m_Name.c_str(),value.c_str());
	}
	
}

HTTPHeaderValue::HTTPHeaderValue(string name, string value)
{
	m_Name = name;
	uint32_t i=0;
	while(i<m_Name.size())
	{
		m_Name[i] = tolower(m_Name[i]);
		i++;
	}
	m_Value = value;

}

HTTPHeaderValue::~HTTPHeaderValue()
{
}

*/
