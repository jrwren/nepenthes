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

#include <config.h>

#ifdef WIN32
#else
#include <unistd.h>
#include <getopt.h>
#endif /* WIN32 */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <ctype.h>

#ifdef HAVE_LIBCAP
#undef _POSIX_SOURCE
#include <sys/capability.h>
#endif

#include "Nepenthes.hpp"
#include "SocketManager.hpp"
#include "EventManager.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "ConsoleLogger.hpp"
#include "FileLogger.hpp"
#include "RingFileLogger.hpp"
#include "ModuleManager.hpp"
#include "ShellcodeManager.hpp"
#include "SubmitManager.hpp"
#include "Config.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "DNSManager.hpp"

#include "SQLManager.hpp"

#include "Message.hpp"


#ifdef STDTAGS
	#undef STDTAGS
	#define STDTAGS l_mgr
#endif	

using namespace nepenthes;



Nepenthes *g_Nepenthes;


Options::Options()
{
	m_runMode = runNormal;
	m_daemonize = false;
	m_verbose = false;
	m_setCaps = false;	
	m_ringLogger = false;
	m_color = colorAuto;

	m_fileCheckArguments = NULL;
	m_configPath = SYSCONFDIR "/nepenthes/nepenthes.conf";
	m_workingDir = PREFIX;
	m_changeUser = NULL;
	m_changeGroup = NULL;
	m_changeRoot = NULL;
	m_diskTags = NULL;
	m_consoleTags = NULL;
}


/**
 * the constructor
 * takes no argument
 * instances all managers
 * inits all managers
 */
Nepenthes::Nepenthes()
{
	g_Nepenthes = this;
	m_running = true;

	m_Config = NULL;
	m_DialogueFactoryManager    = NULL;
	m_DNSManager = NULL;
	m_DownloadManager   = NULL;
	m_EventManager  = NULL;
	m_LogManager    = new LogManager;
	m_ModuleManager = NULL;
	m_ShellcodeManager  = NULL;
	m_SocketManager = NULL;
	m_SubmitManager = NULL;
	m_Utilities = NULL;

	m_SQLManager = NULL;

	m_UID = 0;
	m_GID = 0;
}

/**
 * Nepenthes destuctor
 */
Nepenthes::~Nepenthes()
{
	logPF();

	if ( m_SocketManager != NULL )
		m_SocketManager->Exit();

	if (m_DownloadManager != NULL )
		delete m_DownloadManager;

	if (m_EventManager != NULL)
		delete m_EventManager;

	if (m_SubmitManager != NULL)
	{
		m_SubmitManager->Exit();
    	delete m_SubmitManager;
	}

	if (m_ModuleManager != NULL)
		delete m_ModuleManager;

	if (m_ShellcodeManager != NULL)
		delete m_ShellcodeManager;

	if (m_Utilities != NULL)
		delete m_Utilities;

	if (m_DialogueFactoryManager != NULL)
		delete m_DialogueFactoryManager;

	if (m_DNSManager != NULL)
		delete m_DNSManager;

	if (m_LogManager != NULL)
		delete m_LogManager;

	if ( m_SocketManager != NULL )
		delete m_SocketManager;

	g_Nepenthes = NULL;
	
	printf("Quit\n");
}

/**
 * start nepenthes, using command line arguments.
 * 
 * @param argc   number of arguments.
 * @param argv   vector containing arguments.
 * 
 * @return 0 if the application was shut down poperly, non-null if an error occured.
 */
bool Nepenthes::parseArguments(int32_t argc, char **argv, Options *options)
{
	while( 1 )
	{
		int32_t option_index = 0;
		static struct option long_options[] = {
            { "config", 		1, 0, 'c' },
			{ "capabilities",	0, 0, 'C' },
			{ "disk-log", 		1, 0, 'd' },
			{ "daemonize",		0, 0, 'D' },
			{ "file-check",		1, 0, 'f' },	
			{ "group",			1, 0, 'g' },	
			{ "help", 			0, 0, 'h' },
			{ "large-help",		0, 0, 'H' },
			{ "info",			0, 0, 'i' },
			{ "check-config", 	0, 0, 'k' },
			{ "log", 			1, 0, 'l' },	
			{ "logging-help",	0, 0, 'L' },	
			{ "color",	 		1, 0, 'o' },
			{ "chroot",			1, 0, 'r' }, 
			{ "ringlog",		0, 0, 'R' }, 
			{ "user",			1, 0, 'u' },	
			{ "version", 		0, 0, 'V' },
			{ "verbose", 		0, 0, 'v' },
			{ "workingdir", 	1, 0, 'w' },
			{ 0, 0, 0, 0 }
		};

		int32_t c = getopt_long(argc, argv, "c:Cd:Df:g:hHikl:Lo:r:Ru:vVw:", long_options, (int *)&option_index);
		if (c == -1)
			break;

		switch (c)
		{

		case 'C':

			options->m_setCaps = true;
			break;


		case 'c':
			options->m_configPath = optarg;
			break;

		case 'd':
			options->m_diskTags = optarg;
			break;

		case 'D':
			options->m_daemonize = true;	
            break;


		case 'f':
			options->m_fileCheckArguments = optarg;
			options->m_runMode = runFileCheck;
			break;


		case 'g':
			options->m_changeGroup = optarg;
            break;

		case 'h':
			show_help(false);
			return false;
			break;

		case 'H':
			show_help(true);
			return false;
			break;

		case 'i':
			show_info();
			return false;
			break;

		case 'l':
			options->m_consoleTags = optarg;
			break;

		case 'L':
			show_loghelp();
			return false;
			break;

		case 'k':
            options->m_runMode = runConfigCheck;
            break;

		case 'o':
			if ( !strcmp(optarg, "never") )
				options->m_color = colorNever;
			else if ( !strcmp(optarg, "always") )
				options->m_color = colorAlways;
			else if ( !strcmp(optarg, "auto") )
				options->m_color = colorAuto;
			else
			{
				fprintf(stdout, "Invalid argument for --color; must be one of\n"
						"`never', `always' or `auto'.\n");
				return false;
			}
			break;

		case 'r':
			options->m_changeRoot = optarg;
			break;

		case 'R':
			options->m_ringLogger = true;
			break;

		case 'u':
            options->m_changeUser = optarg;
			break;


		case 'v':
			options->m_verbose = true;
			break;

		case 'V':
			show_version();
			return false;
			break;
 
 		case 'w':
			options->m_workingDir = optarg;
			break;

		case '?':
		case ':':
			exit(0);
			break;

		default:
			break;
		}
	}

	return true;
}


