CC=gcc
CFLAGS=-I.
DEPS = bits.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ul: ul.c 
	arm-linux-gcc -o ul ul.c -Wall -O2 -I.
clean:
	rm ul
