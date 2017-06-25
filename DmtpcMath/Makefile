CXX=g++
LDFLAGS=`root-config --libs` -Wl,-z,defs  -g -lMinuit2
CXXFLAGS= `root-config --cflags` -fPIC -O3 -g
CXXFLAGS+=-I${DMTPC_SOFTWARE}/gsl-2.1/install/include
LDFLAGS+=-L${DMTPC_SOFTWARE}/gsl-2.1/install/lib -lgsl
####CXXFLAGS += -DNAN_CHECKS

VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin



OBJS := $(addprefix $(BUILDDIR)/, DmtpcMathCint.o\
         MathConstants.o SpecialPoly.o Distribution.o\
				 GaussianDist.o PoissonDist.o CrystalBallDist.o\
				 LogNormalDist.o InverseGaussianDist.o Angular.o ConvolvedSegment.o Geometry.o\
         PrincipalComponentFinder.o LandGaus.o EnergyTest.o MultivariateTest.o ParametricVonMisesHTTest.o MultiNormalSampler.o )
INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


all: shared

shared: $(LIBDIR)/libDmtpcMath.so

$(LIBDIR)/libDmtpcMath.so: $(OBJS) | $(LIBDIR)
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -g -shared -o $@

$(OBJS): | $(BUILDDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/DmtpcMathCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	@rootcint -f $(BUILDDIR)/DmtpcMathCint.cc -c $(INCLUDES) LinkDef.h

$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR) 
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 

clean:
	rm build/*.o
	rm build/*.cc
	rm build/*.h
	rm lib/*.so



