# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnlsass.la

vulnlsass_la_SOURCES = vuln-lsass.conf.dist LSASSDialogue.hpp lsass-shellcodes.hpp sch_lsass_hod_bind.hpp sch_lsass_hod_connect.hpp vuln-lsass.hpp LSASSDialogue.cpp sch_lsass_hod_bind.cpp sch_lsass_hod_connect.cpp vuln-lsass.cpp

vulnlsass_la_LDFLAGS = -module -no-undefined -avoid-version
