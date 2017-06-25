CXX=g++
LDFLAGS=`root-config --libs` -Wl,-z,defs
CXXFLAGS= `root-config --cflags` -fPIC -g

VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include


OBJS := $(addprefix $(BUILDDIR)/, DmtpcCoreCint.o Config.o CameraConfig.o RunConfig.o Waveform.o Image.o\
																 	Event.o Dataset.o WritableDataset.o DatasetHeader.o\
																	Location.o DetectorConfig.o TriggerInfo.o ScopeInfo.o ScopeConfig.o WritableEvent.o\
																	CameraInfo.o CommonTools.o )
INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))

$(LIBDIR)/libDmtpcCore.so: $(OBJS) $(LIBDIR)
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -shared -o $@


$(OBJS): | $(BUILDDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)



$(LIBDIR): 
	mkdir -p $(LIBDIR)


$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR)
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 


$(BUILDDIR)/DmtpcCoreCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	@rootcint -f $(BUILDDIR)/DmtpcCoreCint.cc -c $(INCLUDES) LinkDef.h


clean: 
	rm build/*.o
	rm build/*.cc
	rm build/*.h
	rm lib/*.so
