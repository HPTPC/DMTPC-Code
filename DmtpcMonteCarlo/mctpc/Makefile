CXX=g++
LDFLAGS=`root-config --glibs` `dmtpc-config --lib-retrim --lib-core --lib-physics  --lib-wimpspectrum --lib-math --lib-analysis` -L${GARFIELD_HOME}/Library -lGarfield -lgfortran -lgsl -lgslcblas -Wl,-z,defs -lGeom -lgfortran
CXXFLAGS= `root-config --cflags` -fPIC -O3 -g  `dmtpc-config --inc-retrim --inc-core  --inc-physics --inc-wimpspectrum --inc-math --inc-analysis` -I${GARFIELD_HOME}/Include  -Wall -Werror

VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
MODELSDIR=models
BINDIR=bin


OBJS := $(addprefix $(BUILDDIR)/, SimpleAvalanche.o SimpleCamera.o SimpleDrift.o ParticleDistribution.o VectorDistribution.o ScalarDistribution.o  \
																	TRIMGenerator.o Driver.o InteractiveReadout.o FileReadout.o DatasetReadout.o MultiTRIMGenerator.o\
																 	ToyGenerator.o HEEDGenerator.o SampledMCAmpAvalanche.o mctpcCint.o SimpleVUVAvalanche.o \
																	MCAmpAvalancheCache.o CachedMCAmpAvalanche.o IonizingPhotonTable.o) 

MODELS := $(addprefix $(BINDIR)/, 4sh 4shSimple 4shWimp 4shAmBe 4shAngledAlpha 4shAmBeSimple distributionTest unfold angled_alpha_distribution m3WimpFixed m3WimpBG canary_double_amp hideAmbeSimple wimpDistribution m3iso canary_gamma); 

INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


all: shared models


models: shared $(MODELS) 

shared: $(LIBDIR)/libmctpc.so

$(LIBDIR)/libmctpc.so: $(OBJS) | $(LIBDIR) 
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -g -shared -o $@

$(OBJS): | $(BUILDDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BINDIR): 
	mkdir -p $(BINDIR)

$(BUILDDIR)/mctpcCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	@rootcint -f $(BUILDDIR)/mctpcCint.cc -c -p $(CXXFLAGS) $(INCLUDES)  LinkDef.h 

$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR) 
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 

$(BINDIR)/%: models/%.cc $(INCLUDES) Makefile $(LIBDIR)/libmctpc.so | $(BINDIR) 
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -o $@ $(LDFLAGS) -Llib -lmctpc $< 


clean:
	rm -rf build
	rm -rf lib
	rm -rf bin



