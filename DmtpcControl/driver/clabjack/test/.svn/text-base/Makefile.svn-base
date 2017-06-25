CC=gcc
CFLAGS=-g -O2
LDFLAGS=-llabjackusb -L../lib -lclabjack


OBJS:= test_feedback

all: $(OBJS) 

%:%.c 
	$(CC) $(CFLAGS) -I../include $(LDFLAGS) -o $@ $< 

clean: 
	rm -f $(OBJS) 







