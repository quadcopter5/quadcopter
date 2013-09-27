#
# Makefile for the resident Quadcopter program
#

COMMON = ../common

SRCDIR = src
INCLUDEDIR = include

OBJDIR = obj
BINDIR = bin
LIBDIR = lib

CC = g++
CFLAGS = -Iinclude -I$(COMMON)/include
LDFLAGS = -Llib

RELEASEFLAGS = -O3
DEBUGFLAGS = -g -D_DEBUG


.PHONY: all release debug clean dirs

all: debug

release: dirs $(LIBDIR)/libquadcopter.a $(LIBDIR)/libcommon.a \
		$(BINDIR)/quadcopter.x

debug: dirs $(LIBDIR)/libquadcopter_d.a $(LIBDIR)/libcommon_d.a \
		$(BINDIR)/quadcopter_d.x

dirs: $(OBJDIR)/ $(OBJDIR)/common/ $(BINDIR)/ $(LIBDIR)/
$(OBJDIR)/:
	mkdir -p $(OBJDIR)
$(OBJDIR)/common/:
	mkdir -p $(OBJDIR)/common/
$(LIBDIR)/:
	mkdir -p $(LIBDIR)
$(BINDIR)/:
	mkdir -p $(BINDIR)


#
# Main application
#

$(BINDIR)/quadcopter.x: $(OBJDIR)/quadcopter.o $(OBJDIR)/common/exception.o \
		$(LIBDIR)/libquadcopter.a
	$(CC) $(OBJDIR)/quadcopter.o $(OBJDIR)/common/exception.o $(LDFLAGS) \
		-lquadcopter -o $@

$(BINDIR)/quadcopter_d.x: $(OBJDIR)/quadcopter_d.o \
		$(OBJDIR)/common/exception_d.o $(LIBDIR)/libquadcopter_d.a
	$(CC) $(OBJDIR)/quadcopter_d.o $(OBJDIR)/common/exception_d.o $(LDFLAGS) \
		-lquadcopter_d -o $@


$(OBJDIR)/quadcopter.o: $(SRCDIR)/quadcopter.cpp
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/quadcopter.cpp -o $@

$(OBJDIR)/quadcopter_d.o: $(SRCDIR)/quadcopter.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/quadcopter.cpp -o $@


#
# Common
# (shared portion between quadcopter and remote)
#

COMMON_NAMES = exception endianness packetmotion radioconnection

$(LIBDIR)/libcommon.a: \
		$(foreach name,$(COMMON_NAMES),$(OBJDIR)/common/$(name).o)
	ar rcs $@ $^
$(LIBDIR)/libcommon_d.a: \
		$(foreach name,$(COMMON_NAMES),$(OBJDIR)/common/$(name)_d.o)
	ar rcs $@ $^

define COMMON_TEMPLATE =
$$(OBJDIR)/common/$(1).o: $$(COMMON)/$$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(RELEASEFLAGS) -c $$^ -o $$@
$$(OBJDIR)/common/$(1)_d.o: $$(COMMON)/$$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) -c $$^ -o $$@
endef

$(foreach name,$(COMMON_NAMES),$(eval $(call COMMON_TEMPLATE,$(name))))


#
# Quadcopter-specific objects
#

$(LIBDIR)/libquadcopter.a: $(OBJDIR)/gpio.o $(OBJDIR)/radiouart.o \
		$(OBJDIR)/queuebuffer.o $(OBJDIR)/i2c.o $(OBJDIR)/pwm.o \
		$(OBJDIR)/accelerometer.o $(OBJDIR)/motor.o
	ar rcs $@ $^

$(LIBDIR)/libquadcopter_d.a: $(OBJDIR)/gpio_d.o $(OBJDIR)/radiouart_d.o \
		$(OBJDIR)/queuebuffer_d.o $(OBJDIR)/i2c_d.o $(OBJDIR)/pwm_d.o \
		$(OBJDIR)/accelerometer_d.o $(OBJDIR)/motor_d.o
	ar rcs $@ $^

$(OBJDIR)/gpio.o: $(SRCDIR)/gpio.c $(INCLUDEDIR)/gpio.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/gpio.c -o $@
$(OBJDIR)/gpio_d.o: $(SRCDIR)/gpio.c $(INCLUDEDIR)/gpio.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/gpio.c -o $@

$(OBJDIR)/radiouart.o: $(SRCDIR)/radiouart.cpp $(INCLUDEDIR)/radiouart.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/radiouart.cpp -o $@
$(OBJDIR)/radiouart_d.o: $(SRCDIR)/radiouart.cpp $(INCLUDEDIR)/radiouart.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/radiouart.cpp -o $@

$(OBJDIR)/queuebuffer.o: $(SRCDIR)/queuebuffer.c $(INCLUDEDIR)/queuebuffer.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/queuebuffer.c -o $@
$(OBJDIR)/queuebuffer_d.o: $(SRCDIR)/queuebuffer.c $(INCLUDEDIR)/queuebuffer.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/queuebuffer.c -o $@

$(OBJDIR)/i2c.o: $(SRCDIR)/i2c.cpp $(INCLUDEDIR)/i2c.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/i2c.cpp -o $@
$(OBJDIR)/i2c_d.o: $(SRCDIR)/i2c.cpp $(INCLUDEDIR)/i2c.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/i2c.cpp -o $@

$(OBJDIR)/pwm.o: $(SRCDIR)/pwm.cpp $(INCLUDEDIR)/pwm.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/pwm.cpp -o $@
$(OBJDIR)/pwm_d.o: $(SRCDIR)/pwm.cpp $(INCLUDEDIR)/pwm.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/pwm.cpp -o $@

$(OBJDIR)/accelerometer.o: $(SRCDIR)/accelerometer.cpp \
		$(INCLUDEDIR)/accelerometer.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/accelerometer.cpp -o $@
$(OBJDIR)/accelerometer_d.o: $(SRCDIR)/accelerometer.cpp \
		$(INCLUDEDIR)/accelerometer.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/accelerometer.cpp -o $@

$(OBJDIR)/motor.o: $(SRCDIR)/motor.cpp $(INCLUDEDIR)/motor.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/motor.cpp -o $@
$(OBJDIR)/motor_d.o: $(SRCDIR)/motor.cpp $(INCLUDEDIR)/motor.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/motor.cpp -o $@


clean:
	rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)

