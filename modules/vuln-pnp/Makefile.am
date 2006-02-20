# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulnpnp.la

vulnpnp_la_SOURCES = PNPDialogue.cpp pnp-shellcodes.hpp vuln-pnp.cpp PNPDialogue.hpp vuln-pnp.conf.dist vuln-pnp.hpp

vulnpnp_la_LDFLAGS = -module -no-undefined -avoid-version
