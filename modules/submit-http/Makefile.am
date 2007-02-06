AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror

AM_LDFLAGS = $(LIB_CURL)

pkglib_LTLIBRARIES = submithttp.la

submithttp_la_SOURCES = submit-http.cpp submit-http.hpp 
submithttp_la_SOURCES += HTTPSession.hpp HTTPSession.cpp
submithttp_la_SOURCES += submit-http.conf.dist

submithttp_la_LDFLAGS = -module -no-undefined -avoid-version $(AM_LDFLAGS)
