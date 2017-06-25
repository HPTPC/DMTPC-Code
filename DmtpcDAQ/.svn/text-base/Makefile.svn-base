include compile_config

CXX=g++
LDFLAGS=`root-config --libs` `dmtpc-config --lib-core` -Wl,-z,-defs
CXXFLAGS= -g `root-config --cflags` -fstack-protector-all -fPIC -O3 `dmtpc-config --inc-core` `dmtpc-config --inc-control`

#CXXFLAGS += -DBUFFER_USE_SEMAPHORES 

VPATH= src:include

LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
# 

INCLUDES := $(addprefix $(INCLUDEDIR)/, $(shell ls $(INCLUDEDIR)))
OBJS := $(addprefix $(BUILDDIR)/,tinyxml2.o   DAQConfigXml.o  DAQConfig.o Buffer.o Camera.o \
	                                CameraLoader.o CameraHandler.o\
																 	DAQ.o EventBuilder.o\
																 	ScopeHandler.o Stats.o\
																 	Writer.o TriggerHandler.o\
																	ScopeLoader.o Scope.o Camera_Fake.o\
																 	)

### set up here, but please configure in compile_config ### 

ENABLED_TEXT=
DISABLED_TEXT=

# [FLI PROLINE]
#
ifeq ($(enable_fli_proline),1)
	ENABLED_TEXT+= fli_proline
	CXXFLAGS += -D_CAMERA_FLI_PROLINE_
	OBJS += $(BUILDDIR)/Camera_FLIProline.o 
else
	DISABLED_TEXT+= fli_proline
endif


# [ANDOR IKON]
ifeq ($(enable_andor_ikon),1)
	ENABLED_TEXT+= andor_ikon
	CXXFLAGS += -D_CAMERA_ANDOR_IKON_ -I$(ANDOR_SDK)/include 
	LDFLAGS += -L$(ANDOR_SDK)/lib -landor
	OBJS += $(BUILDDIR)/Camera_AndorIkon.o 
else
	DISABLED_TEXT+= andor_ikon
endif

# [ALTA U6]
ifeq ($(enable_alta_u6),1)
	ENABLED_TEXT+= alta_u6
	CXXFLAGS += -D_CAMERA_ALTA_U6_
else
	DISABLED_TEXT+= alta_u6
endif

# [CAEN]
ifeq ($(enable_caen),1)
	ENABLED_TEXT+= caen
	CXXFLAGS += -I$(CAEN_SDK)/include -D_SCOPE_CAEN_
	LDFLAGS += -L$(CAEN_SDK)/lib -lCAENDigitizer
	OBJS += $(BUILDDIR)/Scope_CAEN.o 
else
	DISABLED_TEXT+= caen
endif

# [ALAZAR]
ifeq ($(enable_alazar),1)
	ENABLED_TEXT+= alazar
	CXXFLAGS += -D_ALAZAR
else
	DISABLED_TEXT+= alazar
endif


all: shared
	@echo Enabled: $(ENABLED_TEXT)
	@echo Disabled: $(DISABLED_TEXT) 

shared: $(LIBDIR)/libDmtpcDAQ.so

$(LIBDIR)/libDmtpcDAQ.so: $(OBJS) 
	@echo Building shared library $@
	@$(CXX) $(LDFLAGS) $(OBJS) -g -shared -o $@

$(OBJS): | $(BUILDDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: %.cc $(INCLUDES) compile_config | $(BUILDDIR) 
	@echo Compiling  $< 
	@$(CXX) $(CXXFLAGS) -I./include -o $@ -c $< 



clean:
	rm build/*.o
	rm lib/*.so


