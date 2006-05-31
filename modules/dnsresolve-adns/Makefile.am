# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = $(LIB_ADNS)

pkglib_LTLIBRARIES = dnsresolveadns.la

dnsresolveadns_la_SOURCES = dnsresolve-adns.cpp dnsresolve-adns.hpp

dnsresolveadns_la_LDFLAGS = -module -no-undefined -avoid-version
