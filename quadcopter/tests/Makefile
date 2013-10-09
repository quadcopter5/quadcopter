#
# Makefile for Raspberry Pi tests
#

CC = g++
CFLAGS = -I../include -I../../common/include
DEBUGFLAGS = -g -D_DEBUG
LDFLAGS = -L../lib -lquadcopter_d -lcommon_d

BINDIR = bin

TEST_NAMES = test_gpio test_radiouart test_queuebuffer test_i2c test_pwm \
		test_accelerometer test_motor test_radioconnection test_radio_alternate \
		test_orientation test_packets


.PHONY: all dirs tests source

all: dirs tests


dirs: $(BINDIR)/
$(BINDIR)/:
	mkdir -p $(BINDIR)

tests: source $(foreach name,$(TEST_NAMES),$(BINDIR)/$(name).x)

source:
	make -C .. debug

# Tests

define TEST_TEMPLATE
$$(BINDIR)/$(1).x: $(1).cpp ../lib/libquadcopter_d.a
	$$(CC) $$(CFLAGS) $$(DEBUGFLAGS) $(1).cpp $$(LDFLAGS) -o $$@
endef

$(foreach name,$(TEST_NAMES),$(eval $(call TEST_TEMPLATE,$(name))))


clean:
	rm -rf $(BINDIR) *.o *.x

