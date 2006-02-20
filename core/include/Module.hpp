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

#ifndef HAVE_MODULE_HPP
#define HAVE_MODULE_HPP

#include <string>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;


namespace nepenthes
{

	class Config;
	class ModuleManager;
	class Nepenthes;


    class Module 
    {
    public:
        virtual ~Module(){};
        virtual bool Init()=0;
        virtual bool Exit()=0;

virtual void setConfig(Config *config)
{
	m_Config = config;
}


#ifdef WIN32
virtual void setDlHandle(HMODULE handle)
{
	m_DlHandle = handle;
}
#else
virtual void setDlHandle(void *handle)
{
	m_DlHandle = handle;
}
#endif


#ifdef WIN32
virtual HMODULE getDlHandle()
{
	return m_DlHandle;
}
#else
virtual void *getDlHandle()
{
	return m_DlHandle;
}
#endif


virtual Config *getConfig()
{
	return m_Config;
}

virtual string getModuleDescription()
{
	return m_ModuleDescription;
}

virtual string getModuleName()
{
	return m_ModuleName;
}
    protected:

		ModuleManager *m_ModuleManager;
		Nepenthes 	  *m_Nepenthes;

        string  m_ModuleName;
        string  m_ModuleDescription;
        string  m_ModuleRevision;

#ifdef WIN32
		HMODULE m_DlHandle;
#else
		/**
		 * the dlopen handle we get when opening the file with dlopen 
		 * and we need to dlcose() the module
		 */
        void    *m_DlHandle;
#endif

        Config  *m_Config;
    };


}






#endif
