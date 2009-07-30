# -*- mode: m4; -*-
# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# nepenthes meta-Makefile
# Maximillian Dornseif 2005
# $Id$

AC_PREREQ(2.59)
AC_INIT([nepenthes], [0.2.2], [nepenthesdev@gmail.com])
AM_INIT_AUTOMAKE([nepenthes], [0.2.2])
AC_REVISION([$Id$])

AC_PREFIX_DEFAULT(/opt/nepenthes)
AC_CONFIG_SRCDIR([nepenthes-core/include/Buffer.hpp])
AM_CONFIG_HEADER([config.h])
# AM_MAINTAINER_MODE

AM_CONDITIONAL(NDEBUG, test "$disable_debug" = yes)
AM_CONDITIONAL(PROFILE, test "$enable_profile" = yes)

AC_CANONICAL_HOST
AC_C_CONST
AC_C_BIGENDIAN

dnl Check for some target-specific stuff
case "$host" in
*-*-freebsd*)
  # FreeBSD forgot to  install magic.h in a sane location. Bug or Feature?
  CPPFLAGS="$CPPFLAGS -I/usr/local/include -I/usr/src/contrib/file/"
  LDFLAGS="$LDFLAGS -L/usr/local/lib -R/usr/local/lib"
  ;;
*-*-linux*)
  CPPFLAGS="$CPPFLAGS -fno-strict-aliasing -D_GNU_SOURCE -I/usr/local/include"
  LDFLAGS="$LDFLAGS -L/usr/local/lib -R/usr/local/lib"
  ;;
*-*-darwin*)
  CPPFLAGS="$CPPFLAGS -I/opt/local/include"
  LDFLAGS="$LDFLAGS -L/opt/local/lib"
  if test "$GCC" = "yes"; then
          CFLAGS="$CFLAGS -no-cpp-precomp"
  fi
  ;;
esac

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_YACC
AC_PROG_LEX

# Checks for header files.
#AC_HEADER_DIRENT
#AC_HEADER_STDC
#AC_HEADER_SYS_WAIT
#AC_HEADER_TIME
#AC_HEADER_STDBOOL

AC_CHECK_HEADERS([getopt.h arpa/inet.h fcntl.h memory.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/ioctl.h sys/socket.h sys/time.h sys/timeb.h unistd.h])

dnl   We ought not to have to worry about getopt.h, getopt{,1}.c,
dnl   since they can always be unconditionally compiled and linked.  See
dnl   http://mail.gnu.org/archive/html/autoconf/2000-12/msg00049.html
dnl   and surrounding thread for discussion.  However that doesn't
dnl   appear to work in fact, and the function in getopt_long.c has
dnl   different magic to avoid getting horribly tangled up.
AC_CHECK_FUNCS(getopt)
AC_CHECK_DECLS([getopt,getsubopt,getopt_long,setenv,putenv])
AC_REPLACE_FUNCS(getopt_long getsubopt)

dnl   getopt_long.c uses (and provides a replacement for)
dnl   this glibc utility function
AC_CHECK_FUNCS(__strchrnul)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_UID_T
AC_STRUCT_TM
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL


AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(off_t)

AC_MSG_CHECKING([for SIGPOLL])
AC_EGREP_CPP(we_have_sigpoll,
[#include <signal.h>
#ifdef SIGPOLL
we_have_sigpoll
#endif
], AC_DEFINE([HAVE_SIGPOLL], [1], [Define to 1 if you have the sigpoll signal.]) haveSIGPOLL=yes, haveSIGPOLL=no)
AC_MSG_RESULT([$haveSIGPOLL])

AC_MSG_CHECKING([for SIGCLD])
AC_EGREP_CPP(we_have_sigcld,
[#include <signal.h>
#ifdef SIGCLD
we_have_sigcld
#endif
], AC_DEFINE([HAVE_SIGCLD], [1], [Define to 1 if you have the sigcld signal.]) haveSIGCLD=yes, haveSIGCLD=no)
AC_MSG_RESULT([$haveSIGCLD])

AC_MSG_CHECKING([for SIGPWR])
AC_EGREP_CPP(we_have_sigpwr,
[#include <signal.h>
#ifdef SIGPWR
we_have_sigpwr
#endif
], AC_DEFINE([HAVE_SIGPWR], [1], [Define to 1 if you have the sigpwr signal.]) haveSIGPWR=yes, haveSIGPWR=no)
AC_MSG_RESULT([$haveSIGPWR])

