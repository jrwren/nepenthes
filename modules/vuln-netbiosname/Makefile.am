# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = vulnnetbiosname.la

vulnnetbiosname_la_SOURCES = vuln-netbiosname.conf.dist SMBNameDialogue.hpp vuln-netbiosname.hpp SMBNameDialogue.cpp vuln-netbiosname.cpp

vulnnetbiosname_la_LDFLAGS = -module -no-undefined -avoid-version
