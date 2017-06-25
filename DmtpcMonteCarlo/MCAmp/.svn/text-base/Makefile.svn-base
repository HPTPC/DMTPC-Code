CXX=g++
LDFLAGS=`root-config --libs` -lGmsh -L$(GARFIELD_HOME)/Library -lGarfield `dmtpc-config --lib-waveform` `dmtpc-config --lib-retrim`  -lGeom
CXXFLAGS= `root-config --cflags` -fPIC -g -Os -I$(GARFIELD_HOME)/Include `dmtpc-config --inc-waveform` `dmtpc-config --inc-retrim`


VPATH= src:include:build

LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin
#FLTO=-flto

OBJS := $(addprefix $(BUILDDIR)/, Shape.o Model.o SolverElmer.o Gas.o Avalanche.o Amplifier.o Representation.o MCAmpCint.o)
EXE := $(addprefix $(BINDIR)/, makeGas)

INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


all: shared $(EXE)

shared: $(LIBDIR)/libMCAmp.so

$(LIBDIR)/libMCAmp.so: $(OBJS) | $(LIBDIR) 
	@echo Building shared library $@
	@$(CXX) $(FLTO) $(LDFLAGS) $(OBJS) -shared -o $@

$(OBJS): | $(BUILDDIR) 

$(LIBDIR): 
	mkdir -p $(LIBDIR)

$(BINDIR): 
	mkdir -p $(BINDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: %.cc $(INCLUDES) | $(BUILDDIR) 
	@echo Compiling  $< 
	@$(CXX) $(FLTO) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 

$(BINDIR)/%: %.cc $(INCLUDES) $(LIBDIR)/libMCAmp.so | $(BINDIR) 
	@echo Compiling  $< 
	@$(CXX) $(FLTO) $(CXXFLAGS) -I./include -o $@ -L$(LIBDIR) -lMCAmp $(LDFLAGS) $< 

$(BUILDDIR)/MCAmpCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	@rootcint  -f $(BUILDDIR)/MCAmpCint.cc -c $(CXXFLAGS) $(INCLUDES) LinkDef.h 





clean: 
	rm build/*.o
	rm lib/*.so 
	rm build/*.cc
	rm build/*.h
