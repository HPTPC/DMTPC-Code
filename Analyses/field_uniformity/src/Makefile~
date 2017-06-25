# Glen Cowan, RHUL Physics, November 2007.
# Modified by G. Druitt 

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTLIBS     := $(shell root-config --nonew --libs)
CXXFLAGS     += $(ROOTCFLAGS)
LIBS         += $(ROOTLIBS)
#  Not sure why Minuit isn't being included -- put in by hand
#
LIBS         += -lMinuit
#LIBS += -L/usr/lib64 -lgsl
LIBS += -L$(addprefix $(DMTPC_SOFTWARE), /gsl-2.1/install/lib) -lgsl
LDFLAGS       = -O

#CXX           = g++
CXXFLAGS      += -g -Wall

DARKDIR=/scratch2/gmuraru/dmtpc_software/

#MaxCam
INCLUDES     := -I$(DARKDIR)DmtpcCore/include
INCLUDES	+= -I$(DARKDIR)DmtpcImage/include
INCLUDES	+= -I$(DARKDIR)DmtpcAnalysis/include
INCLUDES	+= -I$(DARKDIR)DmtpcWaveform/include

LIBS         += -L$(DARKDIR)DmtpcCore/lib -lDmtpcCore
LIBS	+= -L$(DARKDIR)DmtpcImage/lib -lDmtpcImage
LIBS	+= -L$(DARKDIR)DmtpcAnalysis/lib -lDmtpcAnalysis
LIBS	+= -L$(DARKDIR)DmtpcWaveform/lib -lDmtpcWaveform
#LIBS         += -L//waveformtools/lib -lWaveformTools

#INCLUDES += -I/Users/richardeggleston/dmtpc/projects/DarkMatter/MaxCam

#example - change for your program
sum_img_bias: sum_img_bias.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

field_uniformity: field_uniformity.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

field_uniformity_skimmed: field_uniformity_skimmed.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

field_uniformity_skimmed_noclust: field_uniformity_skimmed_noclust.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

field_uniformity_skimmed_noclust1: field_uniformity_skimmed_noclust1.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

plot_pedestal: plot_pedestal.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

plot_pix_intensity_skim: plot_pix_intensity_skim.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

%.o: %.cc
	$(CXX) $(INCLUDES) ${CXXFLAGS} -c $?

##### Library / class makes #####
SparkDict: Spark_Info.hh LinkDef.hh
	rootcint -f SparkDict.cc -c $?

libSpark_Info: Spark_Info.cc SparkDict.cc
	$(CXX) -fPIC -shared `root-config --cflags` $? -o libSpark_Info.so

sparkclass: SparkDict libSpark_Info

all: 

test:
	@echo $(ROOTCFLAGS)

clean:	
	-rm -f getEnergies.o
