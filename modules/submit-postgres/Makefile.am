# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = submitpostgres.la

submitpostgres_la_SOURCES = submit-postgres.cpp submit-postgres.hpp 
submitpostgres_la_SOURCES += PGDownloadContext.cpp PGDownloadContext.hpp 
submitpostgres_la_SOURCES += bencoding.c bencoding.h
submitpostgres_la_SOURCES += submit-postgres.conf.dist

submitpostgres_la_LDFLAGS = -module -no-undefined -avoid-version
