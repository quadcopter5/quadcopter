/*
	i2c.h

	I2C class - an interface for RaspberryPi I2C
*/

#ifndef I2C_H
#define I2C_H

#ifndef __cplusplus
#error This header requires C++
#endif

#include <stdint.h>
#include <string>
#include <vector>

#include <i2c.h>

#include "exception.h"

class I2CException : public Exception {
	public:
		I2CException(const std::string &msg, const std::string &file, int line)
				: Exception(msg, file, line) { }
};

class I2C {
	public:
		/**
			Constructor

			name is the name of the Linux I2C device interface,
			e.g. "/dev/i2c-X", where X is a number

			If there is no such device, try appending the following to
			/etc/modules:

			i2c-bcm2708
			i2c-dev

			...and reboot.

			Throws I2CException if the device could not be opened.
		*/
		I2C(const std::string &name);

		/**
			Destructor
		*/
		~I2C();

		/**
			Send the given data to I2C slave with slaveaddr.

			Throws I2CException if the write operation fails.
		*/
		void write(uint8_t slaveaddr, const void *buffer, size_t length);

		/**
			Attempts to read length bytes from I2C slave with slaveaddr. length
			should be less than or equal to the size of the given buffer;
			however, buffer will only be filled with the amount of data that
			the slave sends in response.

			Returns the number of bytes returned by slave.

			Throws I2CException if the read operation fails.
		*/
		size_t read(uint8_t slaveaddr, void *buffer, size_t length);

		/**
			Enqueues a write operation to be sent during the next transaction.
		*/
		void enqueueWrite(uint8_t slaveaddr, const void *buffer, size_t length);

		/**
			Enqueues a read operation to be sent during the next transaction.
		*/
		void enqueueRead(uint8_t slaveaddr, void *buffer, size_t length);

		/**
			Sends the queued read/write operations over the I2C connection.
			The read/write operations are sent in the same order that the
			functions were called.

			Throws I2CException if the operation fails; in this case, the
			queue is kept the same as it was before calling this function.
			However, some sub-operations may have succeeded already.
		*/
		void sendTransaction();

	private:
		int         mFd;            // File descriptor to device
		std::string mFilename;      // The filename that refers to the device
		uint8_t     mLastSlaveAddr; // The last used address, to expedite
		                            // successive operations to the same address

		std::vector<struct i2c_msg> mQueue;

		/**
			Private copy constructor
			Disallows copying of I2C objects, as the I2C interface contains
			mutable state which should not be shared between multiple instances.
		*/
		I2C(const I2C &other);

		/**
			Private copy assignment
			Disallows copying of I2C objects, see copy constructor.
		*/
		I2C &operator=(const I2C &other);
};

#endif

