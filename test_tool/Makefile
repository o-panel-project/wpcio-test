CC=arm-none-linux-gnueabi-gcc
AR=arm-none-linux-gnueabi-ar
LD=arm-none-linux-gnueabi-ld
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
