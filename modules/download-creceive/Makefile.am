# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = downloadcreceive.la

downloadcreceive_la_SOURCES = CReceiveDialogue.hpp download-creceive.hpp CReceiveDialogue.cpp download-creceive.cpp

downloadcreceive_la_LDFLAGS = -module -no-undefined -avoid-version
