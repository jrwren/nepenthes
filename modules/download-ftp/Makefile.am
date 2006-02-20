# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = downloadftp.la

downloadftp_la_SOURCES = CTRLDialogue.hpp FILEDialogue.hpp FTPContext.hpp download-ftp.hpp CTRLDialogue.cpp FTPContext.cpp FILEDialogue.cpp download-ftp.cpp

downloadftp_la_LDFLAGS = -module -no-undefined -avoid-version