/**
 * start nepenthes, using command line arguments.
 * 
 * @param argc   number of arguments.
 * @param argv   vector containing arguments.
 * 
 * @return 0 if the application was shut down poperly, non-null if an error occured.
 */

int32_t Nepenthes::run(int32_t argc, char **argv)
{
	Options             opt;


	if( !parseArguments(argc, argv, &opt) )
		return -1;


	if( opt.m_workingDir != NULL && chdir(opt.m_workingDir) )
	{
		logCrit("Cannot change working diretory to %s: %s.\n", opt.m_workingDir, strerror(errno));
		return -1;
	}

	if( opt.m_changeUser != NULL && !changeUser(opt.m_changeUser) )
		return -1;

	if( opt.m_changeGroup != NULL && !changeGroup(opt.m_changeGroup) )
		return -1;


	show_logo();
	show_version();



	m_LogManager->registerTag(l_crit,   "crit");
	m_LogManager->registerTag(l_warn,   "warn");
	m_LogManager->registerTag(l_debug,  "debug");
	m_LogManager->registerTag(l_info,   "info");
	m_LogManager->registerTag(l_spam,   "spam");
	m_LogManager->registerTag(l_net,    "net");
	m_LogManager->registerTag(l_script, "script");
	m_LogManager->registerTag(l_shell,  "shell");
	m_LogManager->registerTag(l_mem,    "mem");
	m_LogManager->registerTag(l_sc,     "sc");
	m_LogManager->registerTag(l_dl,     "down");
	m_LogManager->registerTag(l_mgr,    "mgr");
	m_LogManager->registerTag(l_hlr,    "handler");
	m_LogManager->registerTag(l_dia,    "dia");
	m_LogManager->registerTag(l_sub,    "submit");
	m_LogManager->registerTag(l_ev,     "event");
	m_LogManager->registerTag(l_mod,    "module");
	m_LogManager->registerTag(l_stdtag, "fixme");

	if ( opt.m_runMode != runFileCheck || opt.m_verbose )
	{

		if ( opt.m_consoleTags )
			m_LogManager->addLogger(new ConsoleLogger(m_LogManager), m_LogManager->parseTagString(opt.m_consoleTags));
		else
			m_LogManager->addLogger(new ConsoleLogger(m_LogManager), l_all);
	}


	if ( opt.m_runMode == runNormal || opt.m_runMode == runFileCheck )
	{
		m_DialogueFactoryManager = new DialogueFactoryManager(this);

		m_DownloadManager   = new DownloadManager(this);
		m_EventManager      = new EventManager(this);

		//	m_Lua				= new Lua
		m_ModuleManager     = new ModuleManager(this);
		m_ShellcodeManager  = new ShellcodeManager(this);
		m_SocketManager     = new SocketManager(this);
		m_SubmitManager     = new SubmitManager(this);
		m_Utilities         = new Utilities();
		m_DNSManager        = new DNSManager(this);
		m_SQLManager        = new SQLManager(this);
	}



	switch ( opt.m_color )
	{
	case colorAuto:
		if ( isatty(STDOUT_FILENO) )
			m_LogManager->setColor(true);
		else
			m_LogManager->setColor(false);
		break;

	case colorNever:
		m_LogManager->setColor(false);
		break;

	case colorAlways:
		m_LogManager->setColor(true);
		break;
	}



	/* load configuration. */
	m_Config = new Config;
	logSpam("Trying to load Nepenthes Configuration from %s \n", opt.m_configPath);
	try
	{
		m_Config->load(opt.m_configPath);
		logInfo("Loaded Nepenthes Configuration from \"%s\".\n", opt.m_configPath);
	}
	catch ( LoadError e )
	{
		fprintf(stderr, "Unable to load configuration file \"%s\": %s\n", opt.m_configPath, e.getMessage());
		return -1;
	}
	catch ( ParseError e )
	{
		fprintf(stderr, "Parse error in \"%s\" on line %d: %s\n", opt.m_configPath, e.getLine(), e.getMessage());
		return -1;
	}

	if ( opt.m_runMode == runConfigCheck ) // passed if we reach this
		return 0;


	if ( opt.m_runMode != runFileCheck || opt.m_verbose )
	{

		if ( opt.m_ringLogger == true )
		{
			string rlpath;
			try
			{
				rlpath = m_Config->getValString("nepenthes.logmanager.ring_logging_file");
			} catch ( ... )
			{
				logCrit("Could not find nepenthes.logmanager.ring_logging_file in Config\n");
				return (false);
			}


			RingFileLogger *fl = new RingFileLogger(m_LogManager);

			fl->setLogFileFormat((char *)rlpath.c_str());
			fl->setMaxFiles(5);
			fl->setMaxSize(1024 * 1024);

			if ( opt.m_diskTags )
				m_LogManager->addLogger(fl, m_LogManager->parseTagString(opt.m_diskTags));
			else
				m_LogManager->addLogger(fl, l_all);

		} else
		{
			string flpath;
			try
			{
				flpath = m_Config->getValString("nepenthes.logmanager.file_logging_file");
			} catch ( ... )
			{
				logCrit("Could not find nepenthes.logmanager.file_logging_file in Config\n");
				return (false);
			}

			FileLogger *fl = new FileLogger(m_LogManager);
			fl->setLogFile(flpath.c_str());
			if ( opt.m_diskTags )
				m_LogManager->addLogger(fl, m_LogManager->parseTagString(opt.m_diskTags));
			else
				m_LogManager->addLogger(fl, l_all);

		}
	}

	if (opt.m_daemonize == true)
	{
		logInfo("running as daemon\n");
		daemon(1,0);
		logInfo("daemon process id is %i\n",getpid());
	}


	if (opt.m_runMode == runFileCheck || opt.m_runMode == runNormal )
	{

        // socketManager will call WASStartup()
		if ( m_SocketManager->Init() == false)
			return -1;

		

		if ( m_ModuleManager->Init() == false )
			return -1;

		m_ModuleManager->doList();
		

		if (m_DNSManager->Init() == false )
			return -1;

		m_DNSManager->doList();
		

		if (m_DownloadManager->Init() == false )
			return -1;

        m_DownloadManager->doList();
		

		m_EventManager->doList();

		m_ShellcodeManager->doList();

        m_SocketManager->doList();
		

		if (m_SubmitManager->Init() == false)
			return -1;

			m_SubmitManager->doList();
		

		m_DialogueFactoryManager->doList();
		m_SQLManager->doList();
	}

	// if we drop priviliges, we have to take care of the logfiles user/group permission
	// if we do not drop privs, make sure the files are ours
	// --common


	if( !m_LogManager->setOwnership(m_UID, m_GID) )
		return -1;

	if ( setCapabilties() == false && opt.m_setCaps == true)
	{
		logCrit("As you asked to force capabilities, this is a critical error and we will terminate right now\n");
		return -1;
	}

	if (opt.m_changeRoot != NULL && changeRoot(opt.m_changeRoot) == false )
		return -1;


	// change process group id
	if ( opt.m_changeGroup != NULL &&  changeGroup() == false )
		return -1;

	// change process user id
	if ( opt.m_changeUser != NULL && changeUser() == false )
		return -1;


	if (opt.m_runMode == runFileCheck )
	{
		show_version();
		fileCheckMain(opt.m_fileCheckArguments,argc,optind,argv);
	}else
	{
        doLoop();
	}


	if (m_Config != NULL)
	{
    	delete m_Config;
	}
    return 0;
}



