# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I/usr/include/libprelude -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE $(CPPFLAG_PRELUDE)
AM_CXXFLAGS = -Wall
AM_LDFLAGS = $(LDFLAG_PRELUDE) ${LIB_PRELUDE}

pkglib_LTLIBRARIES = logprelude.la

logprelude_la_SOURCES = log-prelude.cpp log-prelude.hpp log-prelude.conf.dist

logprelude_la_LDFLAGS = -module -no-undefined -avoid-version

