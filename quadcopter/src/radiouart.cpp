/*
	radiouart.cpp

	RadioUART class - UART interface for Raspberry Pi (Broadcom 2835)
*/

#include <string>
#include <stdint.h>
// #include <string.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
// #include <signal.h>

#include "endianness.h"
#include "queuebuffer.h"
#include "radio.h"
#include "radiouart.h"

RadioUART::RadioUART(int baudrate, Radio::Parity parity) {
	mFD = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mFD == -1)
		THROW_EXCEPT(RadioException, "Could not open /dev/ttyAMA0");

	/* // From before containing the code into a class
	struct sigaction sa;
	sa.sa_handler = sigHandlerIO;
	sa.sa_flags = 0;
	sa.sa_restorer = NULL;
	sigaction(SIGIO, &sa, NULL);

	fcntl(uartfd, F_SETOWN, getpid());
	fcntl(uartfd, F_SETFL, O_ASYNC);
	*/

	setParity(parity);
	setBaudRate(baudrate);

	qb_initialize(&mQueueBuffer);
}

RadioUART::~RadioUART() {
	close(mFD);
	qb_free(&mQueueBuffer);
}

void RadioUART::setBaudRate(int baudrate) {
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
			baud = B9600;
			break;
	}

	struct termios tprops;
	tcgetattr(mFD, &tprops);

	cfsetospeed(&tprops, baud);
	cfsetispeed(&tprops, baud);

	tcsetattr(mFD, TCSAFLUSH, &tprops);

	tcflow(mFD, TCOON | TCION); // Restart input and output
	tcflush(mFD, TCIOFLUSH);    // Flush buffer for clean start
}

void RadioUART::setParity(Radio::Parity parity) {
	struct termios tprops;
	tcgetattr(mFD, &tprops);

	tprops.c_iflag = 0;
	if (parity != Radio::PARITY_NONE)
		tprops.c_iflag = INPCK;

	tprops.c_oflag = 0;

	tprops.c_cflag = CS8 | CREAD;
	if (parity != Radio::PARITY_NONE) {
		tprops.c_cflag |= PARENB;
		if (parity == Radio::PARITY_ODD)
			tprops.c_cflag |= PARODD;
	}

	tprops.c_lflag = 0;

	// Disable control characters
	int cc;
	for (cc = 0; cc < NCCS; ++cc)
		tprops.c_cc[cc] = _POSIX_VDISABLE;

	tprops.c_cc[VMIN] = 0;  // No minimum number of characters for reads
	tprops.c_cc[VTIME] = 0; // No timeout (0 deciseconds)

	// Set the attributes
	tcsetattr(mFD, TCSAFLUSH, &tprops);

	tcflow(mFD, TCOON | TCION); // Restart input and output
	tcflush(mFD, TCIOFLUSH);    // Flush buffer for clean start
}

int RadioUART::write(const std::string &buffer) {
	int bytes = ::write(mFD, buffer.c_str(), buffer.length());
	if (bytes == -1)
		THROW_EXCEPT(RadioException, "Write to radio failed");
	return bytes;
}

int RadioUART::writeChar(char c) {
	int bytes;
	if ((bytes = ::write(mFD, &c, 1)) == -1)
		THROW_EXCEPT(RadioException, "Write to radio failed");
	return bytes;
}

int RadioUART::writeUBE16(uint16_t i) {
	int bytes;
	uint16_t swapped;
	hostToBE(&swapped, &i, sizeof(i));
	if ((bytes = ::write(mFD, &swapped, sizeof(swapped))) == -1)
		THROW_EXCEPT(RadioException, "Write to radio failed");
	return bytes;
}

int RadioUART::writeUBE32(uint32_t i) {
	int bytes;
	uint32_t swapped;
	hostToBE(&swapped, &i, sizeof(i));
	if ((bytes = ::write(mFD, &swapped, sizeof(swapped))) == -1)
		THROW_EXCEPT(RadioException, "Write to radio failed");
	return bytes;
}

int RadioUART::read(std::string &buffer, size_t numbytes) {
	updateQueueBuffer();

	int bytes;
	char tempbuf[numbytes];
	bytes = qb_pop(mQueueBuffer, tempbuf, numbytes);
	buffer.assign(tempbuf, bytes);

	return bytes;
}

int RadioUART::readChar(char *c) {
	updateQueueBuffer();
	return qb_pop(mQueueBuffer, c, 1);
}

int RadioUART::readUBE16(uint16_t *i) {
	updateQueueBuffer();

	if (qb_getSize(mQueueBuffer) >= 2) {
		qb_pop(mQueueBuffer, i, sizeof(*i));
		BEToHost(i, i, sizeof(*i));
		return 2;
	} else
		return 0;
}

int RadioUART::readUBE32(uint32_t *i) {
	updateQueueBuffer();

	if (qb_getSize(mQueueBuffer) >= 4) {
		qb_pop(mQueueBuffer, i, sizeof(*i));
		BEToHost(i, i, sizeof(*i));
		return 4;
	} else
		return 0;
}

int RadioUART::getInputQueueSize() {
	updateQueueBuffer();
	return qb_getSize(mQueueBuffer);
}

/*
	Private member functions
*/

void RadioUART::updateQueueBuffer() {
	char buffer[64];
	int bytes;
	while ((bytes = ::read(mFD, buffer, 64)) > 0)
		qb_push(mQueueBuffer, buffer, bytes);
}

