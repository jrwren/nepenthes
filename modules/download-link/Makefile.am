# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = downloadlink.la

downloadlink_la_SOURCES = download-link.conf.dist LinkDialogue.hpp download-link.hpp LinkDialogue.cpp download-link.cpp

downloadlink_la_LDFLAGS = -module -no-undefined -avoid-version
