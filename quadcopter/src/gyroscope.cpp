/*
	gyroscope.cpp

	Gyroscope class - interface for the L3G4200D (on GY80 board)
*/

#include <stdint.h>
#include <unistd.h>

#include "exception.h"
#include "i2c.h"
#include "geometry.h"
#include "gyroscope.h"

// L3G4200D Register Addresses
#define CTRL_REG1   0x20
#define CTRL_REG2   0x21
#define CTRL_REG3   0x22
#define CTRL_REG4   0x23
#define CTRL_REG5   0x24

#define OUT_X_L     0x28
#define OUT_X_H     0x29
#define OUT_Y_L     0x2A
#define OUT_Y_H     0x2B
#define OUT_Z_L     0x2C
#define OUT_Z_H     0x2D

#define AUTO_INCR   0x80 // bitwise-or w/ register addr to use auto increment

Gyroscope::Gyroscope(I2C *i2c, uint8_t slaveaddr, Range range,
		SampleRate rate) {
	mI2C = i2c;
	mSlaveAddr = slaveaddr;
	mRange = range;
	mRate = rate;
	mSleep = false;

	setRange(mRange);
	setSleepAndRate();
}

Gyroscope::~Gyroscope() {
	try {
		char buffer[2];
		buffer[0] = CTRL_REG1;
		buffer[1] = 0b00000000; // Power Down enable (bit 3)
		mI2C->write(mSlaveAddr, buffer, 2);
	} catch (...)
		{ /* Don't care... just make sure destructor finishes */ }
}

void Gyroscope::setSleep(bool sleep) {
	mSleep = sleep;
	setSleepAndRate();
}

void Gyroscope::setRange(Range range) {
	mRange = range;

	char buffer[2];
	buffer[0] = CTRL_REG4;
	buffer[1] = 0b00000000 | mRange << 4;
			// 0b01**0000 :
			// 00 = 250dps, 01 = 500dps,
			// 10 = 11 = 2000dps
			// Big-Endian (bit 6)
	mI2C->write(mSlaveAddr, buffer, 2);
}

void Gyroscope::setSampleRate(SampleRate rate) {
	mRate = rate;
	setSleepAndRate();
}

Vector3<float> Gyroscope::read() {
	Vector3<float> vector;
	int16_t values[3];
	char buffer = OUT_X_L | AUTO_INCR;

	mI2C->enqueueWrite(mSlaveAddr, &buffer, 1);
	mI2C->enqueueRead(mSlaveAddr, values, 6);
	mI2C->sendTransaction();

	float factor;
	switch (mRange) {
		case RANGE_250DPS:
			factor = 0.00875f;
			break;
		case RANGE_500DPS:
			factor = 0.0175f;
			break;
		case RANGE_2000DPS:
			factor = 0.07f;
			break;
	}
	vector.x = factor * (int)values[0];
	vector.y = factor * (int)values[1];
	vector.z = factor * (int)values[2];

	return vector;
}

void Gyroscope::setSleepAndRate() {
	char buffer[2];
	buffer[0] = CTRL_REG1;

	if (mSleep)
		// Sleep (Power Down disable (bit 3) and XYZ disable (bits 0-2))
		buffer[1] = 0b00001000;
	else
		// Measure (PD disable (bit 3) and XYZ enable (bits 0-2))
		// Sample Rate bits 6-7
		buffer[1] = 0b00001111 | (mRate << 6);

	mI2C->write(mSlaveAddr, buffer, 2);
}

