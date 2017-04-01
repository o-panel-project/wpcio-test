CC=arm-linux-gnueabihf-gcc
AR=arm-linux-gnueabihf-ar
LD=arm-linux-gnueabihf-ld
CFLAGS= -Wall -Os
LDFLAGS= -lpthread
LIBS=

.PHONY: clean all

PROG = wpcio_test

all: $(PROG)

%: $.o
	$(CC) $(LDFLAGS) -o $@ $@.o $(LIBS)
#	[ -f $@ ] cp $@ /tftpboot/.

clean:
	-rm -f $(PROG) *.elf *.gdb *.o
