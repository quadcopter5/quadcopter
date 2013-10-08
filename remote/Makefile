#
# Makefile for the remote program
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

release: dirs $(LIBDIR)/libcommon.a $(LIBDIR)/libremote.a $(BINDIR)/remote.x

debug: dirs $(LIBDIR)/libcommon_d.a $(LIBDIR)/libremote_d.a \
		$(BINDIR)/remote_d.x

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

$(BINDIR)/remote.x: $(OBJDIR)/remote.o $(OBJDIR)/common/exception.o \
		$(LIBDIR)/libcommon.a
	$(CC) $^ $(LDFLAGS) -lcommon -o $@

$(BINDIR)/remote_d.x: $(OBJDIR)/remote_d.o \
		$(OBJDIR)/common/exception_d.o $(LIBDIR)/libcommon_d.a
	$(CC) $^ $(LDFLAGS) -lcommon_d -o $@


$(OBJDIR)/remote.o: $(SRCDIR)/remote.cpp
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $^ -o $@

$(OBJDIR)/remote_d.o: $(SRCDIR)/remote.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $^ -o $@


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

define COMMON_TEMPLATE =
$$(OBJDIR)/common/$(1).o: $$(COMMON)/$$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(RELEASEFLAGS) -c $$^ -o $$@
$$(OBJDIR)/common/$(1)_d.o: $$(COMMON)/$$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) -c $$^ -o $$@
endef

$(foreach name,$(COMMON_NAMES),$(eval $(call COMMON_TEMPLATE,$(name))))


#
# Remote-specific objects
#

REMOTE_NAMES = radiolinux

$(LIBDIR)/libremote.a: \
		$(foreach name,$(REMOTE_NAMES),$(OBJDIR)/$(name).o)
	ar rcs $@ $^
$(LIBDIR)/libremote_d.a: \
		$(foreach name,$(REMOTE_NAMES),$(OBJDIR)/$(name)_d.o)
	ar rcs $@ $^

define REMOTE_TEMPLATE =
$$(OBJDIR)/$(1).o: $$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(RELEASEFLAGS) -c $$^ -o $$@
$$(OBJDIR)/$(1)_d.o: $$(SRCDIR)/$(1).cpp
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) -c $$^ -o $$@
endef

$(foreach name,$(REMOTE_NAMES),$(eval $(call REMOTE_TEMPLATE,$(name))))


clean:
	rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)

