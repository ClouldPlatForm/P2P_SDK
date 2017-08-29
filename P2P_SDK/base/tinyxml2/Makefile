#all: xmltest
#xmltest: xmltest.cpp tinyxml2.cpp tinyxml2.h

#libstun.a: stun.o udp.o
#	ar rv $@ $<

#test: clean xmltest
#	./xmltest
#clean:
#	rm -f *.o xmltest
include ../SkyNatConfig.conf

VERSION=0.96

#CXXFLAGS+=-O2
#LDFLAGS+=-O2 -lssl
CC=$(COMPILETOOL_PREFIX)g++
AR=$(COMPILETOOL_PREFIX)ar

#
# Alternatively, for debugging.
#
CXXFLAGS+=-g -O -Wall
LDFLAGS+=-g -O -Wall
# for solaris
#LDFLAGS+= -lnsl -lsocket


all: xmltest libtinyxml2.a

clean:
	- rm *.o libtinyxml2.a   


xmltest:xmltest.o tinyxml2.o
	$(CC) $(LDFLAGS) -o $@  $^
	
%.o:%.cpp
	$(CC)  -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

libtinyxml2.a: tinyxml2.o
	$(AR) rv $@ $<

%:RCS/%
	co $@