AC_MSG_CHECKING([for SIGSTKFLT])
AC_EGREP_CPP(we_have_sigstkflt,
[#include <signal.h>
#ifdef SIGSTKFLT
we_have_sigstkflt
#endif
], AC_DEFINE([HAVE_SIGSTKFLT], [1], [Define to 1 if you have the sigstkflt signal.]) haveSIGSTKFLT=yes, haveSIGSTKFLT=no)
AC_MSG_RESULT([$haveSIGSTKFLT])

AC_MSG_CHECKING([for SIGUNUSED])
AC_EGREP_CPP(we_have_sigunused,
[#include <signal.h>
#ifdef SIGUNUSED
we_have_sigunused
#endif
], AC_DEFINE([HAVE_SIGUNUSED], [1], [Define to 1 if you have the sigunused signal.]) haveSIGUNUSED=yes, haveSIGUNUSED=no)
AC_MSG_RESULT([$haveSIGUNUSED])

AC_MSG_CHECKING([for SIGINFO])
AC_EGREP_CPP(we_have_siginfo,
[#include <signal.h>
#ifdef SIGINFO
we_have_siginfo
#endif
], AC_DEFINE([HAVE_SIGINFO], [1], [Define to 1 if you have the siginfo signal.]) haveSIGINFO=yes, haveSIGINFO=no)
AC_MSG_RESULT([$haveSIGINFO])

AC_MSG_CHECKING([for SO_NOSIGPIPE])
AC_EGREP_CPP(we_have_sonosigpipe,
[#include <sys/socket.h>
#ifdef SO_NOSIGPIPE
we_have_sonosigpipe
#endif
], AC_DEFINE([HAVE_SO_NOSIGPIPE], [1], [Define to 1 if you have the SO_NOSIGPIPE socketoption.]) haveSO_NOSIGPIPE=yes, haveSO_NOSIGPIPE=no)
AC_MSG_RESULT([$haveSO_NOSIGPIPE])

AC_MSG_CHECKING(for MSG_NOSIGNAL)
AC_EGREP_CPP(we_have_MSG_NOSIGNAL,
[#include <linux/socket.h>
#ifdef MSG_NOSIGNAL
we_have_MSG_NOSIGNAL
#endif
], AC_DEFINE([HAVE_MSG_NOSIGNAL], [1], [Define to 1 if you have the MSG_NOSIGNAL socket flag.]) haveMSG_NOSIGNAL=yes, haveMSG_NOSIGNAL=no)
AC_MSG_RESULT([$haveMSG_NOSIGNAL])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([strndup alarm bzero gethostbyname gethostname gettimeofday inet_ntoa localtime_r memmove memset munmap select socket strcasecmp strdup strerror strncasecmp strstr strtoul setresgid setresuid])


# libadns 
AC_SUBST([LIB_ADNS])

# libpcre
AC_SUBST([LIB_PCRE])

# libcurl
AC_SUBST([LIB_CURL])

# libcapabilities
AC_SUBST([LIB_CAP])

# libmagic
AC_SUBST([LIB_MAGIC])

# libpq
AC_SUBST([LIB_POSTGRES])

# libprelude
AC_SUBST([LIB_PRELUDE])
AC_SUBST([LDFLAG_PRELUDE])
AC_SUBST([CPPFLAG_PRELUDE])

# libipq
AC_SUBST([LIB_IPQ])

# libpcap
AC_SUBST([LIB_PCAP])

# libnet
AC_SUBST([LIB_LIBNET])

# liby
AC_SUBST([LIB_LIBY])

# libdl
AC_SUBST([LIB_DL])

# libssh
AC_SUBST([LIB_SSH])

# libemu
AC_SUBST([LIB_EMU])


dnl **************************************************
dnl * libdl linking flag                             *
dnl **************************************************

enable_dl=no;
AC_CHECK_FUNC([dlopen],enable_dl="yes", enable_dl="no")

if test x$enable_dl = "xno" ; then
	AC_CHECK_LIB([dl],[dlopen],enable_dl="yes", enable_dl="no")
	if test x$enable_dl = "xyes" ; then
		LIB_DL="-ldl"
	fi
fi

if test x$enable_dl = "xno" ; then
	AC_CHECK_LIB([ldld],[dlopen],enable_dl="yes", enable_dl="no")
	if test x$enable_dl = "xyes" ; then
		LIB_DL="-ldl"
	fi
fi



dnl **************************************************
dnl * libipq support                                 *
dnl **************************************************

enable_ipq="yes"

AC_ARG_ENABLE(ipq, [  --enable-ipq	enable support for libipq],
		    [enable_ipq="$enableval"],[enable_ipq="yes"])

