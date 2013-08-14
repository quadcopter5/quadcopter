# Makefile for Raspberry Pi peripheral drivers

CC = g++
CFLAGS = -I.
DEBUGFLAGS = -g -D_DEBUG
RELEASEFLAGS = -O3

SRCDIR = src

OBJDIR = obj
LIBDIR = lib

.PHONY: all debug dirs tests clean

all: dirs $(LIBDIR)/libperipherals.a tests

debug: dirs $(LIBDIR)/libperipherals_d.a


dirs:
	mkdir -p $(OBJDIR) $(LIBDIR)


# Driver archive
$(LIBDIR)/libperipherals.a: $(OBJDIR)/gpio.o $(OBJDIR)/uart.o \
		$(OBJDIR)/queuebuffer.o
	ar rcs $(LIBDIR)/libperipherals.a $(OBJDIR)/gpio.o $(OBJDIR)/uart.o \
		$(OBJDIR)/queuebuffer.o

$(LIBDIR)/libperipherals_d.a: $(OBJDIR)/gpio_d.o $(OBJDIR)/uart_d.o \
		$(OBJDIR)/queuebuffer_d.o
	ar rcs $(LIBDIR)/libperipherals_d.a $(OBJDIR)/gpio_d.o $(OBJDIR)/uart_d.o \
		$(OBJDIR)/queuebuffer_d.o


tests:
	make --directory=tests all


# Driver object files

$(OBJDIR)/gpio.o: $(SRCDIR)/gpio.c gpio.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/gpio.c -o $(OBJDIR)/gpio.o

$(OBJDIR)/uart.o: $(SRCDIR)/uart.c uart.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/uart.c -o $(OBJDIR)/uart.o

$(OBJDIR)/queuebuffer.o: $(SRCDIR)/queuebuffer.c queuebuffer.h
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c $(SRCDIR)/queuebuffer.c \
		-o $(OBJDIR)/queuebuffer.o


$(OBJDIR)/gpio_d.o: $(SRCDIR)/gpio.c gpio.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/gpio.c -o $(OBJDIR)/gpio_d.o

$(OBJDIR)/uart_d.o: $(SRCDIR)/uart.c uart.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/uart.c -o $(OBJDIR)/uart_d.o

$(OBJDIR)/queuebuffer_d.o: $(SRCDIR)/queuebuffer.c queuebuffer.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(SRCDIR)/queuebuffer.c \
		-o $(OBJDIR)/queuebuffer_d.o


clean:
	rm -rf $(OBJDIR) $(LIBDIR) *.o *.x *.a
	make --directory=tests clean

