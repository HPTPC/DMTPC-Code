#If cfitsio is not available, set ENABLE_FITS to 0 
ENABLE_FITS=1

CXX=g++
LDFLAGS=`root-config --libs` -lgsl
LDFLAGS+=-L$(addprefix $(DMTPC_SOFTWARE), /gsl-2.1/install/lib) -lgsl 
CXXFLAGS= `root-config --cflags` -fPIC -O3 -g -w
CXXFLAGS+=-I$(addprefix $(DMTPC_SOFTWARE), /gsl-2.1/install/include/)
LDFLAGS+= -Wl,-z,defs



VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin



OBJS := $(addprefix $(BUILDDIR)/, DmtpcImageCint.o Transforms.o Interpolation.o LensCorrection.o StitchInfo.o Processing.o Pixel.o Tools.o IO.o Calculations.o Palettes.o)
INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


ifeq ($(ENABLE_FITS),1)
	LDFLAGS += -L/scratch1/darkmatter/dmtpc/projects/DarkMatter/cfitsio -lcfitsio
else
	CXXFLAGS += -DDMTPC_NO_FITS
endif 

#all: shared

#shared: $(LIBDIR)/libDmtpcImage.so

$(LIBDIR)/libDmtpcImage.so: $(OBJS) | $(LIBDIR)
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -g -shared -o $@

$(OBJS): | $(BUILDDIR)


$(BUILDDIR): 
	mkdir -p $(BUILDDIR)


$(LIBDIR): 
	mkdir -p $(LIBDIR)


$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR) 
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 


$(BUILDDIR)/DmtpcImageCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	@rootcint -f $(BUILDDIR)/DmtpcImageCint.cc -c -p $(INCLUDES) LinkDef.h

clean:
	rm build/*.o
	rm build/*.cc
	rm build/*.h 
	rm lib/*.so



