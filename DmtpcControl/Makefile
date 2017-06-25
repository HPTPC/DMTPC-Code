CXX=g++
LDFLAGS=`root-config --libs` -Wl,-z,defs `curl-config --libs` `dmtpc-config --lib-core`
CXXFLAGS= `root-config --cflags` -fstack-protector-all -fPIC -g `curl-config --cflags` `dmtpc-config --inc-core`

VPATH=src:include:build
EXEDIR=bin
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include

LDFLAGS  += -L/usr/lib64/mysql -lmysqlclient


### destination for httpd stuff
SCRIPTS_DIR = /var/www/cgi-bin/
HTML_DIR = /var/www/html/


OBJS := $(addprefix $(BUILDDIR)/, MySQLStore.o SerialUtility.o InficonController.o SynAccessNetBooter.o Channel.o ChannelConfig.o CaenUtility.o CaenSupply.o DMSlow.o)

INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))


all: shared slow


shared: $(LIBDIR)/libDmtpcControl.so


$(LIBDIR)/libDmtpcControl.so: $(OBJS) $(LIBDIR)
	@echo Building shared library $@
	@$(CXX) -Wall $(LDFLAGS) $(OBJS) -shared -o $@


$(OBJS): | $(BUILDDIR)

$(EXEDIR):
	mkdir -p $(EXEDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)


$(LIBDIR): 
	mkdir -p $(LIBDIR)


$(BUILDDIR)/%.o: %.cc $(INCLUDES) Makefile | $(BUILDDIR)
	@echo Compiling  $< 
	@$(CXX) -Wall $(CXXFLAGS) -I./include -I./ -o $@ -c $< 


clean: 
	rm -rf build
	rm -rf lib
	rm -rf bin

slow: bin/DMSlow 

bin/DMSlow: build/DMSlow.o | $(EXEDIR)
	$(CXX) -g $(LDFLAGS) -Llib -lDmtpcControl -o bin/DMSlow

deploy_web: 
	cp -r scripts/* $(SCRIPTS_DIR)
	cp -r html/* $(HTML_DIR)
	cp bin/DMSlow $(SCRIPTS_DIR)
	cp lib/libDmtpcControl.so /usr/local/lib/.
	cp ../DmtpcCore/lib/libDmtpcCore.so /usr/local/lib/.

