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

#ifndef LOGMANAGER_HPP
#define LOGMANAGER_HPP

#include <list>
#include <stdint.h>

namespace nepenthes
{
	#define MAX_TAGS 32
	
	using namespace std;
	
	
	class LogHandler;
	struct LogHandlerEntry;
	
	
	/**
	 * tag based logmanager; manages a set of LogHandler.
	 */
	class LogManager
	{
	public:
							LogManager();
		virtual 			~LogManager();
	
		void				registerTag(uint32_t bit, const char *tag);
		virtual void		addLogger(LogHandler *lh, uint32_t filterMask);
		virtual void		log(uint32_t mask, const char *message);
		virtual void		logf(uint32_t mask, const char *format, ...);
		const char			*getTagName(uint32_t bit);
		uint32_t			getTagId(const char *tag);
		virtual uint32_t			parseTagString(const char *tagString);
		void				setColor(bool setting);
		bool				getColorSetting();
		bool				setOwnership(int32_t uid, int32_t gid);

	private:
		bool				m_useColor;
		list<LogHandlerEntry *>	m_Loggers;
		const char			*m_Tags[MAX_TAGS]; // use vector instead?
	};
}


#endif