/**
 * the Nepenthes mainloop
 * 
 * @return should never return ;)
 */
bool Nepenthes::doLoop()
{

	while (m_running)
	{
    	getSocketMgr()->doLoop(500);
		getEventMgr()->doTimeoutLoop();
//		getDNSMgr()->pollDNS();
	}
	return true;
}



#define FC_DO_NONOP 0x001
#define FC_RM_NONOP 0x002
#define FC_RM_KNOWN 0x004

#define FC_IS_KNOWN 0x008
#define FC_IS_NONOP 0x010


bool Nepenthes::fileCheckMain(const char *optval, int32_t argc, int32_t opti, char **argv)
{
	if ( opti >= argc )
	{
		return true;
	}


	uint8_t options=0;
	if (optval != NULL)
	{
		if (strstr(optval,"rmnonop") != NULL)
		{
			options |= FC_RM_NONOP;
		}
		if (strstr(optval,"rmknown") != NULL)
		{
			options |= FC_RM_KNOWN;
		}
		if (strstr(optval,"dononop") != NULL)
		{
			options |= FC_DO_NONOP;
		}
	}

	printf ("Checking Files: \n");
	for ( ; opti < argc && m_running == true; )
	{

		struct stat fileinfo;
		if ( stat((const char*)argv[opti],&fileinfo) != 0 )
		{
			printf("failed\n");
			return -1;
		}

		if ( S_ISREG(fileinfo.st_mode)  )
		{
			
			uint8_t result = fileCheckPrinter(argv[opti],options);

			if (
				(options & FC_RM_KNOWN && result & FC_IS_KNOWN) ||
				(options & FC_RM_NONOP && result & FC_IS_NONOP) 
				)
				
			{
				if (unlink(argv[opti]) != 0)
				{
					printf("could not remove file %s (%s)\n",argv[opti],strerror(errno));
				}
			}

		}else
        if ( S_ISDIR(fileinfo.st_mode) )
		{
			DIR *bindir = opendir(argv[opti]);
			struct dirent *dirnode;
			string basepath = argv[opti];

			while ( (dirnode = readdir(bindir)) != NULL && m_running == true )
			{

#if defined(d_type_IS_NOT_A_POSIX_SPEC)
				if ( dirnode->d_type == 8 )
#else
				if (1)
#endif
				{
					string filepath = basepath + "/" + dirnode->d_name;
					uint8_t result = fileCheckPrinter(filepath.c_str(),options);

					if (
						(options & FC_RM_KNOWN && result & FC_IS_KNOWN) ||
						(options & FC_RM_NONOP && result & FC_IS_NONOP) 
						)
						
					{
						if (unlink(filepath.c_str()) != 0)
						{
							printf("could not remove file %s (%s)\n",filepath.c_str(),strerror(errno));
						}
					}
				}
			}
			closedir(bindir);
		}

		opti++;
	}
	return true;
}

uint8_t Nepenthes::fileCheckPrinter(const char *filename, uint8_t options)
{
	uint8_t result=0;

	Message *Msg=NULL;

	printf("\t %s\t",filename);
	switch ( fileCheck(filename,&Msg) )
	{
	case -1:
		printf("could not open file");
		break;

	case 0:
		if ( m_ShellcodeManager->fileCheck(&Msg) == SCH_DONE )
		{
			result |= FC_IS_KNOWN;
			printf("\033[32;1mKNOWN\033[0m");
		} else
		{
			printf("\033[31;1mFAILED\033[0m");
		}
		delete Msg;
		break;

	case 1:

		result |= FC_IS_NONOP;

		if ( options & FC_DO_NONOP )
		{
			if ( m_ShellcodeManager->fileCheck(&Msg) == SCH_DONE )
			{
				result |= FC_IS_KNOWN;
				printf("\033[32;1mKNOWN\033[0m");
			} else
			{
				printf("\033[31;1mFAILED\033[0m");
			}
		} else
		{
			printf("NONOP");
		}
		

		delete Msg;
		break;
	}
	printf("\n");
	return result;
}


