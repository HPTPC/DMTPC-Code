CXX=g++
LDFLAGS=`root-config --libs` 
CXXFLAGS= `root-config --cflags` -fPIC -O3 -g 

VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin



OBJS := $(addprefix $(BUILDDIR)/, DmtpcPhysicsCint.o RecoilKinematics.o \
	                                Astro.o WimpSpectrum.o\
																 	ENDF_Fission.o ENDF_CrossSection.o ENDF_AngularDistribution.o \
																	SRIMTable.o Thermo.o)

INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


all: shared

shared: $(LIBDIR)/libDmtpcPhysics.so

$(LIBDIR)/libDmtpcPhysics.so: $(OBJS) | $(LIBDIR) 
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -g -shared -o $@

$(OBJS): | $(BUILDDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/DmtpcPhysicsCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	@rootcint -f $(BUILDDIR)/DmtpcPhysicsCint.cc -c $(INCLUDES) LinkDef.h

$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR) 
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 

clean:
	rm build/*.o
	rm build/*.cc
	rm build/*.h
	rm lib/*.so



