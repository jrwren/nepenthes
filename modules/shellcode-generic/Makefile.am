# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = shellcodegeneric.la

shellcodegeneric_la_SOURCES = shellcode-generic.conf.dist sch_generic_connect_trans.cpp sch_generic_connect_trans.hpp sch_generic_konstanz_xor.cpp sch_generic_konstanz_xor.hpp sch_generic_bielefeld_connect.hpp sch_generic_bind.hpp sch_generic_cmd.hpp sch_generic_connect.hpp sch_generic_createprocess.hpp sch_generic_link_bind_trans.hpp sch_generic_link_trans.hpp sch_generic_linkxor.hpp sch_generic_mainz_bind.hpp sch_generic_stuttgart.hpp sch_generic_url.hpp sch_generic_wuerzburg.hpp sch_generic_xor.hpp shellcode-generic.hpp sch_generic_bielefeld_connect.cpp sch_generic_bind.cpp sch_generic_cmd.cpp sch_generic_connect.cpp sch_generic_createprocess.cpp sch_generic_link_bind_trans.cpp sch_generic_link_trans.cpp sch_generic_linkxor.cpp sch_generic_mainz_bind.cpp sch_generic_stuttgart.cpp sch_generic_url.cpp sch_generic_wuerzburg.cpp sch_generic_xor.cpp shellcode-generic.cpp sch_generic_unicode.cpp sch_generic_unicode.hpp

shellcodegeneric_la_LDFLAGS = -module -no-undefined -avoid-version
