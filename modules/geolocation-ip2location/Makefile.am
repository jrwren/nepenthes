# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -I/opt/ip2location/include/ -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = $IP2LOCATION_LIBS

pkglib_LTLIBRARIES = geolocationip2location.la

geolocationip2location_la_SOURCES = geolocation-ip2location.cpp geolocation-ip2location.hpp Ip2Location.c Ip2Location.h

geolocationip2location_la_LDFLAGS = -module -no-undefined -avoid-version
