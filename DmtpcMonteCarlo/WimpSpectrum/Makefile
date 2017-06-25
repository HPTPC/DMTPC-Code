ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)
ARCH = $(shell root-config --arch)


CXXFLAGS = -g -fPIC -O0 -I./include $(ROOTCFLAGS)  

LDFLAGS = -m64 -fPIC $(ROOTLIBS) 
CINTFLAGS = -I./include

CPP = g++
LD = g++
SOFLAGS = -shared -export-dynamic
STATICFLAGS = cq
AR = ar
RANLIB = ranlib
CINT = rootcint



OBJS = .tmp/WimpSpectrum.Cint \
       .tmp/Mollweide.o \
       .tmp/VelocityDistribution.o \
       .tmp/RNG.o \
       .tmp/WimpGen.o




all: libdir lib

install: lib

lib: shared

liball: shared static

shared: tmpdir $(OBJS)
	$(LD) $(SOFLAGS) $(CXXFLAGS) $(OBJS) -m64 -fPIC $(ROOTLIBS) -o .tmp/libWimpSpectrum.so
	cp .tmp/libWimpSpectrum.so lib/	

static: tmpdir $(OBJS)
	$(AR) $(STATICFLAGS) .tmp/libWimpSpectrum.a .tmp/*.o
	$(RANLIB) .tmp/libWimpSpectrum.a
	cp .tmp/libWimpSpectrum.a lib/

clean:
	rm -f lib/*
	rm -f .tmp/*

libdir:
	mkdir -p lib

tmpdir:
	mkdir -p .tmp

.tmp/%.o: src/%.cc 
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.tmp/WimpSpectrum.Cint: include/Mollweide.hh include/WimpGen.hh
	$(CINT) -f .tmp/WimpSpectrumCint.cc -c -Iinclude WimpGen.hh Mollweide.hh  VelocityDistribution.hh include/WimpSpectrumLinkDef.h
	$(CXX) $(CXXFLAGS) -o .tmp/WimpSpectrum.Cint -c .tmp/WimpSpectrumCint.cc


.PHONY: all, lib, install, static, shared, liball, clean, tmpdir, libdir
