# Glen Cowan, RHUL Physics, November 2007.
# Modified by R. Eggleston 

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTLIBS     := $(shell root-config --nonew --libs)
CXXFLAGS     += $(ROOTCFLAGS)
LIBS         += $(ROOTLIBS)
#  Not sure why Minuit isn't being included -- put in by hand
#
LIBS         += -lMinuit -lgslcblas
LDFLAGS       = -O

#CXX           = g++
CXXFLAGS      += -g -Wall

DARKDIR=$(DMTPC_HOME)/

#MaxCam
#INCLUDES     := -I$(DARKDIR)DmtpcCore/include
#INCLUDES	+= -I$(DARKDIR)DmtpcImage/include
#INCLUDES 	+= -I$(DARKDIR)DmtpcAnalysis/include

INCLUDES	:= `dmtpc-config --inc-analysis --inc-waveform --inc-core --inc-image`
LIBS	:= `dmtpc-config --lib-analysis --lib-waveform --lib-core --lib-image`

#LIBS         += -L$(DARKDIR)DmtpcCore/lib -lDmtpcCore
#LIBS	+= -L$(DARKDIR)DmtpcImage/lib -lDmtpcImage
#LIBS	+= -L$(DARKDIR)DmtpcAnalysis/lib -lDmtpcAnalysis
#LIBS         += -L//waveformtools/lib -lWaveformTools

#INCLUDES += -I/Users/richardeggleston/dmtpc/projects/DarkMatter/MaxCam

#example - change for your program
get_bias_frame: get_bias_frame.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

save_bias_frames: save_bias_frames.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)


field_uniformity: field_uniformity.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

preskim_bias_self_subtract: preskim_bias_self_subtract.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

plot_bframe_pixint: plot_bframe_pixint.o
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	mv $@ ../bin/

getSkimBframes: getSkimBframes.o
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
