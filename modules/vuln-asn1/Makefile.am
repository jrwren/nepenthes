# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnasn1.la

vulnasn1_la_SOURCES = vuln-asn1.conf.dist asn1-shellcodes.h 
vulnasn1_la_SOURCES += vuln-asn1.hpp   vuln-asn1.cpp               
vulnasn1_la_SOURCES += IISDialogue.hpp IISDialogue.cpp               
vulnasn1_la_SOURCES += SMBDialogue.hpp SMBDialogue.cpp               





vulnasn1_la_LDFLAGS = -module -no-undefined -avoid-version
