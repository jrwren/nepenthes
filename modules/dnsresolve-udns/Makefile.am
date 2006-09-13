# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -ludns

pkglib_LTLIBRARIES = dnsresolveudns.la

dnsresolveudns_la_SOURCES = dnsresolve-udns.cpp dnsresolve-udns.hpp

dnsresolveudns_la_LDFLAGS = -module -no-undefined -avoid-version
