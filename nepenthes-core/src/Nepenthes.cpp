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

#else
#include <unistd.h>
#include <getopt.h>
#endif


#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


#include "Nepenthes.hpp"
#include "SocketManager.hpp"
#include "EventManager.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "ConsoleLogger.hpp"
#include "FileLogger.hpp"
#include "ModuleManager.hpp"
#include "ShellcodeManager.hpp"
#include "SubmitManager.hpp"
#include "Config.hpp"
#include "Utilities.hpp"
#include "DialogueFactoryManager.hpp"
#include "DNSManager.hpp"
#include "Message.hpp"

using namespace nepenthes;

Nepenthes *g_Nepenthes;
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

	m_SocketManager	= NULL;
	m_DownloadManager	= NULL;
	m_EventManager	= NULL;
    m_SubmitManager	= NULL;
	m_ModuleManager	= NULL;
	m_ShellcodeManager	= NULL;
	m_Utilities	= NULL;
	m_LogManager	= NULL;
	m_DialogueFactoryManager	= NULL;

}

Nepenthes::~Nepenthes()
{
	if ( m_SocketManager != NULL )
	{
		delete m_SocketManager;
		delete m_DownloadManager;
		delete m_EventManager;
		delete m_SubmitManager;
		delete m_ModuleManager;
		delete m_ShellcodeManager;
		delete m_Utilities;
		delete m_LogManager;
		delete m_DialogueFactoryManager;
	}
}

/**
 * start nepenthes, using command line arguments.
 * 
 * @param argc   number of arguments.
 * @param argv   vector containing arguments.
 * 
 * @return 0 if the application was shut down poperly, non-null if an error occured.
 */
