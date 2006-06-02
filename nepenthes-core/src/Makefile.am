# nepenthes Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../include -I ../.. -Wall -Werror
AM_LDFLAGS = $(LIB_MAGIC) $(LIB_CAP) -ldl

bin_PROGRAMS = nepenthes

nepenthes_SOURCES = Buffer.cpp 
nepenthes_SOURCES += Compatibility.cpp 
nepenthes_SOURCES += Config.cpp 
nepenthes_SOURCES += Dialogue.cpp DialogueFactory.cpp DialogueFactoryManager.cpp
nepenthes_SOURCES += DNSManager.cpp DNSHandler.cpp DNSQuery.cpp DNSResult.cpp DNSCallback.cpp
nepenthes_SOURCES += DownloadManager.cpp DownloadHandler.cpp DownloadBuffer.cpp Download.cpp DownloadUrl.cpp
nepenthes_SOURCES += EventManager.cpp EventHandler.cpp 
nepenthes_SOURCES += LogManager.cpp LogHandler.cpp ConsoleLogger.cpp FileLogger.cpp RingFileLogger.cpp
nepenthes_SOURCES += LuaFunctions.cpp LuaInterface.cpp
nepenthes_SOURCES += Message.cpp
nepenthes_SOURCES += ModuleManager.cpp Module.cpp 
nepenthes_SOURCES += Nepenthes.cpp Manager.cpp
nepenthes_SOURCES += Packet.cpp
nepenthes_SOURCES += SocketManager.cpp Socket.cpp TCPSocket.cpp UDPSocket.cpp POLLSocket.cpp FILESocket.cpp
nepenthes_SOURCES += Responder.cpp
nepenthes_SOURCES += ShellcodeManager.cpp ShellcodeHandler.cpp 
nepenthes_SOURCES += SubmitManager.cpp SubmitHandler.cpp 
nepenthes_SOURCES += Utilities.cpp


