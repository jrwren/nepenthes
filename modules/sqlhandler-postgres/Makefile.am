# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpq

pkglib_LTLIBRARIES = sqlhandlerpostgres.la

sqlhandlerpostgres_la_SOURCES = sqlhandler-postgres.cpp sqlhandler-postgres.hpp

sqlhandlerpostgres_la_LDFLAGS = -module -no-undefined -avoid-version
