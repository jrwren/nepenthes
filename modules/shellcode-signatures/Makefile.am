# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = shellcodesignatures.la


BUILD_SOURCES = parser.c

shellcodesignatures_la_SOURCES = shellcode-signatures.sc 
shellcodesignatures_la_SOURCES += parser.y
shellcodesignatures_la_SOURCES += shellcode-signatures.cpp shellcode-signatures.hpp 



shellcodesignatures_la_LDFLAGS = -module -no-undefined -avoid-version

CLEANFILES = parser.c
