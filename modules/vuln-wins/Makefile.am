# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnwins.la

vulnwins_la_SOURCES = vuln-wins.conf.dist WINSDialogue.hpp sch_wins_hs_bind.hpp sch_wins_hs_connect.hpp sch_wins_zuc_connect.hpp vuln-wins.hpp wins-shellcodes.hpp WINSDialogue.cpp sch_wins_hs_bind.cpp sch_wins_hs_connect.cpp sch_wins_zuc_connect.cpp vuln-wins.cpp

vulnwins_la_LDFLAGS = -module -no-undefined -avoid-version
