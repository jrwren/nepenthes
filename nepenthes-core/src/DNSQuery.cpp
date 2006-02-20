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

#include "DNSQuery.hpp"

using namespace nepenthes;


DNSQuery::DNSQuery(DNSHandler *handler, char *dns, void *obj)
{
	m_Handler = handler;
	m_DNS = dns;
	m_Object = obj;
}

DNSQuery::~DNSQuery()
{
}

DNSHandler *DNSQuery::getHandler()
{
	return m_Handler;
}

string DNSQuery::getDNS()
{
	return m_DNS;
}

#ifdef WIN32

#else
adns_query *DNSQuery::getADNS()
{
	return &m_ADNS_QUERY;
}
#endif

void *DNSQuery::getObject()
{
	return m_Object;
}
