CC=gcc
CFLAGS=-g -O2 -fPIC
LDFLAGS=-llabjackusb 

#uncomment to enable debug output
# CFLAGS += -DDEBUG

VPATH=src:include:build
LIBDIR=lib
INCLUDEDIR=include
BINDIR=bin

BUILDDIR := build
OBJS := $(addprefix $(BUILDDIR)/, common.o feedback.o u3.o)

INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))

shared: $(LIBDIR)/libclabjack.so

$(LIBDIR)/libclabjack.so: $(OBJS) | $(LIBDIR) 
	$(CC) $(LDFLAGS) $(OBJS) -shared -o $@

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)

$(OBJS): | $(BUILDDIR) 

$(BUILDDIR)/%.o: %.c $(INCLUDES) Makefile 
	@echo Compiling  $< 
	$(CC) $(CFLAGS) -I./$(INCLUDEDIR)  -o $@ -c $< 



clean: 
	rm -r build
	rm -r lib