int Nepenthes::run(int argc, char **argv)
{
	bool run=true;
	bool confcheck=false;
	bool filecheck=false;
	char *filecheckarg;
	char *confpath = "conf/nepenthes.conf";
	char *basedir;
	char *workingdir = NULL;
	char *chUser = NULL;
	char *chGroup = NULL;
	char *chRoot = NULL;


#ifdef WIN32

#else


	while( 1 )
	{
		int option_index = 0;
		static struct option long_options[] = {
            { "config", 		1, 0, 'c' },
			{ "disk-log", 		1, 0, 'd' },	// FIXME
			{ "file-check",		1, 0, 'f' },	// FIXME 
			{ "group",			1, 0, 'g' },	// FIXME 
            { "help", 			0, 0, 'h' },
			{ "large-help",		0, 0, 'H' },
			{ "info",			0, 0, 'i' },
			{ "check-config", 	0, 0, 'k' },
			{ "log", 			1, 0, 'l' },	// FIXME
			{ "no-color", 		0, 0, 'o' }, 	// FIXME
			{ "chroot",			1, 0, 'r' },	// FIXME
			{ "user",			1, 0, 'u' },	// FIXME 
			{ "version", 		0, 0, 'v' },
			{ "workingdir", 	0, 0, 'w' },
			{ 0, 0, 0, 0 }
		};

		int c = getopt_long(argc, argv, "c:d:f:g:hHikl:or:u:vw:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{

		case 'b':
			basedir = optarg;
			break;

		case 'c':
			confpath = optarg;
			break;

		case 'd':	// FIXME set disk loglevel
			printf("This feature '%c' is todo\nquitting\n",c);
			run=false;
			break;

		case 'f':
//			fprintf(stderr,"filecheck\n");
			filecheckarg = optarg;
			filecheck = true;
			run=false;
			break;


		case 'g':	// FIXME set group
			chGroup=optarg;
			printf("Change Group to %s\n",chGroup);
//			run=false;
            break;

		case 'h':
			show_help(false);
			run=false;
			break;

		case 'H':
			show_help(true);
			run=false;
			break;

		case 'i':
			show_info();
			run=false;
			break;

		case 'l':	// FIXME set console loglevel
			printf("This feature '%c' is todo\nquitting\n",c);
			run=false;
			break;

		
		case 'k':
            run = false;
			confcheck = true;
			break;

		case 'o':	// FIXME set nocolor on console
			printf("This feature '%c' is todo\nquitting\n",c);
			run=false;
			break;

		case 'r':	// FIXME set chroot
			chRoot = optarg;
			printf("Change Root to %s \n",chRoot);
//			printf("This feature '%c' is todo\nquitting\n",c);
//			run=false;

			break;

		case 'u':	// FIXME set user
            chUser = optarg;
			printf("Change User to %s \n",chUser);
//			printf("This feature '%c' is todo\nquitting\n",c);
//			run=false;
			break;


		case 'v':
			show_version();
            run = false;
			break;

		case 'w':
			workingdir = optarg;
			break;

		case '?':
		case ':':
			exit(0);
			break;

		default:
			break;
		}
	}


	if( workingdir && chdir(workingdir) )
	{
		logCrit("Cannot change working diretory to %s: %s.\n", workingdir, strerror(errno));
		return -1;
	}
#endif

	if (chUser != NULL)
	{
		if (changeUser(chUser) == false)
		{
			run=false;
		}
	}

	if (chGroup != NULL)
	{
		changeGroup(chGroup);
	}

/*
	if (chRoot != NULL)
	{
		changeRoot(chRoot);
	}
*/


	if(run == true || confcheck == true || filecheck == true)
	{
		if (run == true)
		{
            show_logo();
			show_version();
		}

		m_LogManager        = new LogManager();
		if (filecheck == false)
		{
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

			m_LogManager->addLogger(new ConsoleLogger(m_LogManager), l_all);
		}

		if ( run == true )
		{
			/* temp hack */
			FileLogger *fl = new FileLogger(m_LogManager);
			fl->setLogFile("log/nepenthes.log");
			m_LogManager->addLogger(fl, l_all);
		}

		if ( run == true || filecheck == true)
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
		}
	}


	if ( run == true || confcheck == true || filecheck == true)
	{
        m_Config = new Config;
		logInfo("Trying to load Nepenthes Configuration from %s \n",confpath);
		try
		{
			m_Config->load(confpath);
			logInfo("Done loading Nepenthes Configuration from %s \n",confpath);
		} catch ( LoadError e )
		{
			printf("Unable to load configuration file %s: %s\n", confpath, e.getMessage());
			run = false;
		} catch ( ParseError e )
		{
			printf("Parse error in %s on line %d: %s\n", confpath, e.getLine(), e.getMessage());
			run = false;
		}
		
	}

	if (run == true || filecheck == true)
	{
    
		// socketManager will call WASStartup()
		m_SocketManager->Init();

		m_DNSManager->Init();

		m_ModuleManager->Init();
		m_ModuleManager->doList();

		m_DownloadManager->Init();
		m_DownloadManager->doList();

		m_EventManager->doList();

		m_ShellcodeManager->doList();

		m_SocketManager->doList();

		m_SubmitManager->Init();
		m_SubmitManager->doList();

		m_DialogueFactoryManager->doList();

	}

	if (chRoot != NULL)
	{
		if (changeRoot(chRoot) == false)
		{
			run = false;
		}
	}

	if (chGroup != NULL)
	{
		if (changeGroup() == false)
		{
			run=false;
		}
	}

	if (chUser != NULL)
	{
		if (changeUser() == false)
		{
			run=false;
		}
	}




	if(run)
	{
        doLoop();
	}else
	if (filecheck)
	{
		show_version();
		fileCheck(filecheckarg,argc,optind,argv);
	}


	if (m_Config != NULL)
	{
    	delete m_Config;
	}
    return 0;
}



