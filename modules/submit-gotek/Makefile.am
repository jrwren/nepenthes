# nepenthes module Makefile
# Maximillian Dornseif
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = submitgotek.la

submitgotek_la_SOURCES = gotekDATADialogue.cpp submit-gotek.cpp gotekCTRLDialogue.cpp gotekDATADialogue.hpp submit-gotek.hpp gotekCTRLDialogue.hpp submit-gotek.conf.dist

submitgotek_la_LDFLAGS = -module -no-undefined -avoid-version
