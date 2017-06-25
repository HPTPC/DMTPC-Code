include ../compile_config

CXX=g++
LDFLAGS=`root-config --libs` `dmtpc-config --lib-core --lib-daq --lib-waveform` -lrt
CXXFLAGS=  -g `root-config --cflags` -fPIC -O3 `dmtpc-config --inc-daq --inc-core --inc-waveform` 

# [ANDOR IKON]
ifeq ($(enable_andor_ikon),1)
	LDFLAGS += -L$(ANDOR_SDK)/lib -landor
endif

# [FLI PROLINE]
ifeq ($(enable_fli_proline),1)
	LDFLAGS += -lfli  
else
endif




BINDIR=bin
VPATH=src

OBJS := $(addprefix $(BINDIR)/, SimpleDAQ CalcScopeBias)

all: $(OBJS)


$(BINDIR): 
	mkdir -p $(BINDIR)

bin/%: %.cc | $(BINDIR) 
	@echo Compiling $< 
	@$(CXX) $(CXXFLAGS) -o $@  $(LDFLAGS) $< 


clean:
	rm -rf bin