bool Nepenthes::doLoop()
{

	while (m_running)
	{
    	getSocketMgr()->doLoop(500);
		getEventMgr()->doTimeoutLoop();
		getDNSMgr()->pollDNS();
	}
	return true;
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
int Nepenthes::fileCheck(const char *filename, Message **Msg)
{
    
	unsigned int filesize=0;
	unsigned char *buffer=NULL;
	struct stat fileinfo;
	int retval = -1;

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

		unsigned int i;
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
		}else
		{
			retval = 1;
		}
	}
	free(buffer);
	return retval;
}

bool Nepenthes::fileCheck(char *optval, int argc, int opti, char **argv)
{
	if ( opti >= argc )
	{
		return true;
	}


	bool rmknown=false;
	bool rmnonop=false;
	if (optval != NULL)
	{
		if (strstr(optval,"rmnonop") != NULL)
		{
			rmnonop = true;
		}
		if (strstr(optval,"rmknown") != NULL)
		{
			rmknown = true;
		}


	}

	printf ("Checking Files: \n");
	for ( ; opti < argc; )
	{
		printf("  * %42s\t",argv[opti]);
		Message *Msg=NULL;
		switch ( fileCheck(argv[opti],&Msg) )
		{
		case -1:
			printf("could not open file");
			break;

		case 0:
			if ( m_ShellcodeManager->fileCheck(&Msg) == SCH_DONE )
			{
				printf("\033[32;1mKNOWN\033[0m");
				if (rmknown == true)
				{
					unlink(argv[opti]);
				}
			} else
			{
				printf("\033[31;1mFAILED\033[0m");
			}
			delete Msg;
			break;

		case 1:
			printf("NONOP");
			if (rmnonop == true)
			{
				unlink(argv[opti]);
			}
			break;
		}
		printf("\n");
		opti++;
	}
	return true;
}



Config *Nepenthes::getConfig()
{
	return m_Config;
}

DownloadManager *Nepenthes::getDownloadMgr()
{
	return m_DownloadManager;
}

EventManager *Nepenthes::getEventMgr()
{
	return m_EventManager;
}

LogManager *Nepenthes::getLogMgr()
{
	return m_LogManager;
}

LuaInterface *Nepenthes::getLua()
{
	return m_Lua;
}

ModuleManager *Nepenthes::getModuleMgr()
{
	return m_ModuleManager;
}


ShellcodeManager *Nepenthes::getShellcodeMgr()
{
	return m_ShellcodeManager;
}

SocketManager *Nepenthes::getSocketMgr()
{
	return m_SocketManager;
}


SubmitManager *Nepenthes::getSubmitMgr()
{
	return m_SubmitManager;
}

Utilities *Nepenthes::getUtilities()
{
	return m_Utilities;
}

DialogueFactoryManager *Nepenthes::getFactoryMgr()
{
	return m_DialogueFactoryManager;
}

DNSManager *Nepenthes::getDNSMgr()
{
	return m_DNSManager;
}


bool Nepenthes::stop()
{
	if(m_running)
	{
    	m_running = false;
		return true;
	}
	return false;
}

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


bool Nepenthes::changeUser(char *user)
{
	passwd * pass;                                
    
	if ( (pass = getpwnam(user)) == NULL )
	{
		printf("Could not get useid for user '%s'\n",user);
		return false;
	}else
	{
		printf("User %s has uid %i\n",user,pass->pw_uid);
		m_UID = pass->pw_uid;
		return true;
	}

}

bool Nepenthes::changeUser()
{
	if( setresuid(m_UID, m_UID, m_UID) < 0 )
	{
		printf("Could not set User id %i for process \n%s\n",m_UID,strerror(errno));
		return false;
	}else
	{
		printf("Process userid %i\n",getuid());
		return true;
	}

}


bool Nepenthes::changeGroup(char *gruppe)
{
    
	struct group *grp;

	if ( (grp = getgrnam(gruppe)) == NULL )
	{
		printf("Could not get groupid for group '%s'\n%s\n",gruppe,strerror(errno));
		return false;
	}else
	{
		printf("Group %s has gid %i\n",gruppe,grp->gr_gid);
		m_GID = grp->gr_gid;
		return true;
	}

}

