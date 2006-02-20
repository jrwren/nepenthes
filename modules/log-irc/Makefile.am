# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = logirc.la

logirc_la_SOURCES = log-irc.conf.dist IrcDialogue.hpp log-irc.hpp IrcDialogue.cpp log-irc.cpp

logirc_la_LDFLAGS = -module -no-undefined -avoid-version
