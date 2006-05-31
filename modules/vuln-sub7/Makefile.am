# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = vulnsub7.la

vulnsub7_la_SOURCES = vuln-sub7.conf.dist SUB7Dialogue.hpp vuln-sub7.hpp SUB7Dialogue.cpp vuln-sub7.cpp

vulnsub7_la_LDFLAGS = -module -no-undefined -avoid-version
