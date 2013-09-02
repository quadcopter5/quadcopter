/*
	i2c.cpp

	I2C class - interface for RaspberryPi I2C
*/

#include <stdint.h>
#include <string>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "exception.h"
#include "i2c.h"

I2C::I2C(const std::string &name) : mFilename(name) {
	mLastSlaveAddr = 0x00;

	// Open device
	mFd = open(mFilename.c_str(), O_RDWR);
	if (mFd == -1)
		THROW_EXCEPT(I2CException, "Could not open " + name);

	// Set initial slave to 0; this is defined as a broadcast to all slaves on
	// the line. So mLastSlaveAddr is consistent with internal device driver
	// state.
	if (ioctl(mFd, I2C_SLAVE, mLastSlaveAddr) < 0)
		THROW_EXCEPT(I2CException, "Could not set I2C slave\n");
}

I2C::~I2C() {
	close(mFd);
}

void I2C::write(uint8_t slaveaddr, const void *data, size_t length) {
	if (slaveaddr != mLastSlaveAddr) {
		if (ioctl(mFd, I2C_SLAVE, slaveaddr) < 0)
			THROW_EXCEPT(I2CException, "Could not set I2C slave\n");
		else
			mLastSlaveAddr = slaveaddr;
	}

	int bytes;
	if ((bytes = ::write(mFd, data, length)) < length) {
		int err = errno;
		if (bytes >= 0)
			THROW_EXCEPT(I2CException, "I2C write could not write all bytes");
		else
			THROW_EXCEPT(I2CException, "I2C write operation failed: "
					+ std::string(strerror(err)));
	}
}

size_t I2C::read(uint8_t slaveaddr, void *buffer, size_t length) {
	if (slaveaddr != mLastSlaveAddr) {
		if (ioctl(mFd, I2C_SLAVE, slaveaddr) < 0)
			THROW_EXCEPT(I2CException, "Could not set I2C slave\n");
		else
			mLastSlaveAddr = slaveaddr;
	}

	size_t bytes;
	if ((bytes = ::read(mFd, buffer, length)) == -1)
		THROW_EXCEPT(I2CException, "I2C read operation failed: "
				+ std::string(strerror(errno)));
	return bytes;
}

void I2C::enqueueWrite(uint8_t slaveaddr, const void *buffer, size_t length) {
	struct i2c_msg msg;
	msg.addr = slaveaddr;
	msg.flags = 0;
	msg.len = length;
	msg.buf = (uint8_t *)buffer;
	mQueue.push_back(msg);
}

void I2C::enqueueRead(uint8_t slaveaddr, void *buffer, size_t length) {
	struct i2c_msg msg;
	msg.addr = slaveaddr;
	msg.flags = I2C_M_RD;
	msg.len = length;
	msg.buf = (uint8_t *)buffer;
	mQueue.push_back(msg);
}

void I2C::sendTransaction() {
	if (mQueue.size() > 0) {
		struct i2c_rdwr_ioctl_data iodata;
		iodata.msgs = &mQueue[0];
		iodata.nmsgs = mQueue.size();

		int iostatus;
		if ((iostatus = ioctl(mFd, I2C_RDWR, &iodata)) < 0)
			THROW_EXCEPT(I2CException, "I2C ioctl() failed");

		mQueue.clear();
	}
}

