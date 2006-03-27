# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnsasserftpd.la

vulnsasserftpd_la_SOURCES = vuln-sasserftpd.conf.dist               
vulnsasserftpd_la_SOURCES += SasserFTPDDialogue.hpp                   SasserFTPDDialogue.cpp                                             
vulnsasserftpd_la_SOURCES += vuln-sasserftpd.hpp                      vuln-sasserftpd.cpp                                                





vulnsasserftpd_la_LDFLAGS = -module -no-undefined -avoid-version
