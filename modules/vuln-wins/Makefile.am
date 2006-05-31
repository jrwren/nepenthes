# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = vulnwins.la

vulnwins_la_SOURCES = vuln-wins.conf.dist 
vulnwins_la_SOURCES += WINSDialogue.hpp 			WINSDialogue.cpp                       
vulnwins_la_SOURCES += vuln-wins.hpp wins-shellcodes.hpp	vuln-wins.cpp                          


vulnwins_la_LDFLAGS = -module -no-undefined -avoid-version
