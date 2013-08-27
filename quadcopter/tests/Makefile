#
# Makefile for Raspberry Pi tests
#

CC = g++
CFLAGS = -I../include
DEBUGFLAGS = -g -D_DEBUG
LDFLAGS = -L../lib -lperipherals_d ../obj/exception_d.o

BINDIR = bin


all: dirs tests


dirs:
	mkdir -p $(BINDIR)

tests: ../lib/libperipherals_d.a $(BINDIR)/test_gpio.x $(BINDIR)/test_uart.x \
		$(BINDIR)/test_queuebuffer.x $(BINDIR)/test_i2c.x \
		$(BINDIR)/test_uart_read.x $(BINDIR)/test_endian.x


# Tests

$(BINDIR)/test_gpio.x: test_gpio.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_gpio.c $(LDFLAGS) \
			-o $(BINDIR)/test_gpio.x

$(BINDIR)/test_uart.x: test_uart.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_uart.c $(LDFLAGS) \
			-o $(BINDIR)/test_uart.x

$(BINDIR)/test_queuebuffer.x: test_queuebuffer.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_queuebuffer.c $(LDFLAGS) \
			-o $(BINDIR)/test_queuebuffer.x

$(BINDIR)/test_i2c.x: test_i2c.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_i2c.cpp $(LDFLAGS) \
			-o $(BINDIR)/test_i2c.x

$(BINDIR)/test_uart_read.x: test_uart_read.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_uart_read.c $(LDFLAGS) \
			-o $(BINDIR)/test_uart_read.x

$(BINDIR)/test_endian.x: test_endian.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_endian.c ../obj/endianness_d.o \
			-o $(BINDIR)/test_endian.x


clean:
	rm -rf $(BINDIR) *.o *.x

