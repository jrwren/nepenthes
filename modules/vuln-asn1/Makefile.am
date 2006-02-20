# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnasn1.la

vulnasn1_la_SOURCES = vuln-asn1.conf.dist asn1-shellcodes.h IISDialogue.hpp SMBDialogue.hpp sch_asn1_iis.hpp sch_asn1_smb_bind.hpp vuln-asn1.hpp IISDialogue.cpp SMBDialogue.cpp sch_asn1_iis.cpp sch_asn1_smb_bind.cpp vuln-asn1.cpp

vulnasn1_la_LDFLAGS = -module -no-undefined -avoid-version
