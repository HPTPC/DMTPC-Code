#If cfitsio is not available, set ENABLE_FITS to 0 
ENABLE_FITS=1

#Hacks for compilation
GSLINCLUDE?=/scratch1/darkmatter/dmtpc/software/gsl-2.1/install/include
GSLLIB?=/scratch1/darkmatter/dmtpc/software/gsl-2.1/install/include
CFITSIOINCLUDE?=/scratch1/darkmatter/dmtpc/projects/DarkMatter/cfitsio
CGITSIOLIB?=/scratch1/darkmatter/dmtpc/projects/DarkMatter/cfitsio

CXX=g++
LDFLAGS=`root-config --libs` -lgsl
LDFLAGS+=-L$(GSLLIB) -lgsl 
CXXFLAGS= `root-config --cflags` -fPIC -O3 -g -w
CXXFLAGS+=-I$(GSLINCLUDE)

LDFLAGS+= -Wl,-z,defs



VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin



OBJS := $(addprefix $(BUILDDIR)/, DmtpcImageCint.o Transforms.o Interpolation.o LensCorrection.o StitchInfo.o Processing.o Pixel.o Tools.o IO.o Calculations.o Palettes.o)
INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


ifeq ($(ENABLE_FITS),1)
	LDFLAGS+=-L$(CFITSIOLIB) -lcfitsio
	CXXFLAGS+=-I$(CFITSIOINCLUDE)
else
	CXXFLAGS += -DDMTPC_NO_FITS
endif 

#all: shared

#shared: $(LIBDIR)/libDmtpcImage.so

$(LIBDIR)/libDmtpcImage.so: $(OBJS) | $(LIBDIR)
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -Wl,--enable-new-dtags,-rpath,"$(GSLLIB) $(CFITSIOLIB)" -g -shared -o $@

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