AC_ARG_WITH(ipq-include,
                      [  --with-ipq-include=DIR  libipq include dir],
                      [ipq_inc="$withval"],[ipq_inc=no])
AC_ARG_WITH(ipq-lib,
                      [  --with-ipq-lib=DIR  libipq library dir],
                      [ipq_lib="$withval"],[ipq_lib=no])


if test x$enable_ipq = "xyes" ; then
        if test x$ipq_inc != "xno"; then
		CPPFLAGS="${CPPFLAGS} -I${ipq_inc}"
	fi
	
	AC_CHECK_HEADER(libipq.h,[enable_ipq=yes],[enable_ipq=no])
	
	if test x$enable_ipq = "xyes" ; then
		if test x$ipq_lib != "xno"; then
			LDFLAGS="${LDFLAGS}  -L${ipq_lib}"
		fi

		AC_CHECK_LIB([ipq],[ipq_set_mode],enable_ipq="yes", enable_ipq="no")

		if test x$enable_ipq = "xyes" ; then
			LIB_IPQ="-lipq"
			AC_DEFINE([HAVE_IPQ], [1], [Define to 1 if you have ip queue on linux])
		fi
	fi
fi


dnl **************************************************
dnl * pcap support                                   *
dnl **************************************************

AC_ARG_ENABLE(pcap, [  --enable-pcap	enable support for packet capture library],
		    [enable_pcap="$enableval"],[enable_pcap="yes"])


AC_ARG_WITH(pcap_include,
	[  --with-pcap-include=DIR  pcap include directory],
	[pcap_inc="$withval"],[pcap_inc=no])

AC_ARG_WITH(pcap_lib,
	[  --with-pcap-lib=DIR  pcap library directory],
	[pcap_lib="$withval"],[pcap_lib=no])

if test x$enable_pcap = xyes; then
	if test "$pcap_inc" != "no"; then
	   CPPFLAGS="${CPPFLAGS} -I${pcap_inc}"
	fi

	AC_CHECK_HEADER(pcap.h,[enable_pcap=yes],[enable_pcap=no])

	if test x$enable_pcap != "xno"; then

		if test "$pcap_lib" != "no"; then
			LDFLAGS="${LDFLAGS}  -L${pcap_lib}"
		fi

		AC_CHECK_LIB([pcap],[pcap_open_live],enable_pcap="yes", enable_pcap="no")
		if test x$enable_pcap = "xyes" ; then
			LIB_PCAP="-lpcap"
			AC_DEFINE([HAVE_PCAP], [1], [Define to 1 if you have packet capture library])
		fi
	fi
	
fi	



dnl **************************************************
dnl * ipfw support                                   *
dnl **************************************************

AC_ARG_ENABLE(ipfw,
[  --enable-ipfw            enable support for ipfw Divert on FreeBSD],
                enable_ipfw="$enableval", enable_ipfw=no)

if test x$enable_ipfw = "xyes"; then
	AC_DEFINE([HAVE_IPFW], [1], [Define to 1 if you want ipfw divert on FreeBSD])
fi	
		


dnl **************************************************
dnl * debug logging                                  *
dnl **************************************************
AC_ARG_ENABLE(debug-logging, [  --enable-debug-logging		spam the console and logfile with all debug information  [default=no]], 
		    enable_debug_logging="$enableval", enable_debug_logging="no")

if test x$enable_debug_logging = "xyes"; then
	AC_DEFINE([HAVE_DEBUG_LOGGING], [1], [Define to 1 if you need debug logging])
fi	


# Checks for generic libraries.

dnl **************************************************
dnl * libcurl Support                                *
dnl **************************************************
AC_ARG_WITH(curl-include,
    [  --with-curl-include=PATH	specify path to curl include files],
    [curl_inc=$withval],
    [curl_inc=no])

AC_ARG_WITH(curl-lib,
    [  --with-curl-lib=PATH	specify path to curl client library],
    [curl_lib=$withval],
    [curl_lib=no])

