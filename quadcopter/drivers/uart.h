/**
	Philip Romano
	UART Interface for Raspberry Pi (Broadcom 2835)
*/

#ifndef UART_H
#define UART_H

#include <stdint.h>

typedef enum _UARTParity {
	UART_PARDISABLE = 0,
	UART_PARODD = 1,
	UART_PAREVEN = 2
} UARTParity;

/**
	Initialize UART functionality.
	This must be called before calling any other functions in this header!

	This function assumes that the UART reference clock is running at
	7.3728 MHz. On Raspbian, you can set the UART clock by adding or changing
	the following line:

	init_uart_clock=7372800

	Returns 1 on success, 0 on error.
	Use uart_getLastError() to get a description of the error.
*/
int uart_init(int baudrate, UARTParity parity);

/**
	Properly close UART functionality.
	This should be called when UART is no longer needed (i.e. the end of
	the program)

	Returns 1 on success, 0 on error.
	Use uart_getLastError() to get a description of the error.
*/
int uart_deinit();

/**
	Write the contents of buffer to UART transmit line, byte-per-byte.
	The size of buffer is assumed to be len bytes.

	Returns 1 on success, 0 on error.
*/
int uart_write(const void *buffer, size_t len);

/**
	Write a single character to UART Tx.

	Returns 1 on success, 0 on error.
*/
int uart_writeChar(char c);

/**
  Write the given integer to UART Tx as a 16-bit big-endian integer.
  This function converts from host to BE.

  Returns 1 on success, 0 on error.
*/
int uart_writeUBE16(uint16_t i);

/**
  Write the given integer to UART Tx as a 32-bit big-endian integer.
  This function converts from host to BE.

  Returns 1 on success, 0 on error.
*/
int uart_writeUBE32(uint32_t i);

/**
	Read the next len bytes from UART Rx into buffer, byte-per-byte.

	Returns the actual number of bytes read.
	If the number of bytes in the queue is less than len, then reading will
	end before reaching len bytes.
*/
int uart_read(void *buffer, size_t len);

/**
	Read the next character from the queue buffer into c.

	Returns 1 on success, 0 on error.
*/
int uart_readChar(char *c);

/**
  Read the next 2 bytes of the queue buffer as a 16-bit big-endian integer.
  This function converts the data from BE to host.

  Returns 1 on success, 0 on error.
*/
int uart_readUBE16(uint16_t *i);

/**
  Read the next 4 bytes of the queue buffer as a 32-bit big-endian integer.
  This function converts the data from BE to host.

  Returns 1 on success, 0 on error.
*/
int uart_readUBE32(uint32_t *i);

/**
	Get the size of the input queue buffer.

	Returns the current number of bytes remaining in the input queue buffer,
	or -1 if UART has not been initialized. These are bytes that have been
	received and stored but not yet read by the application.
*/
int uart_getInputQueueSize();

/**
	Returns a string containing a descripiton of the last error. If no error
	has occurred since the last call to this function, then the string
	"No Error" is returned.
*/
const char *uart_getLastError();

#endif

