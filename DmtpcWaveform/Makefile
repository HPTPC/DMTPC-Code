CXX=g++
LDFLAGS=`root-config --libs` -Wl,-z,defs
CXXFLAGS= `root-config --cflags` -fPIC -O3
CXXFLAGS+=-I/scratch1/darkmatter/dmtpc/software/fftw3-dev/install/include/
LDFLAGS+=-L/scratch1/darkmatter/dmtpc/software/fftw3-dev/install/ -lfftw3

VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin



OBJS := $(addprefix $(BUILDDIR)/,  FirFilter.o FirKernels.o FourierTransform.o IirFilter.o RangeDomainFilter.o WaveformProcessing.o \
	                                 CspPulse.o CspWaveform.o CspWfVector.o FastPulse.o FastWaveform.o FastWfVector.o DmtpcWaveformCint.o\
																	 PMTPulse.o PMTWaveform.o PMTWfVector.o Pulse.o SkimWaveform.o WaveformAnalysis.o WaveformVector.o) 
INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


all: shared

shared: $(LIBDIR)/libDmtpcWaveform.so 

$(LIBDIR)/libDmtpcWaveform.so: $(OBJS)  | $(LIBDIR)
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -g -shared -o $@

$(OBJS): | $(BUILDDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/DmtpcWaveformCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	@rootcint -f $(BUILDDIR)/DmtpcWaveformCint.cc -c $(INCLUDES) LinkDef.h

$(BUILDDIR)/%.o: %.cc $(INCLUDES) | $(BUILDDIR) 
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 

clean:
	rm build/*.o
	rm build/*.cc
	rm build/*.h
	rm lib/*.so