if test xyes = "xyes"; then

    AC_PATH_PROG(LIBCURL_CONFIG, curl-config, no)
    if test "$LIBCURL_CONFIG" = "no" ; then
	AC_MSG_ERROR([libcurl curl-config was not found in PATH])
    fi


    if test "x$curl_inc" != xno; then
      CPPFLAGS="-I$curl_inc $CPPFLAGS"
    fi

    if test "x$curl_lib" != "xno"; then
        LDFLAGS="-L$curl_lib $LDFLAGS"
    fi

    orig_cppflags=$CPPFLAGS
    orig_libs=$LIBS
    
    CPPFLAGS="$CPPFLAGS `curl-config --cflags`"
    LIBS="$LIBS `curl-config --libs`"
    

    AC_CHECK_LIB([curl], [curl_version],
             [AC_DEFINE(HAVE_LIBCURL, 1,
                       [Define if you want to use libcurl])
             LIB_CURL="`curl-config --libs` -lcurl"],
             [AC_MSG_ERROR([libcurl is missing - install it please])
             ])

    CPPFLAGS=$orig_cppflags
    LIBS=$orig_libs
fi
	  

dnl **************************************************
dnl * libpcre Support                                *
dnl **************************************************

AC_ARG_WITH(pcre-include,
    [  --with-pcre-include=PATH		specify path to pcre include files],
    [pcre_inc=$withval],
    [pcre_inc=no])

AC_ARG_WITH(pcre-lib,
    [  --with-pcre-lib=PATH	specify path to pcre client library],
    [pcre_lib=$withval],
    [pcre_lib=no])

if test xyes = "xyes"; then
    orig_cppflags=$CPPFLAGS
  
    if test "x$pcre_inc" != xno; then
      CPPFLAGS="-I$pcre_inc $CPPFLAGS"
    fi

    orig_ldflags=$LDFLAGS
    if test "x$pcre_lib" != "xno"; then
        LDFLAGS="-L$pcre_lib $LDFLAGS"
    fi
    
    AC_CHECK_LIB([pcre], [pcre_compile],
             [AC_DEFINE(HAVE_LIBPCRE, 1,
                        [Define if you want to use libpcre])
              LIB_PCRE="-lpcre"],
             [AC_MSG_ERROR([libpcre is missing - install it please])
             ])
fi	     

dnl **************************************************
dnl * libmagic Support                               *
dnl **************************************************

AC_ARG_WITH(magic-include,
    [  --with-magic-include=PATH		specify path to magic include files],
    [magic_inc=$withval],
    [magic_inc=no])

AC_ARG_WITH(magic-lib,
    [  --with-magic-lib=PATH	specify path to magic client library],
    [magic_lib=$withval],
    [magic_lib=no])

if test xyes = "xyes"; then
    orig_cppflags=$CPPFLAGS
  
    if test "x$magic_inc" != xno; then
      CPPFLAGS="-I$magic_inc $CPPFLAGS"
    fi

    orig_ldflags=$LDFLAGS
    if test "x$magic_lib" != "xno"; then
        LDFLAGS="-L$magic_lib $LDFLAGS"
    fi

    
    orig_libs=$LIBS
    LIBS="-lz $LIBS"

    AC_CHECK_LIB([magic], [magic_open],
             [AC_DEFINE(HAVE_LIBMAGIC, 1,
                       [Define if you want to use libmagic])
              LIB_MAGIC="-lmagic -lz"],
             [AC_MSG_ERROR([libmagic is missing - install it please])
             ])

    LIBS=$orig_libs

    AC_CHECK_HEADERS(magic.h)
fi



dnl **************************************************
dnl * libssh Support                                 *
dnl **************************************************

AC_MSG_CHECKING(for libssh)

AC_ARG_ENABLE(libssh, [  --enable-ssh	Enable Support for ssh[default=auto]], 
		    enable_ssh="$enableval", enable_ssh="yes")

AC_ARG_WITH(ssh-lib,
    [  --with-ssh-lib=PATH	specify path to ssh client library],
    [ssh_lib=$withval],
    [ssh_lib=no])
AC_ARG_WITH(ssh-include,
    [  --with-ssh-include=PATH	specify path to ssh include files],
    [ssh_inc=$withval],
    [ssh_inc=no])


if test x$enable_ssh = xyes; then
  dnl **************************************************
  dnl * ssh headers                                  *
  dnl **************************************************

  orig_cppflags=$CPPFLAGS
  
  if test "x$ssh_inc" != xno; then
    CPPFLAGS="-I$ssh_inc $CPPFLAGS"
  fi
    
  AC_CHECK_HEADER(libssh.h, [found=yes], [found=no])
  

  if test "x$found" != "xno"; then

    dnl **************************************************
    dnl * ssh lib                                      *
    dnl **************************************************
	
    orig_ldflags=$LDFLAGS
    if test "x$ssh_lib" != "xno"; then
        LDFLAGS="-L$ssh_lib $LDFLAGS"
    fi
    AC_CHECK_LIB([ssh], [ssh_bind_new],[found=yes], [found=no])
    
    if test "x$found" != "xyes"; then
        LDFLAGS=$orig_ldflags
    fi
  else    
     CPPFLAGS=$orig_cppflags

  fi

  if test "x$found" != "xyes"; then
    enable_ssh=no
  else
    AC_DEFINE(HAVE_LIBSSH,1,[Define wether we have libssh])
    LIB_SSH="-lssh"
  fi
  
