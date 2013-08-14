# Makefile for Raspberry Pi peripheral drivers

CC = g++
CFLAGS = -I..
DEBUGFLAGS = -g -D_DEBUG
LDFLAGS = -L../lib -lperipherals_d

OBJDIR = obj
BINDIR = bin


all: dirs tests


dirs:
	mkdir -p $(OBJDIR) $(BINDIR)

tests: $(BINDIR)/test_gpio.x $(BINDIR)/test_uart.x $(BINDIR)/test_queuebuffer.x


# Tests

$(BINDIR)/test_gpio.x: $(OBJDIR)/test_gpio.o
	$(CC) $(OBJDIR)/test_gpio.o $(LDFLAGS) -o $(BINDIR)/test_gpio.x

$(OBJDIR)/test_gpio.o: test_gpio.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c test_gpio.c -o $(OBJDIR)/test_gpio.o

$(BINDIR)/test_uart.x: $(OBJDIR)/test_uart.o
	$(CC) $(OBJDIR)/test_uart.o $(LDFLAGS) -o $(BINDIR)/test_uart.x

$(OBJDIR)/test_uart.o: test_uart.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c test_uart.c -o $(OBJDIR)/test_uart.o

$(BINDIR)/test_queuebuffer.x: $(OBJDIR)/test_queuebuffer.o
	$(CC) $(OBJDIR)/test_queuebuffer.o $(LDFLAGS) -o $(BINDIR)/test_queuebuffer.x

$(OBJDIR)/test_queuebuffer.o: test_queuebuffer.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c test_queuebuffer.c -o $(OBJDIR)/test_queuebuffer.o


clean:
	rm -rf $(OBJDIR) $(BINDIR) *.o *.x

