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


#ifndef HAVE_NEPENTHES_HPP
#define HAVE_NEPENTHES_HPP


#ifdef WIN32
#include <windows.h>
#endif

typedef unsigned int uint;
typedef unsigned char byte;

/* nepenthes specific log tags */
#define l_crit		0x00000001
#define l_warn		0x00000002
#define l_debug		0x00000004
#define l_info		0x00000008
#define l_spam		0x00000010
#define l_net		0x00000020
#define l_script	0x00000040
#define l_shell		0x00000080
#define l_mem		0x00000100
#define l_sc		0x00000200		// shell code
#define l_dl		0x00000400		// download
#define l_mgr		0x00000800  	// manager 
#define l_hlr		0x00001000  	// handler
#define l_dia		0x00002000  	// dialogue
#define l_sub 		0x00004000  	// submit
#define l_ev		0x00008000		// event
#define l_mod		0x00010000		// module
#define l_stdtag 	0x00020000		// standard tag

#define l_all		( l_crit  | l_warn | l_debug | l_info | l_spam | l_net | l_script | l_shell | l_mem  | l_sc    | l_dl   | l_mgr  | l_hlr | l_dia | l_sub  | l_ev | l_mod | l_stdtag )
#define l_none		0x00000000


#define STDTAGS l_debug | l_stdtag

/* log shortcuts */
//#define DEBUG 1

#ifdef WIN32

//#define __PRETTY_FUNCTION__ "FOO
#define logWrite(mask, format, ...) g_Nepenthes->getLogMgr()->logf(mask,format, __VA_ARGS__)

#define logSpam(format, ...) logWrite(l_spam 	| STDTAGS , format, __VA_ARGS__)
#define logDebug(format, ...) logWrite(l_debug	| STDTAGS , format, __VA_ARGS__)
#define logInfo(format, ...) logWrite(l_info	| STDTAGS , format, __VA_ARGS__)
#define logWarn(format, ...) logWrite(l_warn	| STDTAGS , format, __VA_ARGS__)
#define logCrit(format, ...) logWrite(l_crit	| STDTAGS , format, __VA_ARGS__)
#define logPF() logInfo("<in %s>\n", __PRETTY_FUNCTION__)



#else

#ifndef DEBUG
#define logWrite(mask, format, ...) g_Nepenthes->getLogMgr()->logf(mask,format, __VA_ARGS__)
#else
#define logWrite(mask, format, ...) printf("%s:%i ",__FILE__,__LINE__); g_Nepenthes->getLogMgr()->logf(mask,format, __VA_ARGS__)
#endif 

#define logSpam(format, ...) logWrite(l_spam 	| STDTAGS , format, __VA_ARGS__)
#define logDebug(format, ...) logWrite(l_debug	| STDTAGS , format, __VA_ARGS__)
#define logInfo(format, ...) logWrite(l_info	| STDTAGS , format, __VA_ARGS__)
#define logWarn(format, ...) logWrite(l_warn	| STDTAGS , format, __VA_ARGS__)
#define logCrit(format, ...) logWrite(l_crit	| STDTAGS , format, __VA_ARGS__)
#define logPF() logInfo("<in %s>\n", __PRETTY_FUNCTION__)

#endif

namespace nepenthes
{

	class Config;
	class DownloadManager;
    class EventManager;
    class LuaInterface;
    class LogManager;
    class ModuleManager;
    class ShellcodeManager;
    class SubmitManager;
	class SocketManager;
	class Utilities;
	class DialogueFactoryManager;
	class DNSManager;
	class Message;


/**
 * the Nepenthes main class (singleton).
 */

    class Nepenthes
    {
    public:
        Nepenthes();
        virtual ~Nepenthes();
        
		virtual Config				*getConfig();
        virtual DownloadManager 	*getDownloadMgr();
        virtual EventManager    	*getEventMgr();
        virtual LuaInterface    	*getLua();
        virtual LogManager      	*getLogMgr();
        virtual ModuleManager   	*getModuleMgr();
        virtual ShellcodeManager 	*getShellcodeMgr();
        virtual SubmitManager   	*getSubmitMgr();
		virtual SocketManager 		*getSocketMgr();
		virtual Utilities			*getUtilities();
		virtual DialogueFactoryManager *getFactoryMgr();
		virtual DNSManager 			*getDNSMgr();
		virtual bool 				doLoop();
		virtual int 				run(int argc, char **argv);
		virtual bool				stop();
		virtual bool 				reloadConfig();

    private:
		Config 				*m_Config;
        DownloadManager 	*m_DownloadManager;
        EventManager    	*m_EventManager;
        LuaInterface    	*m_Lua;
        LogManager      	*m_LogManager;
        ModuleManager   	*m_ModuleManager;
        ShellcodeManager 	*m_ShellcodeManager;
        SubmitManager   	*m_SubmitManager;
		SocketManager   	*m_SocketManager;
		Utilities			*m_Utilities;
		DialogueFactoryManager *m_DialogueFactoryManager;
		DNSManager			*m_DNSManager;
		bool				m_running;

		uid_t				m_UID;
		gid_t				m_GID;
	protected:
		bool fileCheck(char *filecheckarg,int argc, int opti, char **argv);
		int fileCheck(const char *filename, Message **Msg);

		bool changeUser(char *user);
		bool changeUser();

		bool changeGroup(char *group);
		bool changeGroup();

		bool changeRoot(char *path);

    };
}

extern nepenthes::Nepenthes *g_Nepenthes;

void show_help(bool defaults);
void show_version();
void show_logo();
void show_info();
#endif

