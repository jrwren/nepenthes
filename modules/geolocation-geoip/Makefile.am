# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -I/opt/geoip/include/ -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
# AM_LDFLAGS = /opt/geoip/lib/libGeoIP.a

pkglib_LTLIBRARIES = geolocationgeoip.la

geolocationgeoip_la_SOURCES = geolocation-geoip.cpp geolocation-geoip.hpp

geolocationgeoip_la_LDFLAGS = -module -no-undefined -avoid-version