/**
 * 
 * @param filename
 * @param Msg
 * 
 * @return -1 cound not open file
 *         0 cool
 *         1 no nop slide
 */
int32_t Nepenthes::fileCheck(const char *filename, Message **Msg)
{
    logPF();
	uint32_t filesize=0;
	unsigned char *buffer=NULL;
	struct stat fileinfo;
	int32_t retval = -1;

	if (stat((const char*)filename,&fileinfo) != 0)
	{
        return -1;
	}

		filesize=fileinfo.st_size;
		printf("%5i\t",filesize);
		buffer = (unsigned char *)malloc(fileinfo.st_size);

		FILE *f;
		if ( (f = fopen((const char *)filename,"rb")) == NULL )
		{
			return -1;
		} else
		{
			fread(buffer,1,filesize,f);
			fclose(f);

			uint32_t i;
			bool nopslide=false;
			for ( i=0;i<filesize;i++ )
			{
				if ( buffer[i] == 0x90 )
				{
					nopslide = true;
					break;
				}
			}
			if ( nopslide )
			{
				*Msg = new Message((char *)buffer, filesize, 0, 0, 0, 0, NULL, NULL);
				retval = 0;
			} else
			{
				*Msg = new Message((char *)buffer, filesize, 0, 0, 0, 0, NULL, NULL);
				retval = 1;
			}
		}
		free(buffer);
	return retval;
}



/**
 * get nepenthes Config
 * 
 * @return returns pointer to nepenthes Config
 */
Config *Nepenthes::getConfig()
{
	return m_Config;
}

/**
 * get DownloadManager
 * 
 * @return returns pointer to the DownloadManager
 */
DownloadManager *Nepenthes::getDownloadMgr()
{
	return m_DownloadManager;
}

/**
 * get EventManager
 * 
 * @return returns EventManager
 */
EventManager *Nepenthes::getEventMgr()
{
	return m_EventManager;
}

/**
 * get LogManager
 * 
 * @return returns LogManager
 */
LogManager *Nepenthes::getLogMgr()
{
	return m_LogManager;
}

LuaInterface *Nepenthes::getLua()
{
	return m_Lua;
}

/**
 * get ModuleManager
 * 
 * @return returns ModuleManager
 */
ModuleManager *Nepenthes::getModuleMgr()
{
	return m_ModuleManager;
}


/**
 * get ShellcodeManager
 * 
 * @return returns ShellcodeManager
 */
ShellcodeManager *Nepenthes::getShellcodeMgr()
{
	return m_ShellcodeManager;
}

/**
 * get SocketManager
 * 
 * @return returns SocketManager
 */
SocketManager *Nepenthes::getSocketMgr()
{
	return m_SocketManager;
}


/**
 * get SubmitManager
 * 
 * @return returns SubmitManager
 */
SubmitManager *Nepenthes::getSubmitMgr()
{
	return m_SubmitManager;
}

/**
 * get the famous Utilities
 * 
 * @return returns the Utilities
 */
Utilities *Nepenthes::getUtilities()
{
	return m_Utilities;
}

/**
 * get the DialogueFactoryManager
 * 
 * @return returns the DialogueFactoryManager
 */
DialogueFactoryManager *Nepenthes::getFactoryMgr()
{
	return m_DialogueFactoryManager;
}

/**
 * get the DNSManager
 * 
 * @return returns the DNSManager
 */
DNSManager *Nepenthes::getDNSMgr()
{
	return m_DNSManager;
}


/**
 * get the SQLManager
 * 
 * @return returns the SQLManager
 */
SQLManager *Nepenthes::getSQLMgr()
{
	return m_SQLManager;
}

/**
 * stop the nepenthes
 * 
 * @return returns true if running, else false
 */
bool Nepenthes::stop()
{
	if(m_running)
	{
    	m_running = false;
		return true;
	}
	return false;
}

/**
 * reload the config
 * 
 * FIXME does not work
 * exits nepenthes
 * 
 * @return returns false
 */
bool Nepenthes::reloadConfig()
{
	bool retval=true;
	while(retval)
	{
		retval = false;
		printf("no config yet\n");
		stop();
		return false;
	}
	return false;
}


/**
 * resolve a user to a uid_t
 * 
 * @param user   the username to resolve
 * 
 * @return returns true on success, 
 *         else false
 */
bool Nepenthes::changeUser(const char *user)
{
	passwd * pass;                                

	if (isdigit(*user) != 0)
	{
        m_UID = atoi(user);
		printf("User %s has uid %i\n",user,m_UID);
		return true;
	}else
	if ( (pass = getpwnam(user)) == NULL )
	{
		printf("Could not get userid for user '%s'\n",user);
		return false;
	}else
	{
		printf("User %s has uid %i\n",user,pass->pw_uid);
		m_UID = pass->pw_uid;
		return true;
	}

}



/**
 * this was intended to change the process userid 
 * 
 * to be *crossplattform* compatible, we had to add this shiny ifdef jungle
 * md said its taken from honeyd, so i guess its true
 * 
 * @return true if the userid was changed successfully, else false
 */
bool Nepenthes::changeUser()
{
#ifdef HAVE_SETRESUID
	if ( setresuid(m_UID, m_UID, m_UID) < 0 )
	{
		logCrit("Could not set User id %i for process \n%s\n",m_UID,strerror(errno));
		return false;
	} else
	{
		logInfo("Process userid %i\n",getuid());
		return true;
	}
#else

#ifdef HAVE_SETREUID
	if (setrugid(m_UID, m_UID) == -1)
	{
		logCrit("setreuid(%d) failed\n%s\n", m_UID, strerror(errno));
		return false;
	}else
	{
		logInfo("Process setreuid(%d) success\n", m_UID);
	}
#endif

#ifdef HAVE_SETEUID
	if (seteuid(m_UID) == -1)
	{
		logCrit("seteuid(%d) failed\n%s\n", m_UID, strerror(errno));
		return false;
	}else
	{
		logInfo("Process seteuid(%d) success\n", m_UID);
	}
#endif

	if (setuid(m_UID) == -1)
	{
		logCrit("setuid(%d) failed\n%s\n", m_UID, strerror(errno));
		return false;
	}else
	{
		logInfo("Process setuid(%d) success\n", m_UID);
	}
#endif // HAVE_SETRESUID

	return true;
}


