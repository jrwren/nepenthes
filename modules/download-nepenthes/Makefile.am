# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = downloadnepenthes.la

downloadnepenthes_la_SOURCES = download-nepenthes.conf.dist DownloadNepenthesDialogue.hpp download-nepenthes.hpp DownloadNepenthesDialogue.cpp download-nepenthes.cpp

downloadnepenthes_la_LDFLAGS = -module -no-undefined -avoid-version
