# Remote
# 
# Makefile for test programs for remote

COMMON = ../../common

CC = g++
CFLAGS = -I../include -I$(COMMON)/include -Ivisualize -I../../quadcopter/include
DEBUGFLAGS = -g -D_DEBUG
LDFLAGS = -L../lib -L./visualize/lib

BINDIR = bin
SRCDIR = ../src

TEST_NAMES = test_radio test_radioconnection test_radio_alternate \
		test_packets test_stabilize

VISUAL_TEST_NAMES = test_visualizer test_stabilize_visual

all: dirs tests

dirs:
	mkdir -p $(BINDIR)

tests: source $(foreach name,$(TEST_NAMES),$(BINDIR)/$(name).x) \
		$(foreach name,$(VISUAL_TEST_NAMES),$(BINDIR)/$(name).x)

source:
	make -C .. debug


# Tests

define TEST_TEMPLATE
$$(BINDIR)/$(1).x: $(1).cpp ../lib/libremote_d.a ../lib/libcommon_d.a
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) $(1).cpp $$(LDFLAGS) -lremote_d \
		-o $$@ -lcommon_d -lremote_d
endef

$(foreach name,$(TEST_NAMES),$(eval $(call TEST_TEMPLATE,$(name))))


define VISUAL_TEST_TEMPLATE
$$(BINDIR)/$(1).x: $(1).cpp ../lib/libremote_d.a ../lib/libcommon_d.a \
		visualize/lib/libgraphics_d.a visualize/visualizer.cpp \
		visualize/visualizer.h
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) $(1).cpp visualize/visualizer.cpp \
		$$(LDFLAGS) -o $$@ -lcommon_d -lremote_d -lgraphics_d -lGL -lSDL2 \
		-lSDL2main -lfreetype
endef

$(foreach name,$(VISUAL_TEST_NAMES),$(eval $(call VISUAL_TEST_TEMPLATE,$(name))))


clean:
	rm -rf $(BINDIR) *.o *.x