fi  


dnl **************************************************
dnl * mysql Support                                  *
dnl **************************************************

AC_ARG_WITH(mysql-lib,
    [  --with-mysql-lib=PATH	specify path to MySQL client library],
    [mysql_lib=$withval],
    [mysql_lib=no])
AC_ARG_WITH(mysql-include,
    [  --with-mysql-include=PATH	specify path to MySQL include files],
    [mysql_inc=$withval],
    [mysql_inc=no])

AC_ARG_ENABLE(mysql, [  --enable-mysql	Enable Support [default=auto]], 
		    enable_mysql="$enableval", enable_mysql="no")


if test x$enable_mysql = xyes; then

  AC_MSG_CHECKING(for mysql client library)



  dnl **************************************************
  dnl * mysql headers                                  *
  dnl **************************************************

  orig_cppflags=$CPPFLAGS
  
  if test "x$mysql_inc" != xno; then
    CPPFLAGS="-I$mysql_inc $CPPFLAGS"
  fi
  
  AC_CHECK_HEADER(mysql.h, [found=yes], [found=no])

  if test "x$found" != "xno"; then


    dnl **************************************************
    dnl * mysql lib                                      *
    dnl **************************************************
	
    orig_ldflags=$LDFLAGS
    if test "x$mysql_lib" != "xno"; then
        LDFLAGS="-L$mysql_lib $LDFLAGS"
    fi
    AC_CHECK_LIB(mysqlclient, mysql_real_connect, [found=yes], [found=no])
    
    if test "x$found" != "xyes"; then
        LDFLAGS=$orig_ldflags
    fi
  else    
     CPPFLAGS=$orig_cppflags

  fi

  if test "x$found" != "xyes"; then
    enable_mysql=no
  else
    AC_DEFINE(HAVE_MYSQL, 1 , [Define if we have mysql client library])
    LIB_MYSQL="-lmysqlclient"
  fi

  AC_MSG_RESULT($enable_mysql)  
fi  




dnl **************************************************
dnl * postgre Support                                  *
dnl **************************************************

AC_ARG_WITH(postgre-lib,
    [  --with-postgre-lib=PATH	specify path to PostGRE client library],
    [postgre_lib=$withval],
    [postgre_lib=no])
AC_ARG_WITH(postgre-include,
    [  --with-postgre-include=PATH	specify path to PostGRE include files],
    [postgre_inc=$withval],
    [postgre_inc=no])

AC_ARG_ENABLE(postgre, [  --enable-postgre	Enable Support [default=auto]], 
		    enable_postgre="$enableval", enable_postgre="no")


if test x$enable_postgre = xyes; then

  AC_MSG_CHECKING(for postgre client library)



  dnl **************************************************
  dnl * postgre headers                                  *
  dnl **************************************************

  orig_cppflags=$CPPFLAGS
  
  if test "x$postgre_inc" != xno; then
    CPPFLAGS="-I$postgre_inc $CPPFLAGS"
  fi
  
  AC_CHECK_HEADER(libpq-fe.h, [found=yes], [found=no])

  if test "x$found" != "xno"; then


    dnl **************************************************
    dnl * postgre lib                                      *
    dnl **************************************************
	
    orig_ldflags=$LDFLAGS
    if test "x$postgre_lib" != "xno"; then
        LDFLAGS="-L$postgre_lib $LDFLAGS"
    fi
    AC_CHECK_LIB(pq, PQsetdbLogin, [found=yes], [found=no])
    
    if test "x$found" != "xyes"; then
        LDFLAGS=$orig_ldflags
    fi
  else    
     CPPFLAGS=$orig_cppflags

  fi

  if test "x$found" != "xyes"; then
    enable_postgre=no
  else
    AC_DEFINE(HAVE_POSTGRES, 1 , [Define if we have postgre client library])
    LIB_POSTGRES="-lpq"

    have_pqserverversion=no
    AC_CHECK_LIB(pq,PQserverVersion,[have_pqserverversion=yes],[have_pqserverversion=no])
    if test "x$have_pqserverversion" != "xno"; then
      AC_DEFINE(HAVE_PQSERVERVERSION, 1 , [Define if libpq supports PQserverVersion])
    fi

  fi

