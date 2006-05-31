# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = vulnveritas.la

vulnveritas_la_SOURCES = vuln-veritas.conf.dist VERITASDialogue.hpp vuln-veritas.hpp VERITASDialogue.cpp vuln-veritas.cpp

vulnveritas_la_LDFLAGS = -module -no-undefined -avoid-version
