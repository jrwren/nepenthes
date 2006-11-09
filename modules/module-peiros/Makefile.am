# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id: Makefile.am 556 2006-05-31 05:04:17Z common $

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = modulepeiros.la

modulepeiros_la_SOURCES =  module-peiros.cpp module-peiros.hpp  
modulepeiros_la_SOURCES += PeirosParser.cpp  PeirosParser.hpp   
modulepeiros_la_SOURCES += TapInterface.cpp  TapInterface.hpp   

modulepeiros_la_LDFLAGS = -module -no-undefined -avoid-version
