/*
	pwm.cpp

	PWM class - RaspberryPi PWM control interface via PCA9685 I2C
*/

#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

#include "exception.h"
#include "i2c.h"
#include "pwm.h"

// PCA9685 Register Addresses

#define MODE1           0x00
#define MODE2           0x01
#define SUBADR1         0x02
#define SUBADR2         0x03
#define SUBADR3         0x04
#define ALLCALLADR      0x05

#define LED0_ON_L       0x06
#define LED0_ON_H       0x07
#define LED0_OFF_L      0x08
#define LED0_OFF_H      0x09

#define ALL_LED_ON_L    0xFA
#define ALL_LED_ON_H    0xFB
#define ALL_LED_OFF_L   0xFC
#define ALL_LED_OFF_H   0xFD

#define PRE_SCALE       0xFE

// MODE1 bits
#define MODE1_RESTART  0x80
#define MODE1_EXTCLK   0x40
#define MODE1_AI       0x20
#define MODE1_SLEEP    0x10
#define MODE1_SUB1     0x08
#define MODE1_SUB2     0x04
#define MODE1_SUB3     0x02
#define MODE1_ALLCALL  0x01

// MODE2 bits
#define MODE2_INVRT    0x10
#define MODE2_OCH      0x08
#define MODE2_OUTDRV   0x04
#define MODE2_OUTNE1   0x02
#define MODE2_OUTNE0   0x01

PWM::PWM(I2C *i2c, uint8_t slaveaddr) {
	if (!i2c)
		THROW_EXCEPT(PWMException, "Invalid I2C object");

	mI2C = i2c;
	mSlaveAddr = slaveaddr;
	mFrequency = 20;

	setFrequency(mFrequency);
}

PWM::~PWM() {
	// Put into sleep mode
	// Removed this because putting the PCA9685 to sleep causes it to stop
	// outputting its PWM signals. Then the motors see no signal and start
	// beeping.
//	setSleep(true);
}

void PWM::setFrequency(unsigned int hertz) {
	if (hertz == 0)
		THROW_EXCEPT(PWMException, "Invalid frequency provided");

	char buffer[8];
	int  bufsize;

	mFrequency = hertz;

	// Put to sleep before modifying PRE_SCALE
	setSleep(true);

	usleep(1000);

	bufsize = 4;
	buffer[0] = PRE_SCALE;
	buffer[1] = (25000000 / (4096 * hertz)) - 1;
	buffer[2] = MODE1;
	buffer[3] = MODE1_AI; // Not Sleep
	mI2C->write(mSlaveAddr, buffer, bufsize);

	// Must be not sleeping for at least 500us before writing to Reset bit
	usleep(1000);

	bufsize = 2;
	buffer[0] = MODE1;
	buffer[1] = MODE1_RESTART | MODE1_AI;
	mI2C->write(mSlaveAddr, buffer, bufsize);

	usleep(1000);

	resetFrame();
}

void PWM::setLoad(unsigned int channel, float factor) {
	if (channel > 15)
		THROW_EXCEPT(PWMException, "Invalid PWM channel given");

	mLoad[channel] = factor;
	mCount[channel] = (uint16_t)(mLoad[channel] * 4095.0f);
	setExactLoad(channel, mCount[channel]);

	/*
	uint16_t offcount = (uint16_t)(mLoad * 4095.0f);

	// Clip value to boundaries
	if (offcount > 4095)
		offcount = 4095;
	else if (offcount < 0)
		offcount = 0;

	// Send I2C command
	char buffer[5];
	buffer[0] = LED0_ON_L + channel * 4;
	buffer[1] = 0x00; // LEDx_ON_L
	buffer[2] = 0x00; // LEDx_ON_H
	buffer[3] = (char)(offcount & 0x00FF); // LEDx_OFF_L
	buffer[4] = (char)((offcount & 0x0F00) >> 8); // LEDx_OFF_H

	mI2C->write(mSlaveAddr, buffer, 5);
	*/
}

