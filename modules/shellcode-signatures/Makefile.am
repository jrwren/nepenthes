# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_CFLAGS = -Wall
AM_LDFLAGS = ${LIB_PCRE} @LEXLIB@ ${LIB_LIBY}

YACC += -d


pkglib_LTLIBRARIES = shellcodesignatures.la


shellcodesignatures_la_SOURCES = shellcode-signatures.sc 
shellcodesignatures_la_SOURCES += signature_parser.y signature_scanner.l  
shellcodesignatures_la_SOURCES += parser.hpp parser.h
shellcodesignatures_la_SOURCES += shellcode-signatures.cpp shellcode-signatures.hpp 
shellcodesignatures_la_SOURCES += sch_namespace.cpp sch_namespace.hpp
shellcodesignatures_la_SOURCES += sch_namespace_xor.cpp sch_namespace_xor.hpp
shellcodesignatures_la_SOURCES += sch_namespace_bindshell.cpp sch_namespace_bindshell.hpp
shellcodesignatures_la_SOURCES += sch_namespace_connectbackshell.cpp sch_namespace_connectbackshell.hpp
shellcodesignatures_la_SOURCES += sch_namespace_execute.cpp sch_namespace_execute.hpp
shellcodesignatures_la_SOURCES += sch_namespace_url.cpp sch_namespace_url.hpp
shellcodesignatures_la_SOURCES += sch_namespace_linkxor.cpp sch_namespace_linkxor.hpp
shellcodesignatures_la_SOURCES += sch_namespace_connectbackfiletransfer.cpp sch_namespace_connectbackfiletransfer.hpp
shellcodesignatures_la_SOURCES += sch_namespace_bindfiletransfer.cpp sch_namespace_bindfiletransfer.hpp
shellcodesignatures_la_SOURCES += sch_namespace_base64.cpp sch_namespace_base64.hpp
shellcodesignatures_la_SOURCES += sch_engine_unicode.cpp sch_engine_unicode.hpp
shellcodesignatures_la_SOURCES += sch_namespace_konstanzxor.cpp sch_namespace_konstanzxor.hpp
shellcodesignatures_la_SOURCES += sch_namespace_alphanumericxor.cpp sch_namespace_alphanumericxor.hpp

shellcodesignatures_la_LDFLAGS = -module -no-undefined -avoid-version

