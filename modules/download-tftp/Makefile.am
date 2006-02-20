# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -module -no-undefined -avoid-version -lpcre

pkglib_LTLIBRARIES = downloadtftp.la

downloadtftp_la_SOURCES = download-tftp.conf.dist TFTPDialogue.hpp TFTPDownloadHandler.hpp TFTPDownloadHandler.hpp TFTPDialogue.cpp TFTPDownloadHandler.cpp

downloadtftp_la_LDFLAGS = -module -no-undefined -avoid-version
