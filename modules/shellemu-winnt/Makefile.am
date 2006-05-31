# nepenthes module Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir)/nepenthes-core/include -I$(top_srcdir)/nepenthes-core/src -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror


pkglib_LTLIBRARIES = shellemuwinnt.la


shellemuwinnt_la_SOURCES =  VFS.cpp VFS.hpp 
shellemuwinnt_la_SOURCES += shellemu-winnt.cpp shellemu-winnt.hpp 
shellemuwinnt_la_SOURCES += VFSCommand.hpp 
shellemuwinnt_la_SOURCES += VFSNode.hpp 
shellemuwinnt_la_SOURCES += VFSDir.cpp VFSDir.hpp 
shellemuwinnt_la_SOURCES += VFSFile.cpp VFSFile.hpp 
shellemuwinnt_la_SOURCES += WinNTShellDialogue.cpp WinNTShellDialogue.hpp 
shellemuwinnt_la_SOURCES += VFSCommandCMD.cpp VFSCommandCMD.hpp 
shellemuwinnt_la_SOURCES += VFSCommandDir.cpp VFSCommandDir.hpp 
shellemuwinnt_la_SOURCES += VFSCommandECHO.cpp VFSCommandECHO.hpp 
shellemuwinnt_la_SOURCES += VFSCommandFTP.cpp  VFSCommandFTP.hpp 
shellemuwinnt_la_SOURCES += VFSCommandREDIR.cpp VFSCommandREDIR.hpp 
shellemuwinnt_la_SOURCES += VFSCommandRREDIR.cpp VFSCommandRREDIR.hpp 
shellemuwinnt_la_SOURCES += VFSCommandTFTP.cpp  VFSCommandTFTP.hpp 
shellemuwinnt_la_SOURCES += VFSCommandSTART.cpp VFSCommandSTART.hpp 
shellemuwinnt_la_SOURCES += VFSCommandRCP.cpp VFSCommandRCP.hpp 

shellemuwinnt_la_LDFLAGS = -module -no-undefined -avoid-version
