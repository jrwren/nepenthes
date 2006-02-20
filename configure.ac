# -*- mode: m4; -*-
# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# nepenthes meta-Makefile
# Maximillian Dornseif 2005
# $Id$

AC_PREREQ(2.59)
AC_INIT([nepenthes], [0.1.2], [dornseif@informatik.rwth-aachen.de])
AM_INIT_AUTOMAKE([nepenthes], [0.1.2])
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
  CPPFLAGS="$CPPFLAGS -D _GNU_SOURCE -I/usr/local/include"
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
 esac],[CPPFLAGS="${CPPFLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGE_FILES"])
AC_MSG_RESULT($enableval)

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

# Checks for libraries.
AC_CHECK_LIB([adns], [adns_init],
             [AC_DEFINE(HAVE_LIBADNS, 1,
                        [Define if you want to use adns])
              LIBS="-ladns $LIBS"],
             [AC_MSG_ERROR([libadns is missing - install it please])
             ])

AC_CHECK_LIB([curl], [curl_version],
             [AC_DEFINE(HAVE_LIBCURL, 1,
                       [Define if you want to use libcurl])
              LIBS="-lcurl $LIBS"],
             [AC_MSG_ERROR([libcurl is missing - install it please])
             ])

AC_CHECK_LIB([pcre], [pcre_compile],
             [AC_DEFINE(HAVE_LIBPCRE, 1,
                        [Define if you want to use libpcre])
              LIBS="-lpcre $LIBS"],
             [AC_MSG_ERROR([libpcre is missing - install it please])
             ])

AC_CHECK_LIB([magic], [magic_open],
             [AC_DEFINE(HAVE_LIBMAGIC, 1,
                       [Define if you want to use libmagic])
              LIBS="-lmagic $LIBS"],
             [AC_MSG_ERROR([libmagic is missing - install it please])
             ])
AC_CHECK_HEADERS(magic.h)

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
                 tools/Makefile
                 tools/bdiffm/Makefile
                 tools/mkcarray/Makefile
                 modules/Makefile
                 modules/download-creceive/Makefile
                 modules/download-csend/Makefile
                 modules/download-curl/Makefile
                 modules/download-ftp/Makefile
                 modules/download-link/Makefile
                 modules/download-nepenthes/Makefile
                 modules/download-tftp/Makefile
                 modules/log-download/Makefile
                 modules/log-irc/Makefile
                 modules/module-portwatch/Makefile
                 modules/submit-file/Makefile
                 modules/submit-nepenthes/Makefile
                 modules/submit-norman/Makefile
                 modules/submit-xmlrpc/Makefile
                 modules/shellcode-generic/Makefile
                 modules/shellemu-winnt/Makefile
                 modules/vuln-asn1/Makefile
                 modules/vuln-bagle/Makefile
                 modules/vuln-dcom/Makefile
                 modules/vuln-iis/Makefile
                 modules/vuln-kuang2/Makefile
                 modules/vuln-lsass/Makefile
                 modules/vuln-msmq/Makefile
                 modules/vuln-mssql/Makefile
                 modules/vuln-mydoom/Makefile
                 modules/vuln-netbiosname/Makefile
                 modules/vuln-netdde/Makefile
                 modules/vuln-optix/Makefile
                 modules/vuln-pnp/Makefile
                 modules/vuln-sasserftpd/Makefile
                 modules/vuln-sub7/Makefile
                 modules/vuln-upnp/Makefile
                 modules/vuln-veritas/Makefile
                 modules/vuln-wins/Makefile
                 modules/vuln-dameware/Makefile
                 modules/x-1/Makefile
                 modules/x-2/Makefile
                 modules/x-3/Makefile
                 modules/x-4/Makefile
                 modules/x-5/Makefile                 
                 modules/x-6/Makefile
                 modules/x-7/Makefile])
#                modules/module-drone/Makefile
#                modules/vuln-netbios/Makefile
                 
AC_OUTPUT