/**
 * resolve a groupnames gid_t
 * 
 * @param gruppe the group tp resolve
 * 
 * @return returns true on success, else false
 */
bool Nepenthes::changeGroup(const char *gruppe)
{
	struct group *grp;

	if (isdigit(*gruppe) != 0)
	{
		m_GID = atoi(gruppe);
		printf("Group %s has gid %i\n",gruppe,m_GID);
		return true;
	}else
	if ( (grp = getgrnam(gruppe)) == NULL )
	{
		printf("Could not get groupid for group '%s' (%s)\n",gruppe,strerror(errno));
		return false;
	}else
	{
		printf("Group %s has gid %u\n",gruppe,grp->gr_gid);
		m_GID = grp->gr_gid;
		return true;
	}

}

/**
 * changes the process group id
 * 
 * md said the ifdef jungle is taken from honeyd
 * maybe somebody can tell me which operatingsystem lacks setresgid()
 * so i can blame it for the unreadable code
 * 
 * @return true if change worked, else false
 */
bool Nepenthes::changeGroup()
{
	

#ifdef HAVE_SETRESGID
	if ( setresgid(m_GID, m_GID, m_GID) < 0 )
	{
		logCrit("Could not set Group id %i for process \n%s\n",m_GID,strerror(errno));
		return false;
	} else
	{
		logInfo("Process groupid %i\n",getgid());
		return true;
	}
#else /* HAVE_SETRESGID */

#ifdef HAVE_SETGROUPS
	if ( setgroups(1, &m_GID) == -1 )
	{
		logCrit("setgroups(%d) failed\n%s\n", m_GID, strerror(errno));
		return false;
	}else
	{
		logInfo("Process setgroups(%d) success\n", m_GID);
	}
#endif /* HAVE_SETGROUPS */

#ifdef HAVE_SETREGID
	if ( setregid(m_GID, m_GID) == -1 )
	{
		logCrit("setregid(%d) failed\n%s\n", m_GID, strerror(errno));
		return false;
	}else
	{
		logInfo("Process setgroups(%d) success\n", m_GID);
	}
#endif /* HAVE_SETREGID */

	if ( setegid(m_GID) == -1 )
	{
		logCrit("setegid(%d) failed\n%s\n", m_GID, strerror(errno));
		return false;
	}else
	{
		logInfo("Process setegid(%d) success\n", m_GID);
	}

	if ( setgid(m_GID) == -1 )
	{
		logCrit("setgid(%d) failed\n%s\n", m_GID), strerror(errno);
		return false;
	}else
	{
		logInfo("Process setgid(%d) success\n", m_GID);
	}

#endif /* HAVE_SETRESGID */
	return true;
}

/**
 * change the process root
 * 
 * @param path   the path to chroot() to
 * 
 * @return return true on success,
 *         else false
 */
bool Nepenthes::changeRoot(const char *path)
{
	if ( chroot(path) < 0)
	{
		logCrit("Could not changeroot to %s \n%s\n",path,strerror(errno));
		return false;
	}else
	{
		logInfo("Changed root to %s\n",path);
		return true;
	}

}


bool Nepenthes::setCapabilties()
{
	logPF();

#ifdef HAVE_LIBCAP
	// set caps
	cap_t caps = cap_init();
	cap_value_t capList[6] =
	{ 
		CAP_SYS_CHROOT, 		// chroot()
		CAP_NET_BIND_SERVICE, 	// bind() ports < 1024 
		CAP_SETUID, 			// setuid()
		CAP_SETGID,				// setgid()
		CAP_NET_RAW,			// pcap
		CAP_NET_ADMIN			// route
	};

	unsigned num_caps = 6;

	cap_set_flag(caps, CAP_EFFECTIVE, 	num_caps, capList, CAP_SET);
	cap_set_flag(caps, CAP_INHERITABLE, num_caps, capList, CAP_SET);
	cap_set_flag(caps, CAP_PERMITTED, 	num_caps, capList, CAP_SET);

	if ( cap_set_proc(caps) )
	{
		cap_free(caps);
		logCrit("Could not set capabilities '%s'\n",strerror(errno));
		logCrit("Maybe you did not load the kernel module 'capability' ?\n");
		logCrit("Try 'modprobe capability' \n");
		return false;
	}
	cap_free(caps);

	// print caps
	caps = cap_get_proc();
	ssize_t y = 0;
	logInfo("The process %d was given capabilities %s\n",(int) getpid(), cap_to_text(caps, &y));
	fflush(0);
	cap_free(caps);

	return true;
#else
	logCrit("Compiled without support for capabilities, no way to run capabilities\n");
	return false;
#endif	// HAVE_LIBCAP

}



/**
 * the signalhandler
 * 
 * @param iSignal the signal we get
 */
