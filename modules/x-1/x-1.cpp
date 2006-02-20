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

#include "x-1.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;



/**
 * the X1 constructor creates a new X1 Module
 * 
 * sets ModuleName 
 * sets ModuleDescription
 * sets ModuleRevision
 * 
 * this modules does nothing but load
 * 
 * @param nepenthes the Nepenthes
 */
X1::X1(Nepenthes *nepenthes)
{
	m_ModuleName        = "x-1";
	m_ModuleDescription = "eXample Module 1 -loading modules example-";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;
	g_Nepenthes = nepenthes;
}

X1::~X1()
{

}

/**
 * Module::Init()
 * 
 * as this Modules does nothing, nothing is done here
 * 
 * @return true
 */
bool X1::Init()
{
	m_ModuleManager = m_Nepenthes->getModuleMgr();
	return true;
}


/**
 * does nothing but return true
 * 
 * @return true
 */
bool X1::Exit()
{
	return true;
}



extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if (version == MODULE_IFACE_VERSION) {
        *module = new X1(nepenthes);
        return 1;
    } else {
        return 0;
    }
}
