# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnkuang2.la

vulnkuang2_la_SOURCES = vuln-kuang2.conf.dist Kuang2Dialogue.hpp vuln-kuang2.hpp Kuang2Dialogue.cpp vuln-kuang2.cpp

vulnkuang2_la_LDFLAGS = -module -no-undefined -avoid-version
