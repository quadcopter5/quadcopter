#
# Makefile for the resident Quadcopter program
#

CC = g++
CFLAGS = -Iinclude
LDFLAGS = -Llib

RELEASEFLAGS = -O3
DEBUGFLAGS = -g -D_DEBUG

SRCDIR = src
INCLUDEDIR = include

OBJDIR = obj
BINDIR = bin
LIBDIR = lib


.PHONY: all release debug clean dirs

all: debug

release: dirs $(LIBDIR)/libperipherals.a $(BINDIR)/quadcopter.x

debug: dirs $(LIBDIR)/libperipherals_d.a $(BINDIR)/quadcopter_d.x

dirs: $(OBJDIR)/ $(BINDIR)/ $(LIBDIR)/
$(OBJDIR)/:
	mkdir -p $(OBJDIR)
$(LIBDIR)/:
	mkdir -p $(LIBDIR)
$(BINDIR)/:
	mkdir -p $(BINDIR)


# Main application

$(BINDIR)/quadcopter.x: $(OBJDIR)/quadcopter.o $(OBJDIR)/exception.o \
		$(LIBDIR)/libperipherals.a
	$(CC) $(OBJDIR)/quadcopter.o $(OBJDIR)/exception.o $(LDFLAGS) \
		-lperipherals -o $@

$(BINDIR)/quadcopter_d.x: $(OBJDIR)/quadcopter_d.o $(OBJDIR)/exception_d.o \
		$(LIBDIR)/libperipherals_d.a
	$(CC) $(OBJDIR)/quadcopter_d.o $(OBJDIR)/exception_d.o $(LDFLAGS) \
		-lperipherals_d -o $@


$(OBJDIR)/quadcopter.o: $(SRCDIR)/quadcopter.cpp
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/quadcopter.cpp -o $@

$(OBJDIR)/quadcopter_d.o: $(SRCDIR)/quadcopter.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/quadcopter.cpp -o $@


$(OBJDIR)/exception.o: $(SRCDIR)/exception.cpp
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/exception.cpp -o $@

$(OBJDIR)/exception_d.o: $(SRCDIR)/exception.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/exception.cpp -o $@


# Drivers

$(LIBDIR)/libperipherals.a: $(OBJDIR)/gpio.o $(OBJDIR)/uart.o \
		$(OBJDIR)/queuebuffer.o $(OBJDIR)/i2c.o $(OBJDIR)/endianness.o \
		$(OBJDIR)/pwm.o $(OBJDIR)/accelerometer.o
	ar rcs $(LIBDIR)/libperipherals.a $(OBJDIR)/gpio.o $(OBJDIR)/uart.o \
		$(OBJDIR)/queuebuffer.o $(OBJDIR)/i2c.o $(OBJDIR)/endianness.o \
		$(OBJDIR)/pwm.o $(OBJDIR)/accelerometer.o

$(LIBDIR)/libperipherals_d.a: $(OBJDIR)/gpio_d.o $(OBJDIR)/uart_d.o \
		$(OBJDIR)/queuebuffer_d.o $(OBJDIR)/i2c_d.o $(OBJDIR)/endianness_d.o \
		$(OBJDIR)/pwm_d.o $(OBJDIR)/accelerometer_d.o
	ar rcs $(LIBDIR)/libperipherals_d.a $(OBJDIR)/gpio_d.o $(OBJDIR)/uart_d.o \
		$(OBJDIR)/queuebuffer_d.o $(OBJDIR)/i2c_d.o $(OBJDIR)/endianness_d.o \
		$(OBJDIR)/pwm_d.o $(OBJDIR)/accelerometer_d.o


$(OBJDIR)/gpio.o: $(SRCDIR)/gpio.c $(INCLUDEDIR)/gpio.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/gpio.c -o $@

$(OBJDIR)/uart.o: $(SRCDIR)/uart.c $(INCLUDEDIR)/uart.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/uart.c -o $@

$(OBJDIR)/queuebuffer.o: $(SRCDIR)/queuebuffer.c $(INCLUDEDIR)/queuebuffer.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/queuebuffer.c -o $@

$(OBJDIR)/i2c.o: $(SRCDIR)/i2c.cpp $(INCLUDEDIR)/i2c.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/i2c.cpp -o $@

$(OBJDIR)/endianness.o: $(SRCDIR)/endianness.c $(INCLUDEDIR)/endianness.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/endianness.c -o $@

$(OBJDIR)/pwm.o: $(SRCDIR)/pwm.cpp $(INCLUDEDIR)/pwm.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/pwm.cpp -o $@

$(OBJDIR)/accelerometer.o: $(SRCDIR)/accelerometer.cpp \
		$(INCLUDEDIR)/accelerometer.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/accelerometer.cpp -o $@


$(OBJDIR)/gpio_d.o: $(SRCDIR)/gpio.c $(INCLUDEDIR)/gpio.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/gpio.c -o $@

$(OBJDIR)/uart_d.o: $(SRCDIR)/uart.c $(INCLUDEDIR)/uart.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/uart.c -o $@

$(OBJDIR)/queuebuffer_d.o: $(SRCDIR)/queuebuffer.c $(INCLUDEDIR)/queuebuffer.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/queuebuffer.c -o $@

$(OBJDIR)/i2c_d.o: $(SRCDIR)/i2c.cpp $(INCLUDEDIR)/i2c.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/i2c.cpp -o $@

$(OBJDIR)/endianness_d.o: $(SRCDIR)/endianness.c $(INCLUDEDIR)/endianness.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/endianness.c -o $@

$(OBJDIR)/pwm_d.o: $(SRCDIR)/pwm.cpp $(INCLUDEDIR)/pwm.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/pwm.cpp -o $@

$(OBJDIR)/accelerometer_d.o: $(SRCDIR)/accelerometer.cpp \
		$(INCLUDEDIR)/accelerometer.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/accelerometer.cpp -o $@


clean:
	rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)

