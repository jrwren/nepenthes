# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = vulnnetdde.la

vulnnetdde_la_SOURCES = vuln-netdde.conf.dist 
vulnnetdde_la_SOURCES += NETDDEDialogue.hpp NETDDEDialogue.cpp      
vulnnetdde_la_SOURCES += vuln-netdde.hpp    vuln-netdde.cpp         



vulnnetdde_la_LDFLAGS = -module -no-undefined -avoid-version
