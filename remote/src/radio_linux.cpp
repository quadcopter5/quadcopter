/*
	radio_linux.h

	LinuxRadio class - implementation of Radio abstract base class for
		Linux.
*/

#include <string>
#include <stdint.h>

#include <boost/lexical_cast.hpp>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "exception.h"
#include "endianness.h"
#include "radio.h"
#include "radio_linux.h"

LinuxRadio::LinuxRadio(const std::string &devfile, int baudrate, Parity p) {
	mBaudRate = baudrate;
	mParity = p;

	mFD = open(devfile.c_str(), O_RDWR | O_ASYNC);
	if (mFD == -1)
		THROW_EXCEPT(RadioException, "Could not open " + devfile);

	struct termios tprops;
	tcgetattr(mFD, &tprops);

	tprops.c_iflag = 0;
	if (mParity != PARITY_NONE)
		tprops.c_iflag |= INPCK;

	tprops.c_oflag = 0;

	tprops.c_cflag = CS8 | CREAD;
	if (mParity != PARITY_NONE) {
		tprops.c_cflag |= PARENB;
		if (mParity == PARITY_ODD)
			tprops.c_cflag |= PARODD;
	}

	tprops.c_lflag = 0;

	// Disable control characaters
	for (int cc = 0; cc < NCCS; ++cc)
		tprops.c_cc[cc] = _POSIX_VDISABLE;

	tprops.c_cc[VMIN]  = 0; // No minimum number of characters for reads
	tprops.c_cc[VTIME] = 0; // No timeout (0 deciseconds)

	speed_t baudspeed = baudToSpeed(mBaudRate);
	cfsetospeed(&tprops, baudspeed);
	cfsetispeed(&tprops, baudspeed);

	tcsetattr(mFD, TCSAFLUSH, &tprops);

	tcflow(mFD, TCOON | TCION);
	tcflush(mFD, TCIOFLUSH);
}

LinuxRadio::~LinuxRadio() {
	close(mFD);
}

void LinuxRadio::setBaudRate(int baudrate) {
	mBaudRate = baudrate;

	struct termios tprops;
	tcgetattr(mFD, &tprops);

	speed_t baudspeed = baudToSpeed(mBaudRate);
	cfsetospeed(&tprops, baudspeed);
	cfsetispeed(&tprops, baudspeed);

	tcsetattr(mFD, TCSAFLUSH, &tprops);
	tcflush(mFD, TCIOFLUSH);
}

void LinuxRadio::setParity(Parity p) {
	mParity = p;

	struct termios tprops;
	tcgetattr(mFD, &tprops);

	tprops.c_iflag = 0;
	if (mParity != PARITY_NONE)
		tprops.c_iflag |= INPCK;

	tprops.c_cflag = CS8 | CREAD;
	if (mParity != PARITY_NONE) {
		tprops.c_cflag |= PARENB;
		if (mParity == PARITY_ODD)
			tprops.c_cflag |= PARODD;
	}

	tcsetattr(mFD, TCSAFLUSH, &tprops);
	tcflush(mFD, TCIOFLUSH);
}

int LinuxRadio::write(const std::string &buffer) {
	int bytes;
	if ((bytes = ::write(mFD, buffer.c_str(), buffer.length())) == -1)
		THROW_EXCEPT(RadioException, "Failed to write to radio");
	return bytes;
}


int LinuxRadio::writeChar(char c) {
	int bytes;
	if ((bytes = ::write(mFD, &c, 1)) == -1)
		THROW_EXCEPT(RadioException, "Failed to write to radio");
	return bytes;
}

int LinuxRadio::writeUBE16(uint16_t i) {
	int bytes;
	uint16_t swapped;
	hostToBE(&swapped, &i, sizeof(i));
	if ((bytes = ::write(mFD, &swapped, sizeof(swapped))) == -1)
		THROW_EXCEPT(RadioException, "Failed to write to radio");
	return bytes;
}

int LinuxRadio::writeUBE32(uint32_t i) {
	int bytes;
	uint32_t swapped;
	hostToBE(&swapped, &i, sizeof(i));
	if ((bytes = ::write(mFD, &swapped, sizeof(swapped))) == -1)
		THROW_EXCEPT(RadioException, "Failed to write to radio");
	return bytes;
}

int LinuxRadio::read(std::string &buffer, size_t numbytes) {
	int bytes, totalbytes = 0;
	char cbuf[4096];
	std::string result;

	while ((bytes = ::read(mFD, cbuf, 4096)) > 0
			&& (numbytes == 0 || numbytes > 0 && totalbytes < numbytes)) {
		if (bytes == -1)
			THROW_EXCEPT(RadioException, "Failed to read from radio");
		else {
			result.append(cbuf, bytes);
			totalbytes += bytes;
		}
	}

	buffer.assign(result);

	return totalbytes;
}

int LinuxRadio::readChar(char *c) {
	int bytes;
	if ((bytes = ::read(mFD, c, 1)) == -1)
		THROW_EXCEPT(RadioException, "Failed to read from radio");
	return bytes;
}

int LinuxRadio::readUBE16(uint16_t *i) {
	int bytes;
	uint16_t orig;
	if ((bytes = ::read(mFD, &orig, sizeof(orig))) == -1)
		THROW_EXCEPT(RadioException, "Failed to read from radio");
	BEToHost(i, &orig, sizeof(orig));
	return bytes;
}

int LinuxRadio::readUBE32(uint32_t *i) {
	int bytes;
	uint32_t orig;
	if ((bytes = ::read(mFD, &orig, sizeof(orig))) == -1)
		THROW_EXCEPT(RadioException, "Failed to read from radio");
	BEToHost(i, &orig, sizeof(orig));
	return bytes;
}

/*
	Private member functions
*/

speed_t LinuxRadio::baudToSpeed(int baudrate) {
	switch (baudrate) {
		case 1200:
			return B1200;
		case 1800:
			return B1800;
		case 2400:
			return B2400;
		case 4800:
			return B4800;
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
			return B38400;
		case 57600:
			return B57600;
		case 115200:
			return B115200;
		case 230400:
			return B230400;

		default:
			THROW_EXCEPT(RadioException, "Unsupported baudrate ("
					+ boost::lexical_cast<std::string>(baudrate));
	}
}

