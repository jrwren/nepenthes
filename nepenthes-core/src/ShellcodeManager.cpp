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
#include "SocketEvent.hpp"
#include "EventManager.hpp"

using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_sc | l_mgr


/**
 * ShellcodeManager constructor
 * 
 * @param nepenthes
 */
ShellcodeManager::ShellcodeManager(Nepenthes *nepenthes)
{
	m_Nepenthes = nepenthes;
}  

/**
 * ShellcodeManager destructor
 */
ShellcodeManager::~ShellcodeManager()
{

}  

/**
 * does nothing
 * 
 * @return true
 */
bool  ShellcodeManager::Init()
{
	return true;
}

/**
 * does nothing
 * 
 * @return true
 */
bool  ShellcodeManager::Exit()
{
	return true;
}

/**
 * lists registerd ShellcodeHandler 's
 */
void ShellcodeManager::doList()
{
	list <ShellcodeHandler *>::iterator shandler;
	logSpam("=--- %-69s ---=\n","ShellcodeManager");
	int32_t i=0;
	for(shandler = m_ShellcodeHandlers.begin();shandler != m_ShellcodeHandlers.end();shandler++,i++)
	{
		logSpam("  %i) %-8s %s\n",i,(*shandler)->getShellcodeHandlerName().c_str(), (*shandler)->getShellcodeHandlerDescription().c_str());
	}
    logSpam("=--- %2i %-66s ---=\n\n",i, "ShellcodeHandlers registerd");

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
	list <ShellcodeHandler *>::iterator shandler;
	for(shandler = m_ShellcodeHandlers.begin();shandler != m_ShellcodeHandlers.end();shandler++)
	{
		if (*shandler == handler)
		{
			logSpam("Removing %s\n",(*shandler)->getShellcodeHandlerName().c_str());
			m_ShellcodeHandlers.erase(shandler);
			return true;
		}
	}
	return false;
}  


/**
 * check a file for known shellcodes
 * 
 * @param nmsg   the file as Message
 * 
 * @return true on success,
 *         else false
 */
sch_result ShellcodeManager::fileCheck(Message **nmsg)
{
	list <ShellcodeHandler *>::iterator shandler;
	for(shandler = m_ShellcodeHandlers.begin();shandler != m_ShellcodeHandlers.end();)
	{
		sch_result res = (*shandler)->handleShellcode(nmsg);

		switch(res)
		{
		case SCH_DONE:
			printf("%-23s\t",(*shandler)->getShellcodeHandlerName().c_str());
			return SCH_DONE;

		case SCH_NOTHING:
			shandler++;
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
 * @param msg the shellcode we want to check as Message
 * @param trigger	Text describing the delivery mechanism (exploited
 *			vulnerability) for the shellcode.
 * @param known		True if we are certain the message is shellcode, and
 *			that a failure to process the shellcode is likely a bug
 *			in our emulation. False if only success is significant.
 * 
 * @return returns SCH_DONE on success, else SCH_NOTHING
 * 
 * 
 */
sch_result
ShellcodeManager::handleShellcode ( Message **msg, const char *trigger, bool known )
{
//	logDebug("SCHMGR Msg ptr is %x \n",(uint32_t )*msg);
	list <ShellcodeHandler *>::iterator shandler;

	if ( trigger == NULL )
		trigger = "Unknown";
	
//	list <ShellcodeHandler *>::iterator demseinemutter;
//	list <ShellcodeHandler *> notme;

	static Message **nmsg;
	static Message *nnmsg;
	nmsg = msg;
	nnmsg = *nmsg;

	{
		ShellcodeEvent se((*nmsg), NULL, trigger, known, EV_SHELLCODE);
		g_Nepenthes->getEventMgr()->handleEvent(&se);
	}

	for(shandler = m_ShellcodeHandlers.begin();shandler != m_ShellcodeHandlers.end();)
	{

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
			{
				ShellcodeEvent se((*nmsg), (*shandler), trigger, known, EV_SHELLCODE_DONE);
				g_Nepenthes->getEventMgr()->handleEvent(&se);
			}
			return SCH_DONE;

		case SCH_NOTHING:
			shandler++;
			break;

		case SCH_REPROCESS:
            shandler = m_ShellcodeHandlers.begin();
			nnmsg = *nmsg;
			logDebug("SCHMGR REPROCESS Msg ptr is %x \n",(uint32_t )((intptr_t)*msg));
			break;

		case SCH_REPROCESS_BUT_NOT_ME:
/*			notme.push_back(*shandler);
			shandler = m_ShellcodeHandlers.begin();
			nnmsg = *nmsg;
			logDebug("SCHMGR REPROCESS_BUT_NOT_ME Msg ptr is %x \n",(uint32_t )*msg);
*/			
            break;
			

		}
	}

	{
		ShellcodeEvent se((*nmsg), NULL, trigger, known, EV_SHELLCODE_FAIL);
		g_Nepenthes->getEventMgr()->handleEvent(&se);
	}

	return SCH_NOTHING;
}  
