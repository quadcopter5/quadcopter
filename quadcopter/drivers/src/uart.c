/**
	Philip Romano
	UART Interface for Raspberry Pi (Broadcom 2835)
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "uart.h"
#include "queuebuffer.h"

// Internal error handling data
#define UART_ERRSIZE 128
static int error = 0;
static char error_str[UART_ERRSIZE];

static void generateError(const char *str);

// File descriptor to the UART terminal interface
static int uartfd = -1;

// QueueBuffer as an intermediate place to store data as it comes in.
static QueueBuffer *queuebuffer = 0;

// Initialized in uart_init
//   0  = little endian
//   1  = big endian
//   -1 = unsupported
static int hostendian = 0;

// Reverse the byte order of src, storing into dest
static void swapEndian(const void *src, void *dest, size_t numbytes);

// SIgnal handler for SIGIO; called by kernel when IO data becomes
// available.
static void sigHandlerIO(int signumber);

int uart_init(int baudrate, UARTParity parity) {
	// Establish endianness of the running system
	uint32_t test;
	char *set = (char *)&test;
	set[0] = 0x11;
	set[1] = 0x22;
	set[2] = 0x33;
	set[3] = 0x44;

	if (test == 0x11223344)
		hostendian = 1;  // Big-endian
	else if (test == 0x44332211)
		hostendian = 0;  // Little-endian
	else
		hostendian = -1; // Unsupported

	// Convert to speed_t / validate provided baudrate
	speed_t baud;
	switch (baudrate) {
		case 1200:
			baud = B1200;
			break;
		case 1800:
			baud = B1800;
			break;
		case 2400:
			baud = B2400;
			break;
		case 4800:
			baud = B4800;
			break;
		case 9600:
			baud = B9600;
			break;
		case 19200:
			baud = B19200;
			break;
		case 38400:
			baud = B38400;
			break;
		case 57600:
			baud = B57600;
			break;
		case 115200:
			baud = B115200;
			break;
		case 230400:
			baud = B230400;
			break;

		default:
			generateError("Unsupported baud rate requested");
			return 0;
	}

	uartfd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (uartfd == -1) {
		generateError("Could not open /dev/ttyAMA0");
		return 0;
	}

	struct sigaction sa;
	sa.sa_handler = sigHandlerIO;
	sa.sa_flags = 0;
	sa.sa_restorer = NULL;
	sigaction(SIGIO, &sa, NULL);

	fcntl(uartfd, F_SETOWN, getpid());
	fcntl(uartfd, F_SETFL, O_ASYNC);

	// Set terminal properties for /dev/ttyAMA0
	struct termios tprops;

	tprops.c_iflag = 0;
	if (parity != UART_PARDISABLE)
		tprops.c_iflag = INPCK;

	tprops.c_oflag = 0;

	tprops.c_cflag = CS8 | CREAD;
	if (parity != UART_PARDISABLE) {
		tprops.c_cflag |= PARENB;
		if (parity == UART_PARODD)
			tprops.c_cflag |= PARODD;
	}

	tprops.c_lflag = 0;

	// Disable control characters
	int cc;
	for (cc = 0; cc < NCCS; ++cc)
		tprops.c_cc[cc] = _POSIX_VDISABLE;

	tprops.c_cc[VMIN] = 0;  // No minimum number of characters for reads
	tprops.c_cc[VTIME] = 0; // No timeout (0 deciseconds)

	// Baud rate
	cfsetospeed(&tprops, baud);
	cfsetispeed(&tprops, baud);

	// Set the attributes
	tcsetattr(uartfd, TCSAFLUSH, &tprops);

	tcflow(uartfd, TCOON | TCION); // Restart input and output
	tcflush(uartfd, TCIOFLUSH);    // Flush buffer for clean start

	qb_initialize(&queuebuffer);

	return 1;
}

int uart_deinit() {
	if (close(uartfd) != 0) {
		generateError("Could not close /dev/ttyAMA0");
		return 0;
	}

	qb_free(&queuebuffer);

	return 1;
}

int uart_write(const void *buffer, size_t len) {
	if (uartfd == -1) {
		generateError("UART has not been initialized");
		return 0;
	}

	write(uartfd, buffer, len);
	return 1;
}

int uart_writeChar(char c) {
	return uart_write(&c, 1);
}

int uart_writeUBE16(uint16_t i) {
	if (hostendian == 1)
		return uart_write(&i, 2);
	else if (hostendian == 0) {
		uint16_t swapped;
		swapEndian(&i, &swapped, 2);
		return uart_write(&swapped, 2);
	} else {
		generateError("System endianness is not supported. Use uart_write()");
		return 0;
	}
}

int uart_writeUBE32(uint32_t i) {
	if (hostendian == 1)
		return uart_write(&i, 4);
	else if (hostendian == 0) {
		uint16_t swapped;
		swapEndian(&i, &swapped, 4);
		return uart_write(&swapped, 4);
	} else {
		generateError("System endianness is not supported. Use uart_write()");
		return 0;
	}
}

int uart_read(void *buffer, size_t len) {
	if (uartfd == -1) {
		generateError("UART has not been initialized");
		return 0;
	}

	return qb_pop(queuebuffer, buffer, len);
}

int uart_readChar(char *c) {
	// qb_pop() will return 1 if there is a byte, or 0 if there are no bytes
	return uart_read(c, 1);
}

int uart_readUBE16(uint16_t *i) {
	if (uartfd == -1) {
		generateError("UART has not been initialized");
		return 0;
	}

	if (qb_getSize(queuebuffer) >= 2) {
		if (hostendian == 1) {
			qb_pop(queuebuffer, i, 2);
			return 1;
		}
		else if (hostendian == 0) {
			uint16_t be;
			qb_pop(queuebuffer, &be, 2);
			swapEndian(&be, i, 2);
			return 1;
		} else {
			generateError("System endianness is not supported. Use uart_write()");
			return 0;
		}
	} else {
		generateError("Not enough data in queue to read 16-bit integer");
		return 0;
	}
}

int uart_readUBE32(uint32_t *i) {
	if (uartfd == -1) {
		generateError("UART has not been initialized");
		return 0;
	}

	if (qb_getSize(queuebuffer) >= 4) {
		if (hostendian == 1) {
			qb_pop(queuebuffer, i, 4);
			return 1;
		}
		else if (hostendian == 0) {
			uint32_t be;
			qb_pop(queuebuffer, &be, 4);
			swapEndian(&be, i, 4);
			return 1;
		} else {
			generateError("System endianness is not supported. Use uart_write()");
			return 0;
		}
	} else {
		generateError("Not enough data in queue to read 32-bit integer");
		return 0;
	}
}

int uart_getInputQueueSize() {
	if (!queuebuffer) {
		generateError("UART has not been initialized");
		return -1;
	}

	return qb_getSize(queuebuffer);
}

const char *uart_getLastError() {
	if (error) {
		error = 0;
		return error_str;
	} else
		return "No error";
}

void sigHandlerIO(int signumber) {
	if (queuebuffer) {
		char buffer[64];
		int bytes;
		while ((bytes = read(uartfd, buffer, 64)) > 0)
			qb_push(queuebuffer, buffer, bytes);
	}
}

void generateError(const char *str) {
	error = 1;
	strncpy(error_str, str, UART_ERRSIZE);
}

void swapEndian(const void *src, void *dest, size_t numbytes) {
	char   *swapped = (char *)dest,
	       *srcbyte = (char *)src;
	size_t i;

	for (i = 0; i < numbytes; ++i)
		swapped[i] = srcbyte[numbytes - i - 1];
}

