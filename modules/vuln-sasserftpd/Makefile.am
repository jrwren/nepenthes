# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnsasserftpd.la

vulnsasserftpd_la_SOURCES = vuln-sasserftpd.conf.dist SasserFTPDDialogue.hpp sch_sasserftpd_mandragore_bind.hpp sch_sasserftpd_mandragore_connect.hpp vuln-sasserftpd.hpp SasserFTPDDialogue.cpp sch_sasserftpd_mandragore_bind.cpp sch_sasserftpd_mandragore_connect.cpp vuln-sasserftpd.cpp

vulnsasserftpd_la_LDFLAGS = -module -no-undefined -avoid-version
