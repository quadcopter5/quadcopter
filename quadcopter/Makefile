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

$(BINDIR)/quadcopter.x: $(OBJDIR)/quadcopter.o $(LIBDIR)/libcommon.a \
		$(LIBDIR)/libquadcopter.a
	$(CC) $(OBJDIR)/quadcopter.o $(LDFLAGS) -lquadcopter -lcommon -o $@

$(BINDIR)/quadcopter_d.x: $(OBJDIR)/quadcopter_d.o $(LIBDIR)/libcommon_d.a \
		$(LIBDIR)/libquadcopter_d.a
	$(CC) $(OBJDIR)/quadcopter_d.o $(LDFLAGS) -lquadcopter_d -lcommon_d -o $@


$(OBJDIR)/quadcopter.o: $(SRCDIR)/quadcopter.cpp
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/quadcopter.cpp -o $@

$(OBJDIR)/quadcopter_d.o: $(SRCDIR)/quadcopter.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/quadcopter.cpp -o $@


#
# Common
# (shared portion between quadcopter and remote)
#

COMMON_NAMES = exception endianness radioconnection packetmotion \
		packetdiagnostic

$(LIBDIR)/libcommon.a: \
		$(foreach name,$(COMMON_NAMES),$(OBJDIR)/common/$(name).o)
	ar rcs $@ $^
$(LIBDIR)/libcommon_d.a: \
		$(foreach name,$(COMMON_NAMES),$(OBJDIR)/common/$(name)_d.o)
	ar rcs $@ $^

define COMMON_TEMPLATE
$$(OBJDIR)/common/$(1).o: $$(COMMON)/$$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(RELEASEFLAGS) -c $$^ -o $$@
$$(OBJDIR)/common/$(1)_d.o: $$(COMMON)/$$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) -c $$^ -o $$@
endef

$(foreach name,$(COMMON_NAMES),$(eval $(call COMMON_TEMPLATE,$(name))))


#
# Quadcopter-specific objects
#

QUAD_NAMES = geometry gpio radiouart queuebuffer i2c pwm accelerometer \
		gyroscope motor drive

$(LIBDIR)/libquadcopter.a: \
		$(foreach name,$(QUAD_NAMES),$(OBJDIR)/$(name).o)
	ar rcs $@ $^
$(LIBDIR)/libquadcopter_d.a: \
		$(foreach name,$(QUAD_NAMES),$(OBJDIR)/$(name)_d.o)
	ar rcs $@ $^

define QUAD_TEMPLATE
$$(OBJDIR)/$(1).o: $$(SRCDIR)/$(1).cpp $$(INCLUDEDIR)/$(1).h
	$$(CC) $$(CFLAGS) $$(RELEASEFLAGS) -c $$(SRCDIR)/$(1).cpp -o $$@
$$(OBJDIR)/$(1)_d.o: $$(SRCDIR)/$(1).cpp $$(INCLUDEDIR)/$(1).h
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) -c $$(SRCDIR)/$(1).cpp -o $$@
endef

$(foreach name,$(QUAD_NAMES),$(eval $(call QUAD_TEMPLATE,$(name))))


clean:
	rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)

