# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulndameware.la

vulndameware_la_SOURCES = vuln-dameware.cpp vuln-dameware.hpp DWDialogue.cpp DWDialogue.hpp vuln-dameware.conf.dist

vulndameware_la_LDFLAGS = -module -no-undefined -avoid-version