fi  



dnl **************************************************
dnl * libemu support                                  *
dnl **************************************************

AC_ARG_WITH(emu-lib,
    [  --with-emu-lib=PATH	specify path to libemu library],
    [emu_lib=$withval],
    [emu_lib=no])
AC_ARG_WITH(emu-include,
    [  --with-emu-include=PATH	specify path to libemu include files],
    [emu_inc=$withval],
    [emu_inc=no])

AC_ARG_ENABLE(emu, [  --enable-emu	Enable Support [default=auto]], 
		    enable_emu="$enableval", enable_emu="no")


if test x$enable_emu = xyes; then

  AC_MSG_CHECKING(for libemu - x86 emulation library)

  dnl **************************************************
  dnl * emu headers                                  *
  dnl **************************************************

  orig_cppflags=$CPPFLAGS
  
  if test "x$emu_inc" != xno; then
    CPPFLAGS="-I$emu_inc $CPPFLAGS"
  fi
  
  AC_CHECK_HEADER(emu/emu.h, [found=yes], [found=no])

  if test "x$found" != "xno"; then


    dnl **************************************************
    dnl * emu lib                                      *
    dnl **************************************************
	
    orig_ldflags=$LDFLAGS
    if test "x$emu_lib" != "xno"; then
        LDFLAGS="-L$emu_lib $LDFLAGS"
    fi
    AC_CHECK_LIB(emu, emu_new, [found=yes], [found=no])
    
    if test "x$found" != "xyes"; then
        LDFLAGS=$orig_ldflags
    fi
  else    
     CPPFLAGS=$orig_cppflags

  fi

  if test "x$found" != "xyes"; then
    enable_emu=no
  else
    AC_DEFINE(HAVE_EMU, 1 , [Define if we have emu client library])
    LIB_EMU="-lemu"
  fi
fi  




dnl **************************************************
dnl * large filesystem Support                       *
dnl **************************************************

AC_MSG_CHECKING(for Large File System support)
AC_ARG_ENABLE(lfs,
 AC_HELP_STRING([--enable-lfs],[Turn on Large File System (default)]),
 [case "$host" in
 *-*-linux*)
 case "${enableval}" in
    yes) CPPFLAGS="${CPPFLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGE_FILES" ;;
     no) ;;
      *) AC_MSG_ERROR(bad value ${enableval} for --enable-lfs) ;;
   esac
   ;;
 esac],[
 CPPFLAGS="${CPPFLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGE_FILES"
 enable_lfs=yes]
 ,enable_lfs=no,)
AC_MSG_RESULT($enableval)



dnl **************************************************
dnl * dnsresolve Support                             *
dnl **************************************************



AC_ARG_ENABLE(dnsresolve_adns, [  --enable-dnsresolve-adns	Enable adns [default=yes]], 
		    enable_dnsresolve_adns="$enableval", enable_dnsresolve_adns="yes")

dnl **************************************************
dnl * libadns Support                                *
dnl **************************************************
AC_ARG_WITH(adns-include,
    [  --with-adns-include=PATH	specify path to adns include files],
    [adns_inc=$withval],
    [adns_inc=no])

AC_ARG_WITH(adns-lib,
    [  --with-adns-lib=PATH	specify path to adns client library],
    [adns_lib=$withval],
    [adns_lib=no])

if test x$enable_dnsresolve_adns = xyes; then
    orig_cppflags=$CPPFLAGS
  
    if test "x$adns_inc" != xno; then
      CPPFLAGS="-I$adns_inc $CPPFLAGS"
    fi

    orig_ldflags=$LDFLAGS
    if test "x$adns_lib" != "xno"; then
        LDFLAGS="-L$adns_lib $LDFLAGS"
    fi


    AC_CHECK_LIB([adns], [adns_init],
           [AC_DEFINE(HAVE_LIBADNS, 1,
                      [Define if you want to use adns])
            LIB_ADNS="-ladns"],
     enable_dnsresolve_adns=no
   )
fi

if test x$enable_dnsresolve_adns = xno; then
  AC_MSG_ERROR([at least one dnsresolver has to work])
fi



