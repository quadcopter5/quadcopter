/*
	accelerometer.cpp

	Accelerometer class - interface for the ADXL345 (on GY80 board)
*/

#include <stdint.h>
#include <unistd.h>

#include "exception.h"
#include "i2c.h"
#include "geometry.h"
#include "accelerometer.h"

// ADXL345 Register Addresses
#define BW_RATE     0x2C
#define POWER_CTL   0x2D
#define DATA_FORMAT 0x31

#define DATAX0      0x32
#define DATAX1      0x33
#define DATAY0      0x34
#define DATAY1      0x35
#define DATAZ0      0x36
#define DATAZ1      0x37

Accelerometer::Accelerometer(I2C *i2c, uint8_t slaveaddr, Range range,
		SampleRate rate) {
	mI2C = i2c;
	mSlaveAddr = slaveaddr;
	mRange = range;

	setSleep(true);
	setRange(range);
	setSampleRate(rate);
	setSleep(false);

	/*
	char buffer[6];
	buffer[0] = BW_RATE;
	buffer[1] = rate;  // 0b0000**** : 0111 thru 1100 -> 12.5Hz thru 400Hz
	buffer[2] = DATA_FORMAT;
	buffer[3] = range; // 0b000000** : 00 = 2G, 01 = 4G, 10 = 8G, 11 = 16G
	buffer[4] = POWER_CTL;
	buffer[5] = 0b00001000; // Measure (bit 3), !Sleep (bit 2)

	mI2C->write(mSlaveAddr, buffer, 6);
	*/
}

Accelerometer::~Accelerometer() {
	try {
		setSleep(true);
	} catch (...)
		{ /* Don't care... just make sure destructor finishes */ }
}

void Accelerometer::setSleep(bool sleep) {
	char buffer[2];
	buffer[0] = POWER_CTL;

	if (sleep) {
		buffer[1] = 0b00000100; // Sleep (bit 2), !Measure (bit 3)
		mI2C->write(mSlaveAddr, buffer, 2);
	}
	else {
		// Per documentation, standby mode first
		buffer[1] = 0b00000000; // !Sleep (bit 2)
		mI2C->write(mSlaveAddr, buffer, 2);

		// Then enable measurement
		buffer[1] = 0b00001000; // Measure (bit 3)
		mI2C->write(mSlaveAddr, buffer, 2);
	}
}

void Accelerometer::setRange(Range range) {
	mRange = range;

	char buffer[2];
	buffer[0] = DATA_FORMAT;
	buffer[1] = range; // 0b000000** : 00 = 2G, 01 = 4G, 10 = 8G, 11 = 16G
	mI2C->write(mSlaveAddr, buffer, 2);
}

void Accelerometer::setSampleRate(SampleRate rate) {
	char buffer[2];
	buffer[0] = BW_RATE;
	buffer[1] = rate;  // 0b0000**** : 0111 thru 1100 -> 12.5Hz thru 400Hz
	mI2C->write(mSlaveAddr, buffer, 2);
}

Vector3<float> Accelerometer::read() {
	Vector3<float> vector;
	float factor;
	int16_t values[3];
	char buffer = DATAX0;

	mI2C->enqueueWrite(mSlaveAddr, &buffer, 1);
	mI2C->enqueueRead(mSlaveAddr, values, 6);
	mI2C->sendTransaction();

	// From ADXL345 doc, p. 4
	// Described as LSB/g. Number of discrete values per g
	switch (mRange) {
		case RANGE_2G:
			factor = 256.0f;
			break;
		case RANGE_4G:
			factor = 128.0f;
			break;
		case RANGE_8G:
			factor = 64.0f;
			break;
		case RANGE_16G:
			factor = 32.0f;
			break;
	}

	vector.x = (float)values[0] / factor;
	vector.y = (float)values[1] / factor;
	vector.z = (float)values[2] / factor;

	return vector;
}

