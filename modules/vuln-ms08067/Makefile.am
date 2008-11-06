# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = vulnms08067.la

vulnms08067_la_SOURCES = vuln-ms08067.conf.dist vuln-ms08067.cpp vuln-ms08067.hpp

vulnms08067_la_LDFLAGS = -module -no-undefined -avoid-version
