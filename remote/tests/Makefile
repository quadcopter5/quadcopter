# Remote
# 
# Makefile for test programs for remote

CC = g++
CFLAGS = -I../include
DEBUGFLAGS = -g -D_DEBUG
LDFLAGS = -L../lib

BINDIR = bin
SRCDIR = ../src


all: dirs tests


dirs:
	mkdir -p $(BINDIR)

tests: $(BINDIR)/test_radio.x $(BINDIR)/test_radioconnection.x


# Tests

$(BINDIR)/test_radio.x: test_radio.cpp $(SRCDIR)/radiolinux.cpp \
		$(SRCDIR)/exception.cpp $(SRCDIR)/endianness.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_radio.cpp $(SRCDIR)/radiolinux.cpp \
		$(SRCDIR)/exception.cpp $(SRCDIR)/endianness.c $(LDFLAGS) -o $@

$(BINDIR)/test_radioconnection.x: test_radioconnection.cpp \
		$(SRCDIR)/radioconnection.cpp $(SRCDIR)/packetmotion.cpp \
		$(SRCDIR)/radiolinux.cpp $(SRCDIR)/exception.cpp \
		$(SRCDIR)/endianness.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) test_radioconnection.cpp \
		$(SRCDIR)/radioconnection.cpp $(SRCDIR)/packetmotion.cpp \
		$(SRCDIR)/radiolinux.cpp $(SRCDIR)/exception.cpp \
		$(SRCDIR)/endianness.c $(LDFLAGS) -o $@


clean:
	rm -rf $(BINDIR) *.o *.x