bool Nepenthes::changeGroup()
{
	if( setresgid(m_GID, m_GID, m_GID) < 0 )
	{
		printf("Could not set Group id %i for process \n%s\n",m_GID,strerror(errno));
		return false;
	}else
	{
		printf("Process groupid %i\n",getgid());
		return true;
	}

}


bool Nepenthes::changeRoot(char *path)
{
	if ( chroot(path) < 0)
	{
		printf("Could not changeroot to %s \n%s\n",path,strerror(errno));
		return false;
	}else
	{
		printf("Changed root to %s\n",path);
		return true;
	}

}

void SignalHandler(int iSignal)
{
    printf("Got signal %i\n", iSignal);
    switch(iSignal)
    {
	case 1: // SIGHUP
		printf("Got SIGHUP\nRereading Config File!\n");
		g_Nepenthes->reloadConfig();
		break;

	case 2: // SIGINT
		printf("Got SIGINT\nStopping NOW!\n");
		g_Nepenthes->stop();
		break;

	case 6:	// SIGABRT
		printf("%s\n", "Unhandled Exception");
		exit(-1);
		break;

    case 11:
        printf("%s\n", "Segmentation Fault");
        exit(-1);
        break;
    case 28:
        printf("Ignoring %i\n", iSignal);
        signal(iSignal,SIG_IGN);
        break;

    default:
        printf("Exit 'cause of %i\n", iSignal);
        g_Nepenthes->stop();
	}
}

void show_logo();

int main(int argc, char **argv)
{
#ifdef WIN32
	// win32 signals here
#else


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
	signal(SIGPIPE,  SignalHandler);	//      13       Term    Broken pipe: write to pipe with no readers
	signal(SIGALRM,  SignalHandler);	//      14       Term    Timer signal from alarm(2)
	signal(SIGTERM,  SignalHandler);	//      15       Term    Termination signal
	signal(SIGUSR1,  SignalHandler);	//   30,10,16    Term    User-defined signal 1
	signal(SIGUSR2,  SignalHandler);	//   31,12,17    Term    User-defined signal 2
	signal(SIGCHLD,  SignalHandler);	//   20,17,18    Ign     Child stopped or terminated
	signal(SIGCONT,  SignalHandler);	//   19,18,25            Continue if stopped
//  signal(SIGSTOP,  SIG_IGN	  );	//   17,19,23    Stop    Stop process
	signal(SIGTSTP,  SIG_IGN	  );	//   18,20,24    Stop    Stop typed at tty
	signal(SIGTTIN,  SignalHandler);	//   21,21,26    Stop    tty input for background process
	signal(SIGTTOU,  SignalHandler);	//   22,22,27    Stop    tty output for background process

/*
 *	Next the signals not in the POSIX.1 standard but described in SUSv2 and SUSv3 / POSIX 1003.1-2001.
 */

	signal(SIGBUS,   SignalHandler);	//   10,7,10     Core    Bus error (bad memory access)
	signal(SIGPOLL,  SignalHandler);	//               Term    Pollable event (Sys V). Synonym of SIGIO
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
	signal(SIGSTKFLT,SignalHandler);	//    -,16,-     Term    Stack fault on coprocessor (unused)
	signal(SIGIO,    SignalHandler);	//   23,29,22    Term    I/O now possible (4.2 BSD)
	signal(SIGCLD,   SignalHandler);	//    -,-,18     Ign     A synonym for SIGCHLD
	signal(SIGPWR,   SignalHandler);	//   29,30,19    Term    Power failure (System V)
//  signal(SIGINFO,  SignalHandler);	//    29,-,-             A synonym for SIGPWR
//  signal(SIGLOST,  SignalHandler);	//     -,-,-     Term    File lock lost
	signal(SIGWINCH, SIG_IGN      );	//   28,28,20    Ign     Window resize signal (4.3 BSD, Sun)
	signal(SIGUNUSED,SignalHandler);	//    -,31,-     Term    Unused signal (will be SIGSYS)
#endif

	
	Nepenthes nepenthes;
	return nepenthes.run(argc, argv);
}

