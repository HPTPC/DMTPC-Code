CXX=g++
LDFLAGS=`root-config --libs` -lMathMore -lMinuit2 `dmtpc-config --lib-core --lib-image --lib-waveform --lib-math --lib-physics` -Wl,-z,defs -lSpectrum -g	
#for sparkdb
LDFLAGS += `dmtpc-config --lib-quality`
LDFLAGS += -L/usr/lib64 -lsqlite3
#-lgsl -lgslcblas -g
CXXFLAGS= `root-config --cflags` -fPIC  -g `dmtpc-config --inc-core --inc-image --inc-waveform --inc-math --inc-physics`  -Wall -Wextra  
#CXXFLAGS += -O3
#for sparkdb
CXXFLAGS += `dmtpc-config --inc-quality`
#added
LDFLAGS += -L/scratch1/darkmatter/dmtpc/software/fftw3-dev/install -lfftw3
#LDFLAGS+=-L${DMTPC_SOFTWARE}/gsl-2.1/install/lib -lgsl -lgslcblas -g
CXXFLAGS+=-I${DMTPC_SOFTWARE}/fftw3-dev/install/include/
#CXXFLAGS+=-I${DMTPC_SOFTWARE}/gsl-2.1/install/include/
#LDFLAGS += -L/usr/lib64 -lgsl -lgslcblas
#CXXFLAGS+=/usr/

VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin


OBJS := $(addprefix $(BUILDDIR)/, ClusterImage.o GainMap.o Playlist.o AnalysisConfig.o\
																	ClusterFinding.o ClusterAlgo.o CleanSkimConfig.o CleanSkimFunctions.o Stitcher.o SkimEvent.o TrackFitter.o SkimDataset.o CleanSkimImpl.o make_peak_hist.o MCTruth.o MCTPCDataset.o MCTruthAlgo.o HVLog.o GainTree.o DmtpcAnalysisCint.o )

BINARIES := $(addprefix $(BINDIR)/, cleanSkim MCTPCFitter Summer SimpleAlphaGain FitEnergyTest FitEnergyTestParameterizedErrors FitVonMisesHTTest PlaylistFitter makeSparkFriendCanary make_peak_hist GainAnalysis);

INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


all: $(LIBDIR)/libDmtpcAnalysis.so $(BINARIES) 

$(LIBDIR)/libDmtpcAnalysis.so: $(OBJS) $(LIBDIR)
	@echo Building shared library $@
	$(CXX) $(LDFLAGS) $(OBJS) -shared -o $@


$(OBJS): | $(BUILDDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BINDIR): 
	mkdir -p $(BINDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)


$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR)
	@echo Compiling  $< 
	$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 

$(BINDIR)/%: %.cc $(INCLUDES) Makefile | $(BINDIR)
	@echo Compiling $<
	$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ $(LDFLAGS) -L./$(LIBDIR) -lDmtpcAnalysis  $< 

$(BUILDDIR)/DmtpcAnalysisCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	rootcint  -f $(BUILDDIR)/DmtpcAnalysisCint.cc -c $(CXXFLAGS) $(INCLUDES) LinkDef.h


clean: 
	rm -f build/*
	rm -f bin/*
	rm -f lib/*
