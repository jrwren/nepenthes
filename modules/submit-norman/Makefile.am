# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = submitnorman.la

submitnorman_la_SOURCES = submit-norman.conf.dist submit-norman.hpp submit-norman.cpp

submitnorman_la_LDFLAGS = -module -no-undefined -avoid-version
