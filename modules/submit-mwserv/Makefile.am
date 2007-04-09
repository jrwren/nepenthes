# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id: Makefile.am 718 2006-12-28 23:29:59Z common $

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror

AM_LDFLAGS = $(LIB_CURL)

pkglib_LTLIBRARIES = submitmwserv.la

submitmwserv_la_SOURCES = submit-mwserv.cpp submit-mwserv.hpp
submitmwserv_la_SOURCES += TransferSession.cpp TransferSession.hpp
submitmwserv_la_SOURCES += submit-mwserv.conf.dist

submitmwserv_la_CXXFLAGS = -fno-strict-aliasing
submitmwserv_la_LDFLAGS = -module -no-undefined -avoid-version $(AM_LDFLAGS)
