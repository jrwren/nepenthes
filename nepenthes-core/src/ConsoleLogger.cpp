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

#include <stdio.h>
#include <string>
#include "ConsoleLogger.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

#ifdef WIN32
#include <time.h>
#endif

using namespace nepenthes;
using namespace std;

enum LogLevel
{
    L_SPAM = 0,
	L_DEBUG,
	L_INFO,
	L_WARN,
	L_CRIT,
	
	NUM_LOGLEVELS // pseudo
	
};

const int g_ColorMap[NUM_LOGLEVELS] = {
	36, // spam
	32, // debug
	33, // info
	35, // warn
	31, // crit
};

ConsoleLogger::ConsoleLogger(LogManager *lm) //: LogHandler(lm)
{
	m_LogManager = lm;
}

ConsoleLogger::~ConsoleLogger()
{
}

void ConsoleLogger::log(unsigned int mask, const char *message)
{
//	printf("ConsoleLogger: (0x%08x) %s", mask, message);
	struct tm       t;
	time_t          stamp;
	time(&stamp);

#ifdef WIN32
	struct tm *pt =localtime(&stamp);
	memcpy(&t,pt,sizeof(struct tm));
#else
	localtime_r(&stamp, &t);
#endif


	int level = L_SPAM;
	if (mask & l_crit)
	{
		level = L_CRIT;
	} else
		if (mask & l_warn)
	{
		level = L_WARN;
	} else
		if (mask & l_info)
	{
		level = L_INFO;
	} else
		if (mask & l_debug)
	{
		level = L_DEBUG;
	} else
		if (mask & l_spam)
	{
		level = L_SPAM;
	}

	string tag = "";
	for ( unsigned int i = 0; i < MAX_TAGS; i++ )
		if ( (1 << i) & mask  ) 
		{
/*			if ( (1 << i) & l_crit || (1 << i) & l_warn  || (1 << i) & l_info || (1 << i) & l_debug || (1 << i) & l_spam )
				continue;*/
			tag += m_LogManager->getTag(i);
			tag += " ";
		}

#ifdef WIN32
    printf("%s", message);
        

#else
    printf("[ \033[%d;1m%-5s\033[0m] %s",
			 g_ColorMap[level], tag.c_str(), message);
#endif
}
