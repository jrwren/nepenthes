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

#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include "LogManager.hpp"
#include "LogHandlerEntry.hpp"
#include "LogHandler.hpp"

using namespace nepenthes;

/**
 * creates a new log manager.
 */
LogManager::LogManager()
{
	for( int i = 0; i < MAX_TAGS; i++ )
		m_Tags[i] = 0;
}


/**
 * delete the logmanager, unregister and delete all attached loggers.
 */
LogManager::~LogManager()
{
	// unregister all loggers.
	list<LogHandlerEntry *>::iterator it;

	for( it = m_Loggers.begin(); it != m_Loggers.end(); it++ )
	{
//		delete (*it)->m_Lh;
		delete (*it);
	}
}


/**
 * add a new tag and bind it to a bit.
 * 
 * @param bit    the bit to which the tag will be bound.
 * @param tag    the tag name.
 */
void LogManager::registerTag(unsigned int bit, const char *tag)
{
	unsigned int i;

	for( i = 0; i < MAX_TAGS; i++ )
		if( (unsigned int)(1 << i) == bit )
			break;

	assert(i != MAX_TAGS); // wrong argument

	//printf("registered tag (index=%d) %d --> \"%s\"\n", i, (1 << i), tag);
	m_Tags[i] = tag;
}


/**
 * add a new logger with a specified filter.
 * 
 * @param tl         the logger.
 * @param filterMask filter mask, the logger will only receive messages with at least one of these tags.
 */
void LogManager::addLogger(LogHandler *lh, unsigned int filterMask)
{
	LogHandlerEntry *lhe = new LogHandlerEntry;

	lhe->m_Lh = lh;
	lhe->m_FilterMask = filterMask;

	m_Loggers.push_back(lhe);

	//printf("added log manager for these tags: ");

	for( unsigned int i = 0; i < MAX_TAGS; i++ )
		if( filterMask & (1 << i) )
		{
			assert(m_Tags[i]);
			printf("%s (%d) ", m_Tags[i], 1 << i);
		}

	printf("\n");
}


/**
 * write a log message.
 * 
 * @param mask    tags for this message.
 * @param message the message.
 */
void LogManager::log(unsigned int mask, const char *message)
{
	list<LogHandlerEntry *>::iterator it;

	// walk all loggers and log where desired.
	for( it = m_Loggers.begin(); it != m_Loggers.end(); it++ )
		if( (*it)->m_FilterMask & mask )
			(*it)->m_Lh->log(mask, message);
}


/**
 * log a message, accepting variable arguments.
 * 
 * @param mask   the mask for this message.
 * @param format format for the va message.
 */
void LogManager::logf(unsigned int mask, const char *format, ...)
{
	va_list		ap;


	va_start(ap, format);

#ifdef WIN32
	static char message[2048];
	memset(message,0,2048);
//    int len = vscprintf(format,ap);
    int len = 5;
//    printf("len is %i \n",len);
	vsprintf(message,format,ap);
	log(mask,message);

	va_end(ap);
#else
	char		*message;
	vasprintf(&message, format, ap);
	va_end(ap);

	log(mask, message);

	free(message);
#endif
}


/**
 * return the tag for a specified bit.
 * 
 * @param bit    the bit number.
 * 
 * @return the tag assigned to this bit.
 */
const char *LogManager::getTag(unsigned int bit)
{
	assert(m_Tags[bit]);
	return m_Tags[bit];
}
