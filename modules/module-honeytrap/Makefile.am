# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE -I/usr/include/libipq
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = ${LIB_PCAP} ${LIB_IPQ} 

pkglib_LTLIBRARIES = modulehoneytrap.la

modulehoneytrap_la_SOURCES = module-honeytrap.cpp module-honeytrap.hpp module-honeytrap.conf.dist

modulehoneytrap_la_LDFLAGS = -module -no-undefined -avoid-version
