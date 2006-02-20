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

#include "ShellcodeManager.hpp"
#include "ShellcodeHandler.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"
#include "Message.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_mgr


ShellcodeManager::ShellcodeManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}  

ShellcodeManager::~ShellcodeManager()
{

}  

bool  ShellcodeManager::Init()
{
	return true;
}

bool  ShellcodeManager::Exit()
{
	return true;
}

void ShellcodeManager::doList()
{
	list <ShellcodeHandler *>::iterator shandler;
	logInfo("=--- %-69s ---=\n","ShellcodeManager");
	int32_t i=0;
	for(shandler = m_ShellcodeHandlers.begin();shandler != m_ShellcodeHandlers.end();shandler++,i++)
	{
		logInfo("  %i) %-8s %s\n",i,(*shandler)->getShellcodeHandlerName().c_str(), (*shandler)->getShellcodeHandlerDescription().c_str());
	}
    logInfo("=--- %2i %-66s ---=\n\n",i, "ShellcodeHandlers registerd");

}

/**
 * register a shellcodehandler
 * 
 * @param handler a pointer to the handler to register
 * 
 * @return returns true if the handler was registerd successfully
 *         false if the handler was already registerd
 */
bool ShellcodeManager::registerShellcodeHandler(ShellcodeHandler *handler)
{
	logPF();
    m_ShellcodeHandlers.push_back(handler);
	return true;
}  

/**
 * unregister a shellcodehandler
 * 
 * @param handler the ptr to the handler to unregister
 * 
 * @return returns true if the handler was unregisterd
 *         false if there was no such handler
 */
bool ShellcodeManager::unregisterShellcodeHandler(ShellcodeHandler *handler)
{
	return true;
}  


sch_result ShellcodeManager::fileCheck(Message **nmsg)
{
	list <ShellcodeHandler *>::iterator shandler;
	for(shandler = m_ShellcodeHandlers.begin();shandler != m_ShellcodeHandlers.end();shandler++)
	{
		sch_result res = (*shandler)->handleShellcode(nmsg);

		switch(res)
		{
		case SCH_DONE:
			printf("%-23s\t",(*shandler)->getShellcodeHandlerName().c_str());
			return SCH_DONE;

		case SCH_NOTHING:
			break;

		case SCH_REPROCESS:
			printf("%s->",(*shandler)->getShellcodeHandlerName().c_str());
			shandler = m_ShellcodeHandlers.begin();
			
            break;

		case SCH_REPROCESS_BUT_NOT_ME:
			break;


		}
	}
	printf("%-23s\t","");
	return SCH_NOTHING;

}

/**
 * gives the shellcode to the registerd shellcodehandlers
 * 
 * @param shellcode the shellcode we want to check
 * @param len       the shellcodes len
 * 
 * @return returns 0 if there was a shellcodehandler who could use the shellcode
 *         else -1
 */
sch_result ShellcodeManager::handleShellcode(Message **msg)
{
	logDebug("SCHMGR Msg ptr is %x \n",(uint32_t )*msg);
	list <ShellcodeHandler *>::iterator shandler;

	
//	list <ShellcodeHandler *>::iterator demseinemutter;
//	list <ShellcodeHandler *> notme;

	static Message **nmsg;
	static Message *nnmsg;
	nmsg = msg;
	nnmsg = *nmsg;

	for(shandler = m_ShellcodeHandlers.begin();shandler != m_ShellcodeHandlers.end();shandler++)
	{
		if ((uint32_t )*nmsg == 0xffffffff )
		{
			logCrit("ERROR SCHMGR Msg ptr is %x %s ( pre  sch %x)\n",(uint32_t )*nmsg,(uint32_t )nnmsg, (uint32_t )*shandler);
			return SCH_NOTHING;
		}
/*		if (notme.size() > 0)
		{
			bool skip = false;
			for(demseinemutter = notme.begin();demseinemutter != notme.end();demseinemutter++)
			{
				if (*demseinemutter == *shandler)
				{
					skip = true;

				}
			}

			if (skip == true)
			{
				continue;
			}
		}
*/
		sch_result res = (*shandler)->handleShellcode(nmsg);

		switch(res)
		{
		case SCH_DONE:
			return SCH_DONE;

		case SCH_NOTHING:
			break;

		case SCH_REPROCESS:
            shandler = m_ShellcodeHandlers.begin();
			nnmsg = *nmsg;
			logDebug("SCHMGR REPROCESS Msg ptr is %x \n",(uint32_t )*msg);
			break;

		case SCH_REPROCESS_BUT_NOT_ME:
/*			notme.push_back(*shandler);
			shandler = m_ShellcodeHandlers.begin();
			nnmsg = *nmsg;
			logDebug("SCHMGR REPROCESS_BUT_NOT_ME Msg ptr is %x \n",(uint32_t )*msg);
*/			
            break;
			

		}
		if ((uint32_t )*nmsg == 0xffffffff )
		{
        	logCrit("ERROR SCHMGR Msg ptr is %x should be %x ( post sch %x)\n",(uint32_t )*nmsg,(uint32_t )nnmsg, (uint32_t )*shandler);
//			logCrit("FIXING THIS BULLSHIT %x -> %x\n",(uint32_t )*nmsg,(uint32_t )nnmsg);
//			nmsg = &nnmsg;
			return SCH_NOTHING;
		}
	}

	return SCH_NOTHING;
}  
