# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = shellcodegeneric.la

shellcodegeneric_la_SOURCES = shellcode-generic.conf.dist 
shellcodegeneric_la_SOURCES += shellcode-generic.cpp           shellcode-generic.hpp 
shellcodegeneric_la_SOURCES += sch_generic_connect_trans.cpp   sch_generic_connect_trans.hpp 
shellcodegeneric_la_SOURCES += sch_generic_konstanz_xor.cpp    sch_generic_konstanz_xor.hpp 
shellcodegeneric_la_SOURCES += sch_generic_bielefeld_connect.cpp sch_generic_bielefeld_connect.hpp 
shellcodegeneric_la_SOURCES += sch_generic_bind.cpp            sch_generic_bind.hpp 
shellcodegeneric_la_SOURCES += sch_generic_cmd.cpp             sch_generic_cmd.hpp 
shellcodegeneric_la_SOURCES += sch_generic_connect.cpp         sch_generic_connect.hpp 
shellcodegeneric_la_SOURCES += sch_generic_createprocess.cpp   sch_generic_createprocess.hpp 
shellcodegeneric_la_SOURCES += sch_generic_link_bind_trans.cpp sch_generic_link_bind_trans.hpp 
shellcodegeneric_la_SOURCES += sch_generic_link_trans.cpp      sch_generic_link_trans.hpp 
shellcodegeneric_la_SOURCES += sch_generic_linkxor.cpp         sch_generic_linkxor.hpp 
shellcodegeneric_la_SOURCES += sch_generic_mainz_bind.cpp      sch_generic_mainz_bind.hpp 
shellcodegeneric_la_SOURCES += sch_generic_stuttgart.cpp       sch_generic_stuttgart.hpp 
shellcodegeneric_la_SOURCES += sch_generic_url.cpp             sch_generic_url.hpp 
shellcodegeneric_la_SOURCES += sch_generic_wuerzburg.cpp       sch_generic_wuerzburg.hpp 
shellcodegeneric_la_SOURCES += sch_generic_xor.cpp             sch_generic_xor.hpp 
shellcodegeneric_la_SOURCES += sch_generic_unicode.cpp         sch_generic_unicode.hpp
shellcodegeneric_la_SOURCES += sch_generic_winexec.cpp         sch_generic_winexec.hpp
shellcodegeneric_la_SOURCES += sch_generic_leimbach_url_xor.cpp sch_generic_leimbach_url_xor.hpp

shellcodegeneric_la_LDFLAGS = -module -no-undefined -avoid-version
