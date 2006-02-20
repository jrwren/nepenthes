# $Id$
# module makefile

# who am i?
MODNAME = $(shell basename `pwd`)

CXX = g++

CXXFLAGS += -I../../core/include -I../../core/src -pipe -D _GNU_SOURCE -Wall -Werror
LDFLAGS = -shared -lcurl

ifndef NDEBUG
CXXFLAGS += -g
endif

ifdef PROFILE
CXXFLAGS += -pg
endif


all: $(MODNAME) config

MOD_SOURCE := $(shell find -iname '*.cpp')
MOD_OBJ = $(MOD_SOURCE:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(MODNAME): $(MOD_OBJ)
	$(CXX) $(CXXFLAGS) -o $@.so $(MOD_OBJ) $(LDFLAGS)

config:
	sh ../../scripts/module_config_copy.sh

clean:
	rm -f *.o *.so

distclean: clean
	rm -f ${MODNAME}.conf
