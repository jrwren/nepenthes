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

#ifdef WIN32
#include <time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <errno.h>
#include "RingFileLogger.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;
using namespace std;


RingFileLogger::RingFileLogger(LogManager *lm) //: LogHandler(lm)
{
	m_FileFormat = 0;
	m_FirstFile = 0;
	m_MaxFiles = 0;
	m_MaxSize = 0;
	m_LogManager = lm;
}


RingFileLogger::~RingFileLogger()
{
	if (m_FileFormat != NULL)
	{
		free(m_FileFormat);
	}

	if (m_FirstFile != NULL)
	{
    	free(m_FirstFile);
	}
}


void RingFileLogger::setLogFileFormat(char *fmt)
{
	if (m_FileFormat != NULL)
	{
		free(m_FileFormat);
	}
	m_FileFormat = strdup(fmt);


	if (m_FirstFile != NULL)
	{
		free(m_FirstFile);
	}
	asprintf(&m_FirstFile, m_FileFormat, 0);
}

void RingFileLogger::setMaxFiles(uint8_t count)
{
	m_MaxFiles = count;
}

void RingFileLogger::setMaxSize(size_t size)
{
	m_MaxSize = size;
}

void RingFileLogger::rotate()
{
	int32_t i;
	static char filename[0xff], newfilename[0xff];

	snprintf(filename, sizeof(filename), m_FileFormat, m_MaxFiles - 1);
	unlink(filename);
	
	for( i = m_MaxFiles - 2; i >= 0; i-- )
	{
		snprintf(filename, sizeof(filename), m_FileFormat, i);
		snprintf(newfilename, sizeof(newfilename), m_FileFormat, i + 1);

		rename(filename, newfilename);
	}
}

void RingFileLogger::log(uint32_t mask, const char *message)
{
	if( !m_FileFormat || !m_MaxSize || !m_MaxFiles )
		return;

	FILE *f;

	if( !(f = fopen(m_FirstFile, "a")) )
		return;

	struct tm       t;
	time_t          stamp;
	time(&stamp);

#ifdef WIN32
	struct tm *pt =localtime(&stamp);
	memcpy(&t,pt,sizeof(struct tm));
#else
	localtime_r(&stamp, &t);
#endif


	string tag = "";
	for ( uint32_t i = 0; i < MAX_TAGS; i++ )
		if ( (1 << i) & mask  ) 
		{
			tag += " ";
			tag += m_LogManager->getTagName(i);
		}

    fprintf(f, "[%02d%02d%04d %02d:%02d:%02d%s] %s", t.tm_mday, t.tm_mon + 1, t.tm_year + 1900,
			 t.tm_hour, t.tm_min, t.tm_sec, tag.c_str(), message);

	struct stat s;

	fclose(f);

	// rotation needed?
	stat(m_FirstFile, &s);

	if( (uint32_t)s.st_size > m_MaxSize )
		rotate();
}
