# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = ${LIB_PCRE}

pkglib_LTLIBRARIES = vulnoptix.la

vulnoptix_la_SOURCES = vuln-optix.conf.dist OPTIXBindDialogue.hpp OPTIXDownloadDialogue.hpp OPTIXDownloadHandler.hpp OPTIXShellDialogue.hpp vuln-optix.hpp OPTIXBindDialogue.cpp OPTIXDownloadDialogue.cpp OPTIXDownloadHandler.cpp OPTIXShellDialogue.cpp vuln-optix.cpp

vulnoptix_la_LDFLAGS = -module -no-undefined -avoid-version
