# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnmydoom.la

vulnmydoom_la_SOURCES = vuln-mydoom.conf.dist MydoomDialogue.hpp vuln-mydoom.hpp MydoomDialogue.cpp vuln-mydoom.cpp

vulnmydoom_la_LDFLAGS = -module -no-undefined -avoid-version