void PWM::setHighTime(unsigned int channel, float millis) {
	if (channel > 15)
		THROW_EXCEPT(PWMException, "Invalid PWM channel given");

	float cycletime = 1000.0f / mFrequency; // Amt of time per cycle (ms)
	mLoad[channel] = millis / cycletime;
	mCount[channel] = (uint16_t)(mLoad[channel] * 4095.0f);

	setExactLoad(channel, mCount[channel]);

	/* This is handled by setExactLoad()
	// Clip value to boundaries
	if (millis > cycletime)
		millis = cycletime;
	else if (millis < 0.0f)
		millis = 0.0f;
	*/

	/*
	char buffer[5];
	buffer[0] = LED0_ON_L + channel * 4;
	buffer[1] = 0x00; // LEDx_ON_L
	buffer[2] = 0x00; // LEDx_ON_H
	buffer[3] = (char)(offcount & 0x00FF); // LEDx_OFF_L
	buffer[4] = (char)((offcount & 0x0F00) >> 8); // LEDx_OFF_H

	mI2C->write(mSlaveAddr, buffer, 5);
	*/
}

void PWM::setExactLoad(unsigned int channel, uint16_t count) {
	if (channel > 15)
		THROW_EXCEPT(PWMException, "Invalid PWM channel given");

	mCount[channel] = count;

	// Clip value to boundaries
	if (count > 4095)
		count = 4095;

	char buffer[5];
	buffer[0] = LED0_ON_L + channel * 4;
	buffer[1] = 0x00; // LEDx_ON_L
	buffer[2] = 0x00; // LEDx_ON_H
	buffer[3] = (char)(count & 0x00FF); // LEDx_OFF_L
	buffer[4] = (char)((count & 0x0F00) >> 8); // LEDx_OFF_H

	mI2C->write(mSlaveAddr, buffer, 5);
}

void PWM::update(unsigned int channel) {
	if (channel > 15)
		THROW_EXCEPT(PWMException, "Invalid PWM channel given");

	struct timeval current;
	gettimeofday(&current, NULL);

	float partial = mLoad[channel] * 4095.0f
			- (uint16_t)(mLoad[channel] * 4095.0f);
	long switch_time = partial * mFrameLength;

	uint16_t lo_value = (uint16_t)(mLoad[channel] * 4095.0f);

	long elapsed = (current.tv_sec - mFrameStart.tv_sec) * 1000000
			- current.tv_usec - mFrameStart.tv_usec;
	if (elapsed >= mFrameLength) {
		// Start of a new frame
		// Don't care too much about overflow in elapsed... it'll just skip a
		// single frame, and it would only happen if update() is not called for
		// a while
		long elapsed_start = elapsed - elapsed % mFrameLength;
		mFrameStart.tv_sec += elapsed_start / 1000000;
		mFrameStart.tv_usec += elapsed_start % 1000000;
		if (mFrameStart.tv_usec >= 1000000) {
			mFrameStart.tv_sec += mFrameStart.tv_usec / 1000000;
			mFrameStart.tv_usec = mFrameStart.tv_usec % 1000000;
		}
		elapsed -= elapsed_start;
	}

	if (elapsed < switch_time) {
		if (mCount[channel] != lo_value)
			setExactLoad(channel, lo_value);
	} else {
		if (mCount[channel] == lo_value)
			setExactLoad(channel, lo_value + 1);
	}
}

void PWM::setSleep(bool enabled) {
	char buffer[2];
	buffer[0] = MODE1;
	buffer[1] = MODE1_AI;
	if (enabled)
		buffer[1] |= MODE1_SLEEP;
	mI2C->write(mSlaveAddr, buffer, 2);
	resetFrame();
}

/*
	Private member functions
*/

void PWM::resetFrame() {
	gettimeofday(&mFrameStart, NULL);
	mFrameLength = 8 * 1000000 / mFrequency; // 8 intermediate values
}

