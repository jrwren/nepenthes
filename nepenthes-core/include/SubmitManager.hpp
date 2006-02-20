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
#ifndef HAVE_SUBMITMANAGER_HPP
#define HAVE_SUBMITMANAGER_HPP

#include <list>

#ifdef WIN32
#else
#include <magic.h>
#endif

#include "Manager.hpp"

using namespace std;

#define REG_SUBMIT_HANDLER(handler) g_Nepenthes->getSubmitMgr()->registerSubmitter(handler)

namespace nepenthes
{
	class Nepenthes;
	class Download;
	class SubmitHandler;


    class SubmitManager : public Manager
    {
    public: 
        SubmitManager(Nepenthes *nepenthes);
        virtual ~SubmitManager();

        virtual void addSubmission(Download *down);
        virtual bool registerSubmitter(SubmitHandler *handler);

		bool Init();
		bool Exit();
		void doList();

    protected:
        list <SubmitHandler *> 	m_Submitters;
		list <string>			m_FileHashes;	// md5sums

#ifdef WIN32
#else
		magic_t		m_MagicCookie;
#endif
		bool 		m_StrictFileType;
    };


}
#endif
