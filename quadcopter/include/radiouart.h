/*
	radiouart.h

	RadioUART class - UART interface for Raspberry Pi (Broadcom 2835)
*/

#ifndef RADIOUART_H
#define RADIOUART_H

#include <string>
#include <stdint.h>

#include "exception.h"
#include "radio.h"
#include "queuebuffer.h"

class RadioUART : public Radio {
	public:
		/**
			Constructor

			Initialize UART functionality.
			This must be called before calling any other functions in this
			header!

			This function assumes that the UART reference clock is running at
			7.3728 MHz. On Raspbian, you can set the UART clock by adding or
			changing the following line:

			init_uart_clock=7372800

			Throws RadioException if initialization fails.
		*/
		RadioUART(int baudrate, Radio::Parity parity);

		/**
			Destructor

			Properly closes UART functionality.
		*/
		~RadioUART();

		/**
			Set the baudrate used to communicate over UART.
		*/
		virtual void setBaudRate(int baudrate);

		/**
			Set whether parity is used while communicating over UART
		*/
		virtual void setParity(Radio::Parity parity);

		/**
			Write the contents of buffer to UART transmit line, byte-per-byte.
			The size of buffer is assumed to be len bytes.

			Returns the number of bytes actually written.

			Throws RadioException
		*/
		virtual int write(const std::string &buffer);

		/**
			Write a single character to UART Tx.

			Returns the number of bytes written (1 or 0).

			Throws RadioException
		*/
		virtual int writeChar(char c);

		/**
		  Write the given integer to UART Tx as a 16-bit big-endian integer.
		  This function converts from host to BE.

		  Returns the number of bytes written (2 or 0).

		  Throws RadioException
		*/
		virtual int writeUBE16(uint16_t i);

		/**
		  Write the given integer to UART Tx as a 32-bit big-endian integer.
		  This function converts from host to BE.

		  Returns the number of bytes written (4 or 0).

		  Throws RadioException
		*/
		virtual int writeUBE32(uint32_t i);

		/**
			Read the next len bytes from UART Rx into buffer, byte-per-byte.
			The current contents of buffer are replaced with the received
			data.

			If numbytes == 0, then the function will read all available data
			into buffer.

			Returns the actual number of bytes read (less than or equal to
			numbytes), which may be 0 if the queue is empty.

			Throws RadioException
		*/
		virtual int read(std::string &buffer, size_t numbytes = 0);

		/**
			Read the next character from the queue buffer into c.

			Returns the number of bytes read (1 or 0).

			Throws RadioException
		*/
		virtual int readChar(char *c);

		/**
			Read the next 2 bytes of the queue buffer as a 16-bit big-endian
			integer. This function converts the data from BE to host.

			Returns the number of bytes read (2 or 0)

			Throws RadioException
		*/
		virtual int readUBE16(uint16_t *i);

		/**
			Read the next 4 bytes of the queue buffer as a 32-bit big-endian
			integer. This function converts the data from BE to host.

			Returns the number of bytes read (4 or 0)

			Throws RadioException
		*/
		virtual int readUBE32(uint32_t *i);

		/**
			Updates the input queue buffer, and then gets its size.

			Returns the current number of bytes remaining in the input queue
			buffer. These are bytes that have been received and stored but
			not yet read by the application.
		*/
		int getInputQueueSize();

	private:
		int         mFD;
		QueueBuffer *mQueueBuffer;

		/**
			Update the QueueBuffer with the latest received data.
		*/
		void updateQueueBuffer();
};

/**
	Returns a string containing a descripiton of the last error. If no error
	has occurred since the last call to this function, then the string
	"No Error" is returned.

const char *uart_getLastError();
*/

#endif

