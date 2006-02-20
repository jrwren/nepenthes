# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -I/usr/include/postgresql -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
#AM_LDFLAGS = -lpq

pkglib_LTLIBRARIES = logsurfnet.la

logsurfnet_la_SOURCES = log-surfnet.cpp log-surfnet.hpp DatabaseConnection.cpp DatabaseConnection.hpp log-surfnet.conf.dist

logsurfnet_la_LDFLAGS = -module -no-undefined -avoid-version