#dnl **************************************************
#dnl * ipv6 Support                                   *
#dnl **************************************************
#
#AC_ARG_ENABLE(ipv6,
#  [  --enable-ipv6          enable IPv6 support [default=auto].],
#  [ if test "$enableval" = "no"; then
#       enable_ipv6="no"
#    fi ],
#  [ enable_ipv6="yes" ] )
#
#
#
#if test x$enabled_ipv6 = xno ; then
#    AC_MSG_RESULT([no -- disabled by user])
#else
#    enable_ipv6="no"
#    AC_CHECK_FUNC(inet_pton,[enable_ipv6=yes])
#    AC_CHECK_LIB(resolv,inet_pton,[enable_ipv6=yes])
#    AC_MSG_CHECKING(for sockaddr_in6 in netinet/in.h)
#    AC_EGREP_HEADER(sockaddr_in6,netinet/in.h,
#    [AC_MSG_RESULT(yes)], [AC_MSG_RESULT(no); enable_ipv6="no"])
#    if ${have_ipv6}; then
#        AC_DEFINE(HAVE_IPV6, 1, Define if the system have IPv6 structures.)
#    fi
#fi




dnl **************************************************
dnl * libprelude Support                             *
dnl **************************************************
AC_MSG_CHECKING(whether to use prelude for logging)
AC_ARG_ENABLE(prelude,
  [  --enable-prelude          enable prelude support [default=no].],
  [ if test "$enableval" = "yes"; then
       enable_prelude="yes"
    fi ],
  [ enable_prelude="no" ] )

if test x$enable_prelude = xno ; then
    AC_MSG_RESULT([no -- disabled by user])
else
    AC_PATH_PROG(LIBPRELUDE_CONFIG, libprelude-config, no)
    if test "$LIBPRELUDE_CONFIG" = "no" ; then
	AC_MSG_ERROR([libpreludes libprelude-config was not found in PATH])
	enable_prelude=no
    else
        AC_DEFINE(HAVE_LIBPRELUDE, 1,[Define if you want to use libprelude]) 
	PRELUDE_LDFLAGS=`$LIBPRELUDE_CONFIG --ldflags`
	PRELUDE_LIBS=`$LIBPRELUDE_CONFIG --libs`
	PRELUDE_CPPFLAGS=`$LIBPRELUDE_CONFIG --pthread-cflags`
	
	LIB_PRELUDE="$PRELUDE_LIBS"
	LDFLAG_PRELUDE="$PRELUDE_LDFLAGS"
	CPPFLAGS="$CPPFLAGS $PRELUDE_CPPFLAGS"
    fi
fi




dnl **************************************************
dnl * capabilities Support                           *
dnl **************************************************

AC_MSG_CHECKING(whether to use libcap to improve security)
AC_ARG_ENABLE(capabilities, [  --enable-capabilities	Enable capabilities [default=auto]], 
		   [enable_capabilities="$enableval"], 
		   [enable_capabilities="yes"])

if test x$enable_capabilities = xno ; then
    AC_MSG_RESULT([no -- disabled by user])
else
    AC_ARG_WITH(cap-include,
	[  --with-cap-include=PATH      use libcap],
	[cap_inc=$withval],
	[cap_inc=no])
	
    AC_ARG_WITH(cap-lib,
	[  --with-cap-lib=PATH       	use libcap],
	[cap_lib=$withval],
	[cap_lib=no])


    orig_cppflags=$CPPFLAGS
  
    if test "x$cap_inc" != xno; then
      CPPFLAGS="-I$cap_inc $CPPFLAGS"
    fi

    orig_ldflags=$LDFLAGS
    if test "x$cap_lib" != "xno"; then
        LDFLAGS="-L$cap_lib $LDFLAGS"
    fi

    AC_CHECK_LIB(cap, cap_init, 
	[
              AC_DEFINE(HAVE_LIBCAP, 1,[Define if libcap is available to restrict process capabilities])
               LIB_CAP="-lcap"
               enable_capabilities="yes"
	], 
	[
               AC_MSG_WARN([libcap check failed])
               enable_capabilities="no (check failed)"
	])
    
fi    

dnl **************************************************
dnl * liby existence                                 *
dnl **************************************************

AC_CHECK_LIB(y, yyerror,
	[	
              LIB_LIBY="-ly"
              enable_liby="yes"
	], 
	[
               enable_liby="no"
	])
		



