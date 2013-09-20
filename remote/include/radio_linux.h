/*
	radio_linux.h

	LinuxRadio class - implementation of Radio abstract base class for
		Linux.
*/

#ifndef RADIO_LINUX_H
#define RADIO_LINUX_H

#include <string>
#include <stdint.h>

#include <termios.h>

#include "exception.h"
#include "radio.h"

class LinuxRadio : public Radio {
	public:
		/**
			Constructor

			Establishes a connection to the local radio module using the
			given device filename (e.g. "/dev/ttyUSB0")

			Throws RadioException if the setup fails.
		*/
		LinuxRadio(const std::string &devfile, int baudrate, Parity p);

		/**
			Destructor

			Closes the connection with the radio module.
		*/
		~LinuxRadio();

		/**
			Set the baudrate used to communicate with the radio module.
		*/
		virtual void setBaudRate(int baudrate);

		/**
			Set whether parity is used while communicating with the radio
			module.
		*/
		virtual void setParity(Radio::Parity p);

		/**
			Write the contents of buffer to the radio.

			Returns the number of bytes written.

			Throws RadioException if the data fails to send.
		*/
		virtual int write(const std::string &buffer);

		/**
			Write a single byte to the radio.

			Returns the number of bytes written (1 or 0).

			Throws RadioException if the data fails to send.
		*/
		virtual int writeChar(char c);

		/**
			Write an unsigned big-endian 16-bit integer to the radio. The
			function converts from host to BE.

			Returns the number of bytes written (2 or 0)

			Throws RadioException of the data fails to send.
		*/
		virtual int writeUBE16(uint16_t i);

		/**
			Write an unsigned big-endian 32-bit integer to the radio. The
			function converts from host to BE.

			Returns the number of bytes written (4 or 0)

			Throws RadioException of the data fails to send.
		*/
		virtual int writeUBE32(uint32_t i);

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
		virtual int read(std::string &buffer, size_t numbytes = 0);

		/**
			Read a single byte from the radio.

			Returns the number of bytes read (1 or 0)

			Throws RadioException if reading fails. This is different than
			having no data to read; in that case, the function will return 0.
		*/
		virtual int readChar(char *c);

		/**
			Read an unsigned big-endian 16-bit integer from the radio. The
			function converts from BE to host.

			Returns the number of bytes read (2 or 0)

			Throws RadioException if reading fails. This is different than
			having no data to read; in that case, the function will return 0.
		*/
		virtual int readUBE16(uint16_t *i);

		/**
			Read an unsigned big-endian 32-bit integer from the radio. The
			function converts from BE to host.

			Returns the number of bytes read (4 or 0)

			Throws RadioException if reading fails. This is different than
			having no data to read; in that case, the function will return 0.
		*/
		virtual int readUBE32(uint32_t *i);

	private:
		int mFD;   // File descriptor to radio

		int    mBaudRate;
		Radio::Parity mParity;

		/**
			Convert an integer baudrate to POSIX speed_t.

			Throws RadioException if the baudrate is not supported.
		*/
		speed_t baudToSpeed(int baudrate);
};

#endif