void SignalHandler(int32_t iSignal)
{
	if ( g_Nepenthes != NULL )
		logWarn("Got signal %i ('%s')\n", iSignal,strsignal(iSignal));

	switch ( iSignal )
	{
	case SIGHUP:
		if ( g_Nepenthes != NULL )
		{
			logCrit("Got SIGHUP\nRereading Config File!\n\n");
			g_Nepenthes->reloadConfig();
		}
		break;

	case SIGINT:
		if ( g_Nepenthes != NULL )
		{
			logCrit("Got SIGINT\nStopping NOW!\n\n");
			g_Nepenthes->stop();
		}
		break;

	case SIGABRT:
		if ( g_Nepenthes != NULL )
			logCrit("Unhandled Exception\n");
		exit(-1);
		break;

	case SIGSEGV:
		if ( g_Nepenthes != NULL )
			logCrit("Segmentation Fault\n");
		exit(-1);
		break;

	case SIGBUS:
		if ( g_Nepenthes != NULL )
			logCrit("Bus Error\n");
		exit(-1);
		break;

	case SIGPIPE:
		break;

	case SIGCHLD:
		break;


	default:
		if ( g_Nepenthes != NULL )
		{
			logCrit("Exit 'cause of %i\n", iSignal);
			g_Nepenthes->stop();
		}
	}
}

/**
 * show the nepenthes logo
 */
void show_logo();

/**
 * the main
 * 
 * @param argc   argument count
 * @param argv   argument vector
 * 
 * @return returns 0 on success
 */
