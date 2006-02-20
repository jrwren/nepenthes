# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror
AM_LDFLAGS = -lpcre

pkglib_LTLIBRARIES = shellemuwinnt.la

shellemuwinnt_la_SOURCES = VFS.hpp VFSCommand.hpp VFSCommandCMD.hpp VFSCommandDir.hpp VFSCommandECHO.hpp VFSCommandFTP.hpp VFSCommandREDIR.hpp VFSCommandRREDIR.hpp VFSCommandTFTP.hpp VFSDir.hpp VFSFile.hpp VFSNode.hpp WinNTShellDialogue.hpp shellemu-winnt.hpp VFS.cpp VFSCommandCMD.cpp VFSCommandDir.cpp VFSCommandECHO.cpp VFSCommandFTP.cpp VFSCommandREDIR.cpp VFSCommandRREDIR.cpp VFSCommandTFTP.cpp VFSDir.cpp VFSFile.cpp WinNTShellDialogue.cpp shellemu-winnt.cpp

shellemuwinnt_la_LDFLAGS = -module -no-undefined -avoid-version
