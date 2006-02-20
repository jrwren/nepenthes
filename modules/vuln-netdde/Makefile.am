# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnnetdde.la

vulnnetdde_la_SOURCES = vuln-netdde.conf.dist NETDDEDialogue.hpp sch_netdde_hod_bind.hpp sch_netdde_hod_connect.hpp vuln-netdde.hpp NETDDEDialogue.cpp sch_netdde_hod_bind.cpp sch_netdde_hod_connect.cpp vuln-netdde.cpp

vulnnetdde_la_LDFLAGS = -module -no-undefined -avoid-version
