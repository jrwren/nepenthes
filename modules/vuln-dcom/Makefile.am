# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = vulndcom.la

vulndcom_la_SOURCES = vuln-dcom.conf.dist dcom-shellcodes.h DCOMDialogue.hpp sch_dcom_oc192_bind.hpp sch_dcom_sol2k_bind.hpp sch_dcom_sol2k_connect.hpp vuln-dcom.hpp DCOMDialogue.cpp sch_dcom_oc192_bind.cpp sch_dcom_sol2k_bind.cpp sch_dcom_sol2k_connect.cpp vuln-dcom.cpp

vulndcom_la_LDFLAGS = -module -no-undefined -avoid-version
