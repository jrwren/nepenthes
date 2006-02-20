# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnmsmq.la

vulnmsmq_la_SOURCES = vuln-msmq.conf.dist MSMQDialogue.hpp sch_msmq_hod_bind.hpp vuln-msmq.hpp MSMQDialogue.cpp sch_msmq_hod_bind.cpp vuln-msmq.cpp

vulnmsmq_la_LDFLAGS = -module -no-undefined -avoid-version