#if defined(CYGWIN)  || defined(CYGWIN32) || defined(__CYGWIN__) || defined(__CYGWIN32__)  || defined(WIN32)
int main(int32_t argc, char **argv)
{

	// win32 signals here
#else
int main(int32_t argc, char **argv)
{

/*         Signal     Handler                  Value     Action   Comment
 *	-------------------------------------------------------------------------
 *	First the signals described in the original POSIX.1 standard.
 */
	signal(SIGHUP,   SignalHandler);	//       1       Term    Hangup detected on controlling terminal or death of controlling process
	signal(SIGINT,   SignalHandler);	//       2       Term    Interrupt from keyboard
	signal(SIGQUIT,  SignalHandler);	//       3       Core    Quit from keyboard
	signal(SIGILL,   SignalHandler);	//       4       Core    Illegal Instruction
	signal(SIGABRT,  SignalHandler);	//       6       Core    Abort signal from abort(3)
	signal(SIGFPE,   SignalHandler);	//       8       Core    Floating point exception
//  signal(SIGKILL,  SignalHandler);	//       9       Term    Kill signal
	signal(SIGSEGV,  SignalHandler);	//      11       Core    Invalid memory reference

/* I hate breaking this well formatted list, 
 * but some systems lack
 * MSG_NOSIGNAL as send() option and 
 * SO_NOSIGPIPE as setsockopt() feature
 * So we would get sigpipe when sending data on a closed connection ...
 * it sucks, but we have to ignore sigpipe on such systems (f.e. OpenBSD 3.8)
 */
#if !defined(HAVE_SO_NOSIGPIPE) && !defined(HAVE_MSG_NOSIGNAL)
	signal(SIGPIPE,  SIG_IGN);	        //      13       Term    Broken pipe: write to pipe with no readers
#else
	signal(SIGPIPE,  SignalHandler);	//      13       Term    Broken pipe: write to pipe with no readers
#endif
										// 
	signal(SIGALRM,  SignalHandler);	//      14       Term    Timer signal from alarm(2)
	signal(SIGTERM,  SignalHandler);	//      15       Term    Termination signal
//	signal(SIGUSR1,  SignalHandler);	//   30,10,16    Term    User-defined signal 1
//	signal(SIGUSR2,  SignalHandler);	//   31,12,17    Term    User-defined signal 2
	signal(SIGCHLD,  SignalHandler);	//   20,17,18    Ign     Child stopped or terminated
	signal(SIGCONT,  SignalHandler);	//   19,18,25            Continue if stopped
//	signal(SIGSTOP,  SIG_IGN	  );	//   17,19,23    Stop    Stop process
//	signal(SIGTSTP,  SIG_IGN	  );	//   18,20,24    Stop    Stop typed at tty
	signal(SIGTTIN,  SignalHandler);	//   21,21,26    Stop    tty input for background process
	signal(SIGTTOU,  SignalHandler);	//   22,22,27    Stop    tty output for background process

/*
 *	Next the signals not in the POSIX.1 standard but described in SUSv2 and SUSv3 / POSIX 1003.1-2001.
 */

	signal(SIGBUS,   SignalHandler);	//   10,7,10     Core    Bus error (bad memory access)
	
#ifdef HAVE_SIGPOLL
	signal(SIGPOLL,  SignalHandler);	//               Term    Pollable event (Sys V). Synonym of SIGIO
#endif
	signal(SIGPROF,  SignalHandler);	//   27,27,29    Term    Profiling timer expired
	signal(SIGSYS,   SignalHandler);	//   12,-,12     Core    Bad argument to routine (SVID)
	signal(SIGTRAP,  SignalHandler);	//      5        Core    Trace/breakpoint trap

	signal(SIGURG,   SignalHandler);	//   16,23,21    Ign     Urgent condition on socket (4.2 BSD)
	signal(SIGVTALRM,SignalHandler);	//   26,26,28    Term    Virtual alarm clock (4.2 BSD)
	signal(SIGXCPU,  SignalHandler);	//   24,24,30    Core    CPU time limit exceeded (4.2 BSD)
	signal(SIGXFSZ,  SignalHandler);	//   25,25,31    Core    File size limit exceeded (4.2 BSD)
/*
 *	Next various other signals.
 */
	signal(SIGIOT,   SignalHandler);	//         6     Core    IOT trap. A synonym for SIGABRT
//  signal(SIGEMT, 	 SignalHandler);	//       7,-,7   Term
#ifdef HAVE_SIGSTKFLT
	signal(SIGSTKFLT,SignalHandler);	//    -,16,-     Term    Stack fault on coprocessor (unused)
#endif
	signal(SIGIO,    SignalHandler);	//   23,29,22    Term    I/O now possible (4.2 BSD)
#ifdef HAVE_SIGCLD
	signal(SIGCLD,   SignalHandler);	//    -,-,18     Ign     A synonym for SIGCHLD
#endif
#ifdef HAVE_SIGPWR
	signal(SIGPWR,   SignalHandler);	//   29,30,19    Term    Power failure (System V)
#endif
#ifdef HAVE_SIGINFO
	signal(SIGINFO,  SignalHandler);	//    29,-,-             

#endif
//  signal(SIGLOST,  SignalHandler);	//     -,-,-     Term    File lock lost
	signal(SIGWINCH, SIG_IGN      );	//   28,28,20    Ign     Window resize signal (4.3 BSD, Sun)
#ifdef HAVE_SIGUNUSED
	signal(SIGUNUSED,SignalHandler);	//    -,31,-     Term    Unused signal (will be SIGSYS)
#endif
#endif
	
	Nepenthes *nepenthes = new Nepenthes();
	int retval = nepenthes->run(argc, argv);
	delete nepenthes;
	printf("run is done %i\n",retval);
	return retval;
}

void show_logo()
{
	puts("#                                                                             #\n" \
		 "                                                                               \n" \
		 "                 !¦..,('                                                       \n" \
		 "                 !!..,¦,                                                       \n" \
		 "                 !*'.,!*                                                       \n" \
		 "                 *!'.,!¦                                                       \n" \
		 "                 '!'.,!¦                                                       \n" \
		 "                 .¦,',*¦,,.                                                    \n" \
		 "            ,#=%(#%#%%#%%%C                                                    \n" \
		 "         .#%C=,:::::::,,5%#.                                                   \n" \
		 "         (#!!,,,,,,,,,,,J7#'                                                   \n" \
		 "         =#!(=,,,,,,,,:==(7!                       ,%###!##%'.                 \n" \
		 "         .=#(CJ3$#%C7==¦((77                     ,%##%%!#%%%#%*                \n" \
		 "       .#(#J#A3#$==7=J=!¦(=3'                 ,%##%%::::,,::%#%, '!(====((!,'. \n" \
		 "     ######$@53C=(¦¦¦¦==(¦=C7                %##(::::,,,,,::%##A$$3$CCJ3CJC(7C!\n" \
		 "   #(###3#'JÐ%J(¦!!!*!!J(((=J'              ,##(:::,,,,,:::%##5%C'           . \n" \
		 " .#3#3#$#. C5$C7¦!=¦¦¦!7=(¦(7=              ;##(:,,,,,,,::%###%.               \n" \
		 " '####*.   7AJC7¦!7¦(¦¦!¦((=7C.             |###((,,,,,,,)%##%=C,    .*¦(7!    \n" \
		 "           (%7C77!(==!¦!¦¦=7=C!             ((%############%!(CC¦.,¦JAA%3C'    \n" \
		 "           ¦$C7C7=¦(=¦¦¦((=(=J= ,:%%@M;%%;.77C7(=C!¦!*!¦=!¦C(777J3$3=!,'.      \n" \
		 "           ,%CJJ=C(7(((((=((=JC 7####::###CC=(CC=(=¦(*!=!=¦=7=77JJ,            \n" \
		 "           .%J337J==(=7==C=C7C3,##J!::::77#%J=CJ==¦¦¦=!=¦===(7C7C*             \n" \
		 "            C$33C3C7=7C777CJJJ%7##C:,,,,,:#$3CJC==(=7(¦¦¦(=C(J$CJ*             \n" \
		 "            '%$333CCC3CCCCCJ%3%#%#$C:,,,:3#AJJ3C7=JC=(((((7C7C3J3!             \n" \
		 "             (#$$3JJJ%3J$J33$$AA33#5J:,,:A#$$$JC7CJCC7=(=(CJ$CJJ$,             \n" \
		 "              J#%$$%33%$5$A#$%#%CJ3#A%:35#%J3$3JJCCC$C7J77JC$J33$'             \n" \
		 "              .75AA%%%$AA%#A%5#%CJC#J###3#J$%3$JJCCJCC3J33$33$$A.              \n" \
		 "                =5555A%AA5#5@%,%CJC3J###ACJJ$5$33JJJJJJJJ%%%3%%A%              \n" \
		 "                .3@55##A5###='!AJCJJ3###ACJ3A#$$$%$$33333$%%%AA5!              \n" \
		 "                 %3J%5Ð5$J='  '533CC3###ACJ$#@A%%%AA$A%$$A%AA55¦               \n" \
		 "                 A=(3$A'.      3A3J33###%CJ$Ð#AA55AA%5#AA%%5#5¦                \n" \
		 "                .AC=¦$J        'A%$J$A##A33AAÐ555###55#5555#A!                 \n" \
		 "                *J7==A¦         '$%$%%##A3A=.JA###555%55##5(.                  \n" \
		 "                =JJJJ5'          '(A5A#JA%!   *=Ð@#%AA5553¦                    \n" \
		 "                J$3J$$             .55A$#*     .#A$AÐ%7*                       \n" \
		 "               .$33$%C              %$JJ5.     !3J(%¦.                         \n" \
		 "               '%%%A$7              %CC75.     C(J7(                           \n" \
		 "               *A%J$A=              %==73     ,J7C$'                           \n" \
		 "               7%$JJ@!             'A7=CC     =C7CJ                            \n" \
		 "               $J3(3#.             *$77C=     %(CC(                            \n" \
		 "              .#33($J              ¦7C73,    '$JJC,                            \n" \
		 "                                                                               \n" \
		 "#                                                                             #\n" \
		 "                                                                               \n" \
		 "                              Nepenthes Ampullaria                             \n" \
		 "                                                                               \n" \
		 "#                                                                             #\n");
}

/**
 * show cli flags help 
 * 
 * @param defaults include default values
 */
void show_help(bool defaults)
{
	typedef struct 
	{
		char *m_shortOpt;
		char *m_longOpt;
        char *m_Description;
		char *m_Default;
	} helpstruct;

	helpstruct myopts[]=
	{
        {"c",	"config=FILE",		"use FILE as configuration file",				SYSCONFDIR "/nepenthes.conf"	},
		{"C",	"capabilities",		"force kernel 'security' capabilities",	0						},
		{"d",	"disk-log",			"disk logging tags, see -L",			0						},
		{"D",	"daemonize",		"run as daemon",						0						},
		{"f",	"file-check=OPTS",	"check file for known shellcode, OPTS can\n"
			"                              be any combination of `rmknown' and\n"
			"                              `rmnonop'; seperate by comma when needed",   0			},
		{"h",	"help",				"display help",							0						},
		{"H",	"large-help",		"display help with default values",		0						},
		{"i",	"info",		   		"how to contact us",					0						},
		{"k",	"check-config",		"check configuration file for syntax errors",		0			},
		{"l",	"log",				"console logging tags, see -L",			0						},
		{"L",	"logging-help",		"display help for -d and -l",			0						},
		{"o",	"color=WHEN",		"control color usage. WHEN may be `never',\n"
			"                              `always' or `auto'",					"`auto'"		},
        {"r",	"chroot=DIR",		"chroot to DIR after startup",				"don't chroot"		},
		{"R",	"ringlog",			"use ringlogger instead of filelogger",			"filelogger"	},
		{"u",	"user=USER",				"switch to USER after startup",	"keep current user"},
		{"g",	"group=GROUP",			"switch to GROUP after startup (use with -u)", "keep current group"},
		{"V",	"version",			"show version",							""						},
		{"w",	"workingdir=DIR",		"set the process' working dir to DIR",			PREFIX		},
	};
	show_version();

	for ( uint32_t i=0;i<sizeof(myopts)/sizeof(helpstruct);i++ )
	{
		printf("  -%s, --%-19s %s\n", myopts[i].m_shortOpt,
			myopts[i].m_longOpt,
			myopts[i].m_Description);
		
		if( defaults == true && myopts[i].m_Default )
		{
			printf("                              Default value/behaviour: %s\n", myopts[i].m_Default);
		}
	}
}


#if defined(__GNUG__)
	#define MY_COMPILER "g++"
/*
	#if ( __GNUC__ == 4 && __GNUC_MINOR__ == 0  && __GNUC_PATCHLEVEL__ <= 2 ) // g++ 4 detection
		#error "MAKE SURE TO READ THIS"
		#error "g++ 4 has bugs, dont use it" 
		#error "nepenthes would compile using g++4, but it will segfault while running"
		#error "refer to http://nepenthes.sourceforge.net/documentation:readme:faq:gcc_4 for more information"
		#error "if you got no other compiler feel free to remove this section by commenting it out"
		#error "but don't complain when it does not work"
		#error "you can find it in Nepenthes.cpp around line 1569, just search for 'g++ 4 detection'"
	#endif
*/	
#elif defined(__CYGWIN__)
	#define MY_COMPILER "cygwin"
#else	
	#define MY_COMPILER "unknown Compiler"
#endif


#if defined(__FreeBSD__)
#  define MY_OS "FreeBSD"
#elif defined(linux) || defined (__linux)
#  define MY_OS "Linux"
#elif defined (__MACOSX__) || defined (__APPLE__)
#  define MY_OS "Mac OS X"
#elif defined(__NetBSD__)
#  define MY_OS "NetBSD"
#elif defined(__OpenBSD__)
#  define MY_OS "OpenBSD"
#elif defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
#  define MY_OS "Windows"
#elif defined(CYGWIN)
#  define MY_OS "Cygwin\Windows"
#else
#  define MY_OS "Unknown OS"
#endif


#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
#  define MY_ARCH "Alpha"
#elif defined(__arm__)
#  if defined(__ARMEB__)
#    define MY_ARCH "ARMeb"
#  else 
#    define MY_ARCH "ARM"
#  endif 
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL)
#  define MY_ARCH "x86"
#elif defined(__x86_64__) || defined(__amd64__)
#  define MY_ARCH "x86_64"
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64)
#  define MY_ARCH "Intel Architecture-64"
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
#  if defined(__mips32__) || defined(__mips32)
#    define MY_ARCH "MIPS32"
#  else 
#    define MY_ARCH "MIPS"
#  endif 
#elif defined(__hppa__) || defined(__hppa)
#  define MY_ARCH "PA RISC"
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(_M_PPC) || defined(__PPC) || defined(__PPC__)
#  define MY_ARCH "PowerPC"
#elif defined(__THW_RS6000) || defined(_IBMR2) || defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2)
#  define MY_ARCH "RS/6000"
#elif defined(__sparc__) || defined(sparc) || defined(__sparc)
#  define MY_ARCH "SPARC"
#else
#  define MY_ARCH "Unknown Architecture"
#endif

