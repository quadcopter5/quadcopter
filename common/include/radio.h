/*
	radio.h

	Radio class - abstract base class that represents a connection to an XBee
		radio module. Implementation must be defined in subclasses.
*/

#ifndef RADIO_H
#define RADIO_H

#include <string>
#include <stdint.h>

#include "exception.h"

class RadioException : public Exception {
	public:
		RadioException(const std::string &msg, const std::string &file,
				int line) : Exception(msg, file, line) { }
};

class Radio {
	public:
		/**
			Set the baudrate used to communicate with the radio module.
		*/
		virtual void setBaudRate(int baudrate) = 0;

		enum Parity {
			PARITY_NONE = 0,
			PARITY_ODD,
			PARITY_EVEN
		};

		/**
			Set whether parity is used while communicating with the radio
			module.
		*/
		virtual void setParity(Parity p) = 0;

		/**
			Write the contents of buffer to the radio.

			Returns the number of bytes written.

			Throws RadioException if the data fails to send.
		*/
		virtual int write(const std::string &buffer) = 0;

		/**
			Write a single byte to the radio.

			Returns the number of bytes written (1 or 0).

			Throws RadioException if the data fails to send.
		*/
		virtual int writeChar(char c) = 0;

		/**
			Write an unsigned big-endian 16-bit integer to the radio. The
			function converts from host to BE.

			Returns the number of bytes written (2 or 0)

			Throws RadioException of the data fails to send.
		*/
		virtual int writeUBE16(uint16_t i) = 0;

		/**
			Write an unsigned big-endian 32-bit integer to the radio. The
			function converts from host to BE.

			Returns the number of bytes written (4 or 0)

			Throws RadioException of the data fails to send.
		*/
		virtual int writeUBE32(uint32_t i) = 0;

		/**
			Read numbytes from the radio into buffer. After this call, the
			previous contents of buffer will be replaced by the data read from
			the radio.

			If numbytes == 0, then the function will read all available data
			into buffer.

			Returns the number of bytes read (less than or equal to numbytes)

			Throws RadioException if reading fails. This is different than
			having no data to read; in that case, the function will return 0.
			buffer is unmodified in case of exception.
		*/
		virtual int read(std::string &buffer, size_t numbytes = 0) = 0;

		/**
			Read a single byte from the radio.

			Returns the number of bytes read (1 or 0)

			Throws RadioException if reading fails. This is different than
			having no data to read; in that case, the function will return 0.
		*/
		virtual int readChar(char *c) = 0;

		/**
			Read an unsigned big-endian 16-bit integer from the radio. The
			function converts from BE to host.

			Returns the number of bytes read (2 or 0)

			Throws RadioException if reading fails. This is different than
			having no data to read; in that case, the function will return 0.
		*/
		virtual int readUBE16(uint16_t *i) = 0;

		/**
			Read an unsigned big-endian 32-bit integer from the radio. The
			function converts from BE to host.

			Returns the number of bytes read (4 or 0)

			Throws RadioException if reading fails. This is different than
			having no data to read; in that case, the function will return 0.
		*/
		virtual int readUBE32(uint32_t *i) = 0;
};

#endif

