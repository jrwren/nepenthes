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

#include <dlfcn.h>

#include "ModuleManager.hpp"
#include "Module.hpp"
#include "Config.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;
using namespace std;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod | l_mgr

/**
 * ModuleManager constructor
 * 
 * @param nepenthes the Nepenthes
 */
ModuleManager::ModuleManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}

/**
 * ModuleManager destructor
 */
ModuleManager::~ModuleManager()
{
	logPF();
	Exit();
}

/**
 * list all registerd modules
 */
void ModuleManager::doList()
{
	list <Module *>::iterator module;
	logSpam("=--- %-69s ---=\n","ModuleManager");
	int32_t i=0;
	for(module = m_Modules.begin();module != m_Modules.end();module++,i++)
	{
		logSpam("  %i) %-8s %s\n",i,(*module)->getModuleName().c_str(), (*module)->getModuleDescription().c_str());
	}
    logSpam("=--- %2i %-66s ---=\n\n",i, "Modules loaded");
}

/**
 * loads all modules found in the config
 * using the provided configfiles found in the config too
 * 
 * @return returns true if there was no error loading module or config, else false
 */
bool ModuleManager::Init()
{
	string sModuleDir;
	string sModuleConfDir;
	try {
		sModuleDir = m_Nepenthes->getConfig()->getValString("nepenthes.moduledir");
		sModuleConfDir = m_Nepenthes->getConfig()->getValString("nepenthes.moduleconfigdir");

		logSpam("Module     dir is %s \n",sModuleDir.c_str());
        logSpam("ModuleConf dir is %s \n",sModuleConfDir.c_str());
    } catch ( ... ) {
        logCrit("Could not find %s in config file\n","moduledir, moduleconfigdir");
        exit(-1);
    }

	StringList sList;
	sList = *m_Nepenthes->getConfig()->getValStringList("nepenthes.modules");

	
	uint32_t i = 0;
	while (i < sList.size())
	{
//        printf("Module %s\n",sList[i]);

		string sModulePath = sModuleDir + "/" + sList[i] ;
		i++;

        string sModuleConf;
        if (strlen(sList[i]) > 0) 
        {
            sModuleConf = sModuleConfDir + "/" + sList[i] ;
        }
		i++;
		string sScript;
		if (strlen(sList[i]) > 0) 
		{
			sScript = sModuleConfDir + "/" + sList[i] ;
		}
		i++;

		


		bool bModRet = registerModule(&sModulePath, &sModuleConf);
		try
		{
			if (  bModRet == false )
			{
				if(m_Nepenthes->getConfig()->getValInt("nepenthes.modulemanager.exit_on_broken_moduleload") == 1 )
				{
					logCrit("ERROR LOADING MODULE %s: SHUTTING DOWN\n",sModulePath.c_str());
					m_Nepenthes->stop();
					return false;
				}
			}            
		} catch ( ... )
		{
			logCrit("Could not find %s in config file\n","nepenthes.modulemanager.exit_on_broken_moduleload");
			m_Nepenthes->stop();
		}

		if (sScript.size() > 0)
		{
//			m_pCParent->m_pLuaInterface->runFile(&sScript);
		}
		
	}
	return true;
}

/**
 * unload all modules
 * 
 * @return 
 */
bool ModuleManager::Exit()
{
	while ( m_Modules.size() > 0 )
	{
		void *handle = m_Modules.front()->getDlHandle();
		m_Modules.front()->Exit();

		if ( m_Modules.front()->getConfig() != NULL )
		{
        	delete m_Modules.front()->getConfig();
		}

		delete m_Modules.front();
		dlclose(handle);
		m_Modules.pop_front();
	}
	return true;


}

/**
 * loads a module, and adds it to the Module list
 * 
 * @param modulepath the path to the module
 * @param configpath the path to the config file
 * 
 * @return returns true if there was no error, else false
 */
bool ModuleManager::registerModule(string *modulepath, string *configpath)
{
	bool retval=true;

    void *handle;

    typedef int32_t (*module_init_proc)(int32_t, Module**, Nepenthes *);
    module_init_proc module_init;

	handle = dlopen (modulepath->c_str(), RTLD_NOW|RTLD_LOCAL);

    if ( handle == NULL )
    {
        logCrit("Failed to load library \"%s\": %s\n", modulepath->c_str(), dlerror());
        return false;
    }

    module_init = (module_init_proc)dlsym(handle, "module_init");
    if ( module_init == NULL )
    {
        logCrit("Cannot obtain symbol \"module_init\" from \"%s\": %s\n", modulepath->c_str(), dlerror());
        dlclose (handle);
        return false;
    }


    Module *newmodule;
    if ( module_init (MODULE_IFACE_VERSION, &newmodule, m_Nepenthes) != 1 )
    {

        logCrit("Module \"%s\" library failed to initialize\n", modulepath->c_str());
        dlclose (handle);
        return false;
    }
    newmodule->setDlHandle(handle);

// fixme load config
	if(configpath->size() > 0)
	{

		Config *config = new Config;

		try
		{
			config->load(configpath->c_str());
		} catch( LoadError e )
		{
			printf("Unable to load configuration file %s: %s\n", configpath->c_str(), e.getMessage());
			retval = false;
		} catch( ParseError e )
		{
			printf("Parse error in %s on line %d: %s\n", configpath->c_str(), e.getLine(), e.getMessage());
			retval = false;
		}

		newmodule->setConfig(config);
	}else
		newmodule->setConfig(NULL);


	if ( newmodule->Init() == false )
	{
		logCrit("Module instance of \"%s\" using configuration \"%s\" failed to initialize\n", modulepath->c_str(), configpath->c_str());
		delete newmodule;
		dlclose (handle);
		return false;
	}

    m_Modules.push_back(newmodule);
//	logInfo("Added Module %s %s \n",pszModulePath, pNewModule->m_sModuleDir.c_str());
    return retval;
}

/**
 * deletes a module by module name
 * 
 * @param modulename the modules name
 * 
 * @return returns true if the module was found and could be removed, else false
 */
bool ModuleManager::unregisterModule(string *modulename)
{

	return true;
}


