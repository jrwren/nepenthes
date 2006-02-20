# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulniis.la

vulniis_la_SOURCES = vuln-iis.conf.dist iis-shellcodes.h IISDialogue.hpp sch_iis_tc_bind.hpp sch_iis_thc_bind.hpp sch_iis_thc_connect.hpp vuln-iis.hpp IISDialogue.hpp sch_iis_tc_bind.hpp sch_iis_thc_bind.hpp sch_iis_thc_connect.hpp vuln-iis.hpp IISDialogue.cpp sch_iis_tc_bind.cpp sch_iis_thc_bind.cpp sch_iis_thc_connect.cpp vuln-iis.cpp

vulniis_la_LDFLAGS = -module -no-undefined -avoid-version
