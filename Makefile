# nepenthes Makefile
# Paul Baecher, Markus Koetter
# $Id$

#
# compiler flags
#

CXX = g++
CXXFLAGS += -Icore/include  
CXXFLAGS +=  -D _GNU_SOURCE -Wall -Werror

ifndef NDEBUG
CXXFLAGS += -g
endif

#
# linker flags
#

LDFLAGS = -ldl -lmagic -ladns

#
# what makes a complete nepenthes build?
#

MODULES += x-1 x-2 x-3 x-4 x-5 x-6 # x-7
MODULES += download-tftp download-csend download-curl download-nepenthes download-ftp download-creceive download-link
MODULES += shellcode-generic
MODULES += vuln-dcom vuln-wins vuln-optix vuln-sub7 vuln-bagle vuln-mydoom vuln-kuang2 vuln-lsass vuln-mssql vuln-asn1 vuln-iis vuln-msmq vuln-netbiosname 
MODULES += vuln-netdde vuln-upnp vuln-sasserftpd vuln-veritas
MODULES += submit-file submit-norman submit-nepenthes # submit-postgres
MODULES += shellemu-winnt
MODULES += log-download log-irc
# MODULES += log-prelude

MODULES += module-portwatch

#
# Version?
#
VERSION := $(shell if test -d .svn; then svnversion .; fi)

ifndef VERSION
VERSION := $(shell if test -f version; then cat version; fi)
endif

ifndef VERSION
VERSION := unknown
endif

ifdef RELEASE
VERSION := RELEASE
endif

#
# different aliases
#

all: core modules
core: nepenthes
modules: $(MODULES)

release:
	scripts/mkrelease.sh

# recursively call make for each module
$(MODULES):
	$(MAKE) -C modules/$@

#
# which obj files belong to the different parts?
#

CORE_SOURCE := $(shell find core/src/ -iname '*.cpp')
CORE_OBJ = $(CORE_SOURCE:.cpp=.o)

#
# how to build the obj files?
#

core/src/%.o: core/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -D NEPENTHES_VERSION=\"$(VERSION)\" -o $@ $<

#
# how to link the different parts?
#

nepenthes: $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o bin/$@ $(CORE_OBJ) $(LDFLAGS)

#
# cleaning up...
#

clean_core:
	rm -f core/src/*.o bin/nepenthes

clean_modules:
	$(foreach mod, $(MODULES), make clean -C modules/$(mod);)
	

distclean_modules:
	$(foreach mod, $(MODULES), make distclean -C modules/$(mod);)

clean: clean_core clean_modules

distclean: clean_core distclean_modules
