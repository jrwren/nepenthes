# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = shellcodesignatures.la


shellcodesignatures_la_SOURCES = shellcode-signatures.sc 
shellcodesignatures_la_SOURCES += y.tab.c lex.yy.c
shellcodesignatures_la_SOURCES += shellcode-signatures.cpp shellcode-signatures.hpp 
shellcodesignatures_la_SOURCES += sch_namespace_xor.cpp sch_namespace_xor.hpp
shellcodesignatures_la_SOURCES += sch_namespace_bindshell.cpp sch_namespace_bindshell.hpp
shellcodesignatures_la_SOURCES += sch_namespace_connectbackshell.cpp sch_namespace_connectbackshell.hpp

shellcodesignatures_la_LDFLAGS = -module -no-undefined -avoid-version

