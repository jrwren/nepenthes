# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnlsass.la

vulnlsass_la_SOURCES = vuln-lsass.conf.dist lsass-shellcodes.hpp 
vulnlsass_la_SOURCES += LSASSDialogue.hpp LSASSDialogue.cpp    
vulnlsass_la_SOURCES += vuln-lsass.hpp    vuln-lsass.cpp       



vulnlsass_la_LDFLAGS = -module -no-undefined -avoid-version
