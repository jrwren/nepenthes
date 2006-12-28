# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = ${LIB_SSH}

pkglib_LTLIBRARIES = vulnssh.la

vulnssh_la_SOURCES =  vuln-ssh.cpp vuln-ssh.hpp SSHSocket.cpp SSHSocket.hpp SSHDialogue.cpp SSHDialogue.hpp vuln-ssh.conf.dist

vulnssh_la_LDFLAGS = -module -no-undefined -avoid-version $(AM_LDFLAGS)
