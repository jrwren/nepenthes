# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror

pkglib_LTLIBRARIES = vulnbagle.la

vulnbagle_la_SOURCES = vuln-bagle.conf.dist BagleDialogue.hpp vuln-bagle.hpp BagleDialogue.cpp vuln-bagle.cpp

vulnbagle_la_LDFLAGS = -module -no-undefined -avoid-version
