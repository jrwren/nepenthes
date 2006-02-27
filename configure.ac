# -*- mode: m4; -*-
# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# nepenthes meta-Makefile
# Maximillian Dornseif 2005
# $Id$

AC_PREREQ(2.59)
AC_INIT([nepenthes], [0.1.6], [dornseif@informatik.rwth-aachen.de])
AM_INIT_AUTOMAKE([nepenthes], [0.1.6])
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
    orig_cppflags=$CPPFLAGS
  
    if test "x$curl_inc" != xno; then
      CPPFLAGS="-I$curl_inc $CPPFLAGS"
    fi

    orig_ldflags=$LDFLAGS
    if test "x$curl_lib" != "xno"; then
        LDFLAGS="-L$curl_lib $LDFLAGS"
    fi

    AC_CHECK_LIB([curl], [curl_version],
             [AC_DEFINE(HAVE_LIBCURL, 1,
                       [Define if you want to use libcurl])
              LIBS="-lcurl $LIBS"],
             [AC_MSG_ERROR([libcurl is missing - install it please])
             ])
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
              LIBS="-lpcre $LIBS"],
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


    AC_CHECK_LIB([magic], [magic_open],
             [AC_DEFINE(HAVE_LIBMAGIC, 1,
                       [Define if you want to use libmagic])
              LIBS="-lmagic $LIBS"],
             [AC_MSG_ERROR([libmagic is missing - install it please])
             ])
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
    LIBS="-lssh $LIBS"
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
    LIBS="-lmysqlclient $LIBS"
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
    LIBS="-lpq $LIBS"
  fi

  AC_MSG_RESULT($enable_postgre)  
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
dnl * GeoLocation Support                            *
dnl **************************************************

AC_ARG_ENABLE(geolocation, [  --enable-geolocation	Enable GeoLocation [default=no]], 
		    enable_geolocation="$enableval", enable_geolocation="no")

AC_ARG_ENABLE(geolocation_hostip, [  --enable-geolocation-hostip	Enable GeoIP [default=auto]], 
		    enable_geolocation_hostip="$enableval", enable_geolocation_hostip="yes")

AC_ARG_ENABLE(geolocation_ip2location, [  --enable-geolocation-ip2location	Enable IP2Location [default=auto]], 
		    enable_geolocation_ip2location="$enableval", enable_geolocation_ip2location="yes")
		    
AC_ARG_ENABLE(geolocation_geoip, [  --enable-geolocation-geoip	Enable GeoIP [default=auto]], 
		    enable_geolocation_geoip="$enableval", enable_geolocation_geoip="yes")


