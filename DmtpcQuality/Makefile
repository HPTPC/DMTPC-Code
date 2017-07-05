CXX=g++
LDFLAGS=`root-config --libs`
CXXFLAGS= `root-config --cflags` -fPIC -O0 -g  -Wall -Wextra -w
CXXFLAGS +=`$(DMTPC_HOME)/DmtpcTools/dmtpc-config/dmtpc-config --inc-core`
#dbrailsf 05/07/17  Hack in a sqlite3 location if specified in an environment variable
ifneq ($(SQLITE3INCLUDE),)
	CXXFLAGS +=-I$(SQLITE3INCLUDE)
endif
#for uin32_t on m3daq, comment out elsewhere:
#CXXFLAGS += -std=c++0x
LDFLAGS +=`$(DMTPC_HOME)/DmtpcTools/dmtpc-config/dmtpc-config --lib-core`
#dbrailsf 05/07/17  Hack in a sqlite3 location if specified in an environment variable
ifneq ($(SQLITE3LIB),)
	LDFLAGS +=-L$(SQLITE3LIB) -lsqlite3
else
	LDFLAGS += -L/usr/lib64 -lsqlite3
endif
LDFLAGS += -L/usr/lib64/mysql -lmysqlclient

VPATH=src:include:build
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin
DOCDIR=doc

OBJS := $(addprefix $(BUILDDIR)/, MySQL.o SQLite.o RunQuality.o DmtpcQualityCint.o)
BINARIES := $(addprefix $(BINDIR)/, extractDatabaseV4 runCheckerV2 oneRunSparkList )
INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))
#DOCS := $(addprefix $(DOCDIR)/, Readme.tex)

all: $(LIBDIR)/libDmtpcQuality.so $(BINARIES) 

$(LIBDIR)/libDmtpcQuality.so: $(OBJS) $(LIBDIR)
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -Wl,--enable-new-dtags,-rpath,"$(SQLITE3LIB)" -shared -o $@


$(OBJS): | $(BUILDDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BINDIR): 
	mkdir -p $(BINDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)


$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR)
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ -c $< 

$(BINDIR)/%: %.cc $(INCLUDES) Makefile | $(BINDIR)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -I./include -I./ -o $@ $(LDFLAGS) -L./$(LIBDIR) -lDmtpcQuality  $< 

#$(DOCDIR)/%.tex: %.tex Makefile | $(DOCDIR)
#	@echo Compiling $<
#	pdflatex $@

$(BUILDDIR)/DmtpcQualityCint.cc: $(INCLUDES) LinkDef.h | $(BUILDDIR)
	@echo Running rootcint
	rootcint  -f $(BUILDDIR)/DmtpcQualityCint.cc -c -p $(CXXFLAGS) include/RunQuality.hh include/MySQL.hh include/SQLite.hh LinkDef.h


clean: 
	rm -f build/*.o
	rm -f build/*.h
	rm -f build/*.cc
	rm -f bin/*
	rm -f lib/*
