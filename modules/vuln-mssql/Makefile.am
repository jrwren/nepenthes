# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror

pkglib_LTLIBRARIES = vulnmssql.la

vulnmssql_la_SOURCES = vuln-mssql.conf.dist MSSQLDialogue.hpp vuln-mssql.hpp MSSQLDialogue.cpp vuln-mssql.cpp mssql-shellcodes.h

vulnmssql_la_LDFLAGS = -module -no-undefined -avoid-version
