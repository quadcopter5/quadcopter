#
# Makefile for Raspberry Pi tests
#

CC = g++
CFLAGS = -I../include
DEBUGFLAGS = -g -D_DEBUG
LDFLAGS = -L../lib -lperipherals_d ../obj/exception_d.o

BINDIR = bin

.PHONY: all dirs tests

all: dirs tests


dirs:
	mkdir -p $(BINDIR)

tests: ../lib/libperipherals_d.a $(BINDIR)/test_gpio.x $(BINDIR)/test_uart.x \
		$(BINDIR)/test_queuebuffer.x $(BINDIR)/test_i2c.x \
		$(BINDIR)/test_uart_read.x $(BINDIR)/test_endian.x \
		$(BINDIR)/test_pwm.x $(BINDIR)/test_accelerometer.x \
		$(BINDIR)/test_motor.x
	make -C .. debug

# Tests

$(BINDIR)/test_gpio.x: test_gpio.c ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_gpio.c $(LDFLAGS) -o $@

$(BINDIR)/test_uart.x: test_uart.c ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_uart.c $(LDFLAGS) -o $@

$(BINDIR)/test_queuebuffer.x: test_queuebuffer.c ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_queuebuffer.c $(LDFLAGS) -o $@

$(BINDIR)/test_i2c.x: test_i2c.cpp ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_i2c.cpp $(LDFLAGS) -o $@

$(BINDIR)/test_uart_read.x: test_uart_read.c ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_uart_read.c $(LDFLAGS) -o $@

$(BINDIR)/test_endian.x: test_endian.c ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_endian.c ../obj/endianness_d.o -o $@

$(BINDIR)/test_pwm.x: test_pwm.cpp ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_pwm.cpp $(LDFLAGS) -o $@

$(BINDIR)/test_accelerometer.x: test_accelerometer.cpp ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_accelerometer.cpp $(LDFLAGS) -o $@

$(BINDIR)/test_motor.x: test_motor.cpp ../lib/libperipherals_d.a
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_motor.cpp $(LDFLAGS) -o $@


clean:
	rm -rf $(BINDIR) *.o *.x