void show_logo()
{
	printf("%s","#                                                                             #\n");
	printf("%s","                                                                               \n");
	printf("%s","                 !¦..,('                                                       \n");
	printf("%s","                 !!..,¦,                                                       \n");
	printf("%s","                 !*'.,!*                                                       \n");
	printf("%s","                 *!'.,!¦                                                       \n");
	printf("%s","                 '!'.,!¦                                                       \n");
	printf("%s","                 .¦,',*¦,,.                                                    \n");
	printf("%s","            ,#=%(#%#%%#%%%C                                                    \n");
	printf("%s","         .#%C=,:::::::,,5%#.                                                   \n");
	printf("%s","         (#!!,,,,,,,,,,,J7#'                                                   \n");
	printf("%s","         =#!(=,,,,,,,,:==(7!                       ,%###!##%'.                 \n");
	printf("%s","         .=#(CJ3$#%C7==¦((77                     ,%##%%!#%%%#%*                \n");
	printf("%s","       .#(#J#A3#$==7=J=!¦(=3'                 ,%##%%::::,,::%#%, '!(====((!,'. \n");
	printf("%s","     ######$@53C=(¦¦¦¦==(¦=C7                %##(::::,,,,,::%##A$$3$CCJ3CJC(7C!\n");
	printf("%s","   #(###3#'JÐ%J(¦!!!*!!J(((=J'              ,##(:::,,,,,:::%##5%C'           . \n");
	printf("%s"," .#3#3#$#. C5$C7¦!=¦¦¦!7=(¦(7=              ;##(:,,,,,,,::%###%.               \n");
	printf("%s"," '####*.   7AJC7¦!7¦(¦¦!¦((=7C.             |###((,,,,,,,)%##%=C,    .*¦(7!    \n");
	printf("%s","           (%7C77!(==!¦!¦¦=7=C!             ((%############%!(CC¦.,¦JAA%3C'    \n");
	printf("%s","           ¦$C7C7=¦(=¦¦¦((=(=J= ,:%%@M;%%;.77C7(=C!¦!*!¦=!¦C(777J3$3=!,'.      \n");
	printf("%s","           ,%CJJ=C(7(((((=((=JC 7####::###CC=(CC=(=¦(*!=!=¦=7=77JJ,            \n");
	printf("%s","           .%J337J==(=7==C=C7C3,##J!::::77#%J=CJ==¦¦¦=!=¦===(7C7C*             \n");
	printf("%s","            C$33C3C7=7C777CJJJ%7##C:,,,,,:#$3CJC==(=7(¦¦¦(=C(J$CJ*             \n");
	printf("%s","            '%$333CCC3CCCCCJ%3%#%#$C:,,,:3#AJJ3C7=JC=(((((7C7C3J3!             \n");
	printf("%s","             (#$$3JJJ%3J$J33$$AA33#5J:,,:A#$$$JC7CJCC7=(=(CJ$CJJ$,             \n");
	printf("%s","              J#%$$%33%$5$A#$%#%CJ3#A%:35#%J3$3JJCCC$C7J77JC$J33$'             \n");
	printf("%s","              .75AA%%%$AA%#A%5#%CJC#J###3#J$%3$JJCCJCC3J33$33$$A.              \n");
	printf("%s","                =5555A%AA5#5@%,%CJC3J###ACJJ$5$33JJJJJJJJ%%%3%%A%              \n");
	printf("%s","                .3@55##A5###='!AJCJJ3###ACJ3A#$$$%$$33333$%%%AA5!              \n");
	printf("%s","                 %3J%5Ð5$J='  '533CC3###ACJ$#@A%%%AA$A%$$A%AA55¦               \n");
	printf("%s","                 A=(3$A'.      3A3J33###%CJ$Ð#AA55AA%5#AA%%5#5¦                \n");
	printf("%s","                .AC=¦$J        'A%$J$A##A33AAÐ555###55#5555#A!                 \n");
	printf("%s","                *J7==A¦         '$%$%%##A3A=.JA###555%55##5(.                  \n");
	printf("%s","                =JJJJ5'          '(A5A#JA%!   *=Ð@#%AA5553¦                    \n");
	printf("%s","                J$3J$$             .55A$#*     .#A$AÐ%7*                       \n");
	printf("%s","               .$33$%C              %$JJ5.     !3J(%¦.                         \n");
	printf("%s","               '%%%A$7              %CC75.     C(J7(                           \n");
	printf("%s","               *A%J$A=              %==73     ,J7C$'                           \n");
	printf("%s","               7%$JJ@!             'A7=CC     =C7CJ                            \n");
	printf("%s","               $J3(3#.             *$77C=     %(CC(                            \n");
	printf("%s","              .#33($J              ¦7C73,    '$JJC,                            \n");
	printf("%s","                                                                               \n");
	printf("%s","#                                                                             #\n");
	printf("%s","                                                                               \n");
	printf("%s","                              Nepenthes Ampullaria                             \n");
	printf("%s","                                                                               \n");
	printf("%s","#                                                                             #\n");
}

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
        {   "c",    "config",           "give path to Config File"      	,   "conf/nepenthes.conf"	},
		{   "d",    "disk-log",         "log to disk"                   	,   "FIXME"					},
		{   "f",    "file-check",       "check file for known shellcode rmknown,rmnonop"	,   ""						},
		{   "g",    "group",            "set group to switch to"			,   "default is not to switch"},
		{	"h",  	"help",            	"show help"                     	,   ""						},
		{	"H",  	"large-help",       "show help with default values"     ,   ""						},
		{	"i",  	"info",    			"how to contact us"					,   ""						},
		{	"k",  	"check-config",    	"check config for syntax errors"	,   ""						},
		{	"l",  	"log",             	"log to console"                	,   "FIXME"					},
		{	"o",  	"no-color",        	"log without colors"            	,   "FIXME"					},
        {   "r",    "chroot",        	"chroot to"				,   			"default is not to chroot set"},
		{   "u",    "user",        	    "set user to switch to"				,   "default is not to switch"},
		{	"v",  	"version",         	"show version"                  	,   ""						},
		{	"w",  	"workingdir",       "where shall the process live"      ,   "./"						},
	};
	show_version();

	if ( defaults == true )
	{
		for ( unsigned int i=0;i<sizeof(myopts)/sizeof(helpstruct);i++ )
		{
			printf("  -%s,\t--%-12s %-30s %s\n", myopts[i].m_shortOpt,
				   myopts[i].m_longOpt,
				   myopts[i].m_Description,
				   myopts[i].m_Default);
		}
	}else
	{
		for ( unsigned int i=0;i<sizeof(myopts)/sizeof(helpstruct);i++ )
		{
			printf("  -%s,\t--%-12s %-30s\n", myopts[i].m_shortOpt,
				   myopts[i].m_longOpt,
				   myopts[i].m_Description
				   );
		}

	}
}

#ifdef __GNUG__
  #define MY_COMPILER "g++"
#else
  #define MY_COMPILER "unknown Compiler"
#endif


void show_version()
{
	printf("\n");
	printf("Nepenthes Version %s \n",NEPENTHES_VERSION);
    printf("Compiled on %s %s with %s %s \n",__DATE__, __TIME__,MY_COMPILER,__VERSION__);
    printf("\n");
}

void show_info()
{
	show_version();
	printf("Copyright (C) 2005  Paul Baecher & Markus Koetter\n");
	printf("Contact:\tnepenthesdev@users.sourceforge.net\n");
	printf("\t\thttp://nepenthes.sourceforge.net\n");
}



