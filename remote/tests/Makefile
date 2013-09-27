# Remote
# 
# Makefile for test programs for remote

COMMON = ../../common

CC = g++
CFLAGS = -I../include -I$(COMMON)/include
DEBUGFLAGS = -g -D_DEBUG
LDFLAGS = -L../lib

BINDIR = bin
SRCDIR = ../src

TEST_NAMES = test_radio test_radioconnection


all: dirs tests

dirs:
	mkdir -p $(BINDIR)

tests: source $(foreach name,$(TEST_NAMES),$(BINDIR)/$(name).x)

source:
	make -C .. debug


# Tests

define TEST_TEMPLATE
$$(BINDIR)/$(1).x: $(1).cpp ../lib/libremote_d.a ../lib/libcommon_d.a
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) $(1).cpp $$(LDFLAGS) -lremote_d \
		-o $$@ -lcommon_d -lremote_d
endef

$(foreach name,$(TEST_NAMES),$(eval $(call TEST_TEMPLATE,$(name))))


clean:
	rm -rf $(BINDIR) *.o *.x

