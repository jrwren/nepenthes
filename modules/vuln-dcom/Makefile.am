# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = vulndcom.la

vulndcom_la_SOURCES = vuln-dcom.conf.dist dcom-shellcodes.h 
vulndcom_la_SOURCES += DCOMDialogue.hpp DCOMDialogue.cpp                      
vulndcom_la_SOURCES += vuln-dcom.hpp    vuln-dcom.cpp                         



vulndcom_la_LDFLAGS = -module -no-undefined -avoid-version
