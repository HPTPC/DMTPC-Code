#add tests to the line below
TESTS= testTrivial testConfig testWaveform testBuffer testDataset

#add recompilation triggers here
DEPENDENCIES=../DmtpcCore/lib/libDmtpcCore.so ../DmtpcDAQ/lib/libDmtpcDAQ.so

#printout trigger
PRINTOUT="^REASON:"
INDENT = "....."

CC=g++
CFLAGS=-O3 -g `root-config --cflags` -Wall `dmtpc-config --inc-core --inc-daq --inc-physics` 
LFLAGS=`root-config --libs` `dmtpc-config --lib-core --lib-daq --lib-physics` -lrt


all: get_ready $(TESTS)
	@echo ------------------------------------------
	@echo `grep PASSED results | wc -l` / `cat results | wc -l ` tests passed

get_ready:
	@rm -f results
	@mkdir -p output
	@mkdir -p times
	@mkdir -p bin
	@echo ------------------------------------------

clean: 
	@find src -type f -exec touch {} \; 
	@rm -f results
	@rm -rf output 
	@rm -rf times
	@rm -rf bin 

bin/%: src/%.cc $(DEPENDENCIES)
	@echo "Compiling $*"
	@rm -f $@
	-@$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

	
$(TESTS): % :bin/% 
	@echo -n $(INDENT)
	@echo -n $@ | tee -a results 
	@if  (/usr/bin/time -f '%e s %M kb' -o times/$@ $? `cat args/$@ 2>/dev/null` > output/$@.out 2>&1); \
		then echo -n -e "\tPASSED\t " | tee -a results; \
	  echo \(`tail -n 1 times/$@`\) | tee -a results; \
		else echo -n -e "\tFAILED\t " | tee -a results; \
	  echo \(`tail -n 1 times/$@`\) | tee -a results; \
		cat output/$@.out | grep $(PRINTOUT) | sed "s/^/$(INDENT)/";\
	 	fi

