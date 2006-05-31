# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = modulebridge.la

modulebridge_la_SOURCES = module-bridge.cpp module.bridge.hpp
modulebridge_la_SOURCES += BridgeDialogueAccept.cpp BridgeDialogueAccept.hpp
modulebridge_la_SOURCES += BridgeDialogueConnect.cpp BridgeDialogueConnect.hpp

modulebridge_la_LDFLAGS = -module -no-undefined -avoid-version
