
AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror

pkglib_LTLIBRARIES = logattack.la

logattack_la_SOURCES = log-attack.conf.dist log-attack.cpp log-attack.hpp
logattack_la_LDFLAGS = -module -no-undefined -avoid-version

