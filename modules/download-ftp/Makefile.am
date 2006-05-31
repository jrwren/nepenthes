# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = downloadftp.la

downloadftp_la_SOURCES = CTRLDialogue.hpp CTRLDialogue.cpp        
downloadftp_la_SOURCES += FILEDialogue.hpp FILEDialogue.cpp        
downloadftp_la_SOURCES += FTPContext.hpp   FTPContext.cpp          
downloadftp_la_SOURCES += download-ftp.hpp download-ftp.cpp        
downloadftp_la_SOURCES += download-ftp.conf.dist

downloadftp_la_LDFLAGS = -module -no-undefined -avoid-version
