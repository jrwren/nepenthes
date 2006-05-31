# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = $(LIB_CURL)

pkglib_LTLIBRARIES = downloadcurl.la

downloadcurl_la_SOURCES = download-curl.conf.dist download-curl.hpp download-curl.cpp

downloadcurl_la_LDFLAGS = -module -no-undefined -avoid-version