void show_loghelp()
{
	show_info();
	printf("\nAll Nepenthes log messages have certain tags.\n");
	printf("You can specify filter rules with -d and -l\n");
	printf("with a filter string. A filter string is a set of\n");
	printf("tags seperated by comma. A message needs to have at\n");
	printf("least one tag of the filter string you specify, i.e.\n");
	printf("bin/nepenthes -l \"foo,bar\" would require either `foo' or\n");
	printf("`bar' to appear within the log message. Messages\n");
	printf("which don't have one of these tags will be discarded.\n");
}

/**
 * show version
 */
void show_version()
{
	struct utsname sysinfo;
	int i = uname(&sysinfo);

	printf("\n");
	printf("Nepenthes Version %s \n",VERSION);
	printf("Compiled on %s/%s at %s %s with %s %s \n",MY_OS,MY_ARCH,__DATE__, __TIME__,MY_COMPILER,__VERSION__);

	if (i == 0)
	{
		printf("Started on %s running %s/%s release %s\n",
			   sysinfo.nodename,
			   sysinfo.sysname, 
			   sysinfo.machine,
			   sysinfo.release
			   );
	}

	printf("\n");
}

/**
 * show compiling info
 */
void show_info()
{
	show_version();
	printf("Copyright (C) 2005  Paul Baecher & Markus Koetter\n");
	printf("Contact:\tnepenthesdev@users.sourceforge.net\n");
	printf("\t\thttp://nepenthes.sourceforge.net\n");
}

