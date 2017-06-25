CXX=g++

LDFLAGS=`root-config --libs`  -L../lib -lMCAmp  -lGmsh -L$(GARFIELD_HOME)/Library -lGarfield `dmtpc-config --lib-waveform` `dmtpc-config --lib-retrim` -lGeom
CXXFLAGS= `root-config --cflags` -fPIC -I../include -g -Os -I$(GARFIELD_HOME)/Include `dmtpc-config --inc-retrim`


VPATH= src

LIBDIR=lib
BUILDDIR=build
BINDIR=bin

OBJS := $(addprefix $(BINDIR)/, testModel testCompoundModel testMesh driftCageSimple \
					tripleMesh driftCageSimpler wireFieldCage fieldCage tripleMeshDoubleStage \
					driftCageTrack driftCageSimpleIntersecting driftCageTrackSimpleMesh driftCageTRIMTrack\
					fieldCageSimple)

all: $(OBJS) 


$(LIBDIR): 
	mkdir -p $(LIBDIR)


$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BUILDDIR)/%.o: %.cc | $(BUILDDIR)
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -o $@ -c $< 


$(BINDIR)/%: $(BUILDDIR)/%.o | $(BINDIR)
	@echo Creating  $@ 
	@$(CXX) $(LDFLAGS) -o $@ $< 



clean: 
	rm -f $(BUILDDIR)/*.o
	rm -f $(BINDIR)/* 
