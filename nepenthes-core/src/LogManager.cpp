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
#include "Nepenthes.hpp"

using namespace nepenthes;

/**
 * creates a new log manager.
 */
LogManager::LogManager()
{
	for( int32_t i = 0; i < MAX_TAGS; i++ )
		m_Tags[i] = 0;

	m_useColor = false;
}


/**
 * delete the logmanager, unregister and delete all attached loggers.
 */
LogManager::~LogManager()
{
	logPF();
	// unregister all loggers.
	list<LogHandlerEntry *>::iterator it;

	for( it = m_Loggers.begin(); it != m_Loggers.end(); it++ )
	{
//		delete (*it)->m_Lh;
		delete (*it);
	}
	
	m_Loggers.clear();
}


/**
 * add a new tag and bind it to a bit.
 * 
 * @param bit    the bit to which the tag will be bound.
 * @param tag    the tag name.
 */
void LogManager::registerTag(uint32_t bit, const char *tag)
{
	uint32_t i;

	for( i = 0; i < MAX_TAGS; i++ )
		if( (uint32_t)(1 << i) == bit )
			break;

	assert(i != MAX_TAGS); // wrong argument

	//printf("registered tag (index=%d) %d --> \"%s\"\n", i, (1 << i), tag);
	m_Tags[i] = tag;
}


/**
 * add a new logger with a specified filter.
 * 
 * @param lh         the LogHandler
 * @param filterMask filter mask, the logger will only receive messages with at least one of these tags.
 */
void LogManager::addLogger(LogHandler *lh, uint32_t filterMask)
{
	LogHandlerEntry *lhe = new LogHandlerEntry;

	lhe->m_Lh = lh;
	lhe->m_FilterMask = filterMask;

	m_Loggers.push_back(lhe);

	//printf("added log manager for these tags: ");

	for( uint32_t i = 0; i < MAX_TAGS; i++ )
		if( filterMask & (1 << i) )
		{
			assert(m_Tags[i]);
			
			#ifdef HAVE_DEBUG_LOGGING
			printf("%s (%d) ", m_Tags[i], 1 << i);
			#endif
		}
	
	#ifdef HAVE_DEBUG_LOGGING
	printf("\n");
	#endif
}


/**
 * write a log message.
 * 
 * @param mask    tags for this message.
 * @param message the message.
 */
void LogManager::log(uint32_t mask, const char *message)
{
	if ( m_Loggers.size() == 0)
	{
		printf("%s",message);
		return;
	}

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
void LogManager::logf(uint32_t mask, const char *format, ...)
{
	va_list		ap;


	va_start(ap, format);

#ifdef WIN32
	static char message[2048];
	memset(message,0,2048);
//    int32_t len = vscprintf(format,ap);
    int32_t len = 5;
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
 * return the tag name for a specified bit.
 * 
 * @param bit    the bit number.
 * 
 * @return the tag name assigned to this bit.
 */
const char *LogManager::getTagName(uint32_t bit)
{
	assert(m_Tags[bit]);
	return m_Tags[bit];
}

/**
 * Return the bit id for a specified tag name.
 * 
 * @param tag the tag name.
 * 
 * @return the bit to which the tag is assigned or
 * 			MAX_TAGS if the tag was not found.
 */
uint32_t LogManager::getTagId(const char *tag)
{
	uint32_t i;

	for( i = 0; i < MAX_TAGS; i++ )
	{
		if( m_Tags[i] && !strcmp(m_Tags[i], tag) )
			return i;
	}

	return i;
}


uint32_t LogManager::parseTagString(const char *tagString)
{
	char *str = strdup(tagString), *ptr, *tag;
	uint32_t mask = 0, tagId;

	ptr = str;

	while( (tag = strsep(&ptr, ",")) )
	{

		tagId = getTagId(tag);
		if( tagId != MAX_TAGS )
			mask |= (1 << tagId);
	}

	free(str);
	return mask;
}

void LogManager::setColor(bool setting)
{
	m_useColor = setting;
}

bool LogManager::getColorSetting()
{
	return m_useColor;
}


/**
 * Ensure file ownership for all attached loggers.
 *
 * @param user The desired username.
 * @param group The desired group.
 *
 * @return false if at least one logger failed, true otherwise.
 */
bool LogManager::setOwnership(int32_t uid, int32_t gid)
{
	list<LogHandlerEntry *>::iterator it;

	for ( it = m_Loggers.begin(); it != m_Loggers.end(); it++ )
		if ( !(*it)->m_Lh->setOwnership(uid, gid) )
			return false;

	return true;
}