if test x$enable_geolocation = xyes; then


  AC_DEFINE_UNQUOTED(HAVE_GEOLOCATION, 1, [Define wether we want to use GeoLocation])
  

  AC_ARG_WITH(geoip-include,
    [  --with-geoip-include=PATH	specify path to GeoIP include files],
    [geoip_inc=$withval],
    [geoip_inc=no])

  AC_ARG_WITH(geoip-lib,
    [  --with-geoip-lib=PATH	specify path to GeoIP client library],
    [geoip_lib=$withval],
    [geoip_lib=no])


  AC_ARG_WITH(ip2location-include,
    [  --with-ip2location-include=PATH	specify path to IP2Location include files],
    [ip2location_inc=$withval],
    [ip2location_inc=no])

  AC_ARG_WITH(ip2location-lib,
    [  --with-ip2location-lib=PATH	specify path to IP2Location client library],
    [ip2location_lib=$withval],
    [ip2location_lib=no])


  dnl **************************************************
  dnl * GeoIP Support                                  *
  dnl **************************************************

  if test x$enable_geolocation_geoip = xyes; then

    dnl **************************************************
    dnl * GeoIP headers                                  *
    dnl **************************************************

    orig_cppflags=$CPPFLAGS
  
    if test "x$geoip_inc" != xno; then
      CPPFLAGS="-I$geoip_inc $CPPFLAGS"
    fi
  
    AC_CHECK_HEADER(GeoIP.h, [found=yes], [found=no])

    if test "x$found" != "xno"; then
      dnl **************************************************
      dnl * GeoIP lib                                      *
      dnl **************************************************
	
      orig_ldflags=$LDFLAGS
      if test "x$geoip_lib" != "xno"; then
          LDFLAGS="-L$geoip_lib $LDFLAGS"
      fi
      AC_CHECK_LIB([GeoIP], [GeoIP_open], [found=yes], [found=no])
    
      if test "x$found" != "xyes"; then
          LDFLAGS=$orig_ldflags
      fi
    else    
       CPPFLAGS=$orig_cppflags
    fi

    if test "x$found" != "xyes"; then
      enable_geolocation_geoip=no
    else
      AC_DEFINE(HAVE_LIBGEOIP, 1,[Define if you want to use MaxMind's GeoIP])
      LIBS="-lGeoIP $LIBS"
    fi
    
  fi  

    
  



  dnl **************************************************
  dnl * IP2Location Support                            *
  dnl **************************************************

  if test x$enable_geolocation_ip2location = xyes; then

    dnl **************************************************
    dnl * IP2location headers                            *
    dnl **************************************************

    orig_cppflags=$CPPFLAGS
  
    if test "x$ip2location_inc" != xno; then
      CPPFLAGS="-I$ip2location_inc $CPPFLAGS"
    fi
  
    AC_CHECK_HEADER(IP2Location.h, [found=yes], [found=no])

    if test "x$found" != "xno"; then
      dnl **************************************************
      dnl * IP2Location lib                                *
      dnl **************************************************
	
      orig_ldflags=$LDFLAGS

      if test "x$ip2location_lib" != "xno"; then
          LDFLAGS="-L$ip2location_lib -lIP2Location $LDFLAGS"
      fi
      
      AC_CHECK_LIB([IP2Location], [IP2Location_open], [found=yes], [found=no])
    
      if test "x$found" != "xyes"; then
          LDFLAGS=$orig_ldflags
      fi
    else    
       CPPFLAGS=$orig_cppflags
    fi

    if test "x$found" != "xyes"; then
      enable_geolocation_ip2location=no
    else
      AC_DEFINE(HAVE_LIBIP2LOCATION, 1,[Define if you want to use IP2Location])
      LIBS="-lIP2Location $LIBS"
    fi
  
  fi  
  
else   
  enable_geolocation_hostip=no
  enable_geolocation_geoip=no
  enable_geolocation_ip2location=no

fi




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
            LIBS="-ladns $LIBS"],
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
	PRELUDE_CPPFLAGS=`$LIBPRELUDE_CONFIG --cflags`
	
	LIBS="$PRELUDE_LIBS $LIBS"
	LDFLAGS="$PRELUDE_LDFLAGS $LDFLAGS"
	CPPFLAGS="$PRELUDE_CPPFLAGS $CPPFLAGS"
	
    fi
fi

    



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
		 modules/vuln-ssh/Makefile
                 modules/x-1/Makefile
                 modules/x-2/Makefile
                 modules/x-3/Makefile
                 modules/x-4/Makefile
                 modules/x-5/Makefile                 
                 modules/x-6/Makefile
                 modules/x-7/Makefile
                 modules/x-8/Makefile
		 modules/geolocation-hostip/Makefile
		 modules/geolocation-geoip/Makefile
                 modules/download-http/Makefile
		 modules/upload-http/Makefile
		 modules/dnsresolve-adns/Makefile
		 modules/geolocation-ip2location/Makefile
		 modules/log-surfnet/Makefile		 
		 modules/vuln-msdtc/Makefile
		 modules/download-rcp/Makefile
		 modules/vuln-ftpd/Makefile
		 modules/log-prelude/Makefile
		 ])
#		 modules/log-prelude/Makefile
#                modules/module-drone/Makefile		 
#		 modules/log-surfnet/Makefile])
#                modules/vuln-netbios/Makefile
#                 modules/module-baitnswitch/Makefile
                 
                 
AC_OUTPUT

echo
echo "*** Dumping Nepenthes Configuration ***"
echo "- DNS Resolve                 "
echo "  - Support adns              : $enable_dnsresolve_adns"
echo ""
echo "- Support GeoLocation         : $enable_geolocation"
echo "  - Support hostip            : $enable_geolocation_hostip"
echo "  - Support GeoIP             : $enable_geolocation_geoip"
echo "  - Support IP2Location       : $enable_geolocation_ip2location"
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

# echo ""
# echo "- Support ipv6                : $enable_ipv6"