AC_DEFUN([AC_DEFINE_DIR], [
  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix
dnl In Autoconf 2.60, ${datadir} refers to ${datarootdir}, which in turn
dnl refers to ${prefix}.  Thus we have to use `eval' twice.
  eval ac_define_dir="\"[$]$2\""
  eval ac_define_dir="\"$ac_define_dir\""
  AC_SUBST($1, "$ac_define_dir")
  AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])

AC_DEFINE_DIR(PREFIX, prefix, [default working directory])
AC_DEFINE_DIR(LOCALESTATEDIR, localstatedir, [where to put logs etc])
AC_DEFINE_DIR(LIBDIR, libdir, [where to look for plugins])
AC_DEFINE_DIR(SYSCONFDIR, [sysconfdir], [System configuration dir])

AC_CONFIG_FILES([Makefile
                 nepenthes-core/Makefile
                 nepenthes-core/include/Makefile
                 nepenthes-core/src/Makefile
                 modules/Makefile
                 modules/dnsresolve-adns/Makefile
                 modules/download-creceive/Makefile
                 modules/download-csend/Makefile
                 modules/download-curl/Makefile
                 modules/download-ftp/Makefile
                 modules/download-http/Makefile
                 modules/download-link/Makefile
                 modules/download-rcp/Makefile
                 modules/download-tftp/Makefile
		 modules/log-attack/Makefile
                 modules/log-download/Makefile
                 modules/log-irc/Makefile
                 modules/log-prelude/Makefile
                 modules/log-surfnet/Makefile
		 modules/module-bridge/Makefile
                 modules/module-honeytrap/Makefile
                 modules/module-portwatch/Makefile
		 modules/module-peiros/Makefile
                 modules/shellcode-generic/Makefile
                 modules/shellcode-signatures/Makefile
                 modules/shellemu-winnt/Makefile
                 modules/sqlhandler-postgres/Makefile
                 modules/submit-file/Makefile
                 modules/submit-gotek/Makefile
                 modules/submit-norman/Makefile
                 modules/submit-postgres/Makefile
                 modules/submit-http/Makefile
                 modules/submit-mwserv/Makefile
                 modules/vuln-asn1/Makefile
                 modules/vuln-bagle/Makefile
                 modules/vuln-dameware/Makefile
                 modules/vuln-dcom/Makefile
                 modules/vuln-ftpd/Makefile
                 modules/vuln-iis/Makefile
                 modules/vuln-kuang2/Makefile
                 modules/vuln-lsass/Makefile
                 modules/vuln-msdtc/Makefile
                 modules/vuln-msmq/Makefile
                 modules/vuln-mssql/Makefile
                 modules/vuln-ms08067/Makefile
                 modules/vuln-mydoom/Makefile
                 modules/vuln-netbiosname/Makefile
                 modules/vuln-netdde/Makefile
                 modules/vuln-optix/Makefile
                 modules/vuln-pnp/Makefile
                 modules/vuln-realvnc/Makefile
                 modules/vuln-sasserftpd/Makefile
		 modules/vuln-sav/Makefile
                 modules/vuln-ssh/Makefile
                 modules/vuln-sub7/Makefile
                 modules/vuln-upnp/Makefile
                 modules/vuln-veritas/Makefile
                 modules/vuln-wins/Makefile
                 modules/x-1/Makefile
                 modules/x-2/Makefile
                 modules/x-3/Makefile
                 modules/x-4/Makefile
                 modules/x-5/Makefile
                 modules/x-6/Makefile
                 modules/x-9/Makefile
                 modules/log-hexdump/Makefile
		 ])
#		 modules/shellcode-emulation/Makefile
#		 modules/module-upnp/Makefile		 
#		 modules/log-prelude/Makefile
#                modules/module-drone/Makefile		 
#		 modules/log-surfnet/Makefile])
#                modules/vuln-netbios/Makefile
#                 modules/module-baitnswitch/Makefile
#		 modules/dnsresolve-udns/Makefile                 

AC_OUTPUT

echo
echo "*** Dumping Nepenthes Configuration ***"
echo "- Debug Logging               : $enable_debug_logging"
echo ""
echo "- link on liby                : $enable_liby"
echo ""
echo "- DNS Resolve                 "
echo "  - Support adns              : $enable_dnsresolve_adns"
echo ""
echo "- Support Large Filesystem    : $enable_lfs"
echo "" 
echo "- Support MySQL               : $enable_mysql"
echo "" 
echo "- Support PostgreSQL          : $enable_postgre"
echo "" 
echo "- Support ssh                 : $enable_ssh"
echo "" 
echo "- Support prelude             : $enable_prelude"
echo "" 
echo "- Support capabilities        : $enable_capabilities"
echo ""
echo "- Packet Monitoring/Sniffing"
echo "  - linux ip_queue (ipq)      : $enable_ipq"
echo "  - FBSD ipfw Divert sockets  : $enable_ipfw"
echo "  - Packet Capture Lib (pcap) : $enable_pcap"

# echo ""
# echo "- Support ipv6                : $enable_ipv6"
