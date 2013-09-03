/*
	pwm.cpp

	PWM class - RaspberryPi PWM control interface via PCA9685 I2C
*/

#include <stdint.h>
#include <unistd.h>

#include "exception.h"
#include "i2c.h"
#include "pwm.h"

// PCA9685 Register Addresses

#define PCA9685_MODE1           0x00
#define PCA9685_MODE2           0x01
#define PCA9685_SUBADR1         0x02
#define PCA9685_SUBADR2         0x03
#define PCA9685_SUBADR3         0x04
#define PCA9685_ALLCALLADR      0x05

#define PCA9685_LED0_ON_L       0x06
#define PCA9685_LED0_ON_H       0x07
#define PCA9685_LED0_OFF_L      0x08
#define PCA9685_LED0_OFF_H      0x09

#define PCA9685_LED1_ON_L       0x0A
#define PCA9685_LED1_ON_H       0x0B
#define PCA9685_LED1_OFF_L      0x0C
#define PCA9685_LED1_OFF_H      0x0D

#define PCA9685_LED2_ON_L       0x0E
#define PCA9685_LED2_ON_H       0x0F
#define PCA9685_LED2_OFF_L      0x10
#define PCA9685_LED2_OFF_H      0x11

#define PCA9685_LED3_ON_L       0x12
#define PCA9685_LED3_ON_H       0x13
#define PCA9685_LED3_OFF_L      0x14
#define PCA9685_LED3_OFF_H      0x15

#define PCA9685_LED4_ON_L       0x16
#define PCA9685_LED4_ON_H       0x17
#define PCA9685_LED4_OFF_L      0x18
#define PCA9685_LED4_OFF_H      0x19

#define PCA9685_LED5_ON_L       0x1A
#define PCA9685_LED5_ON_H       0x1B
#define PCA9685_LED5_OFF_L      0x1C
#define PCA9685_LED5_OFF_H      0x1D

#define PCA9685_LED6_ON_L       0x1E
#define PCA9685_LED6_ON_H       0x1F
#define PCA9685_LED6_OFF_L      0x20
#define PCA9685_LED6_OFF_H      0x21

#define PCA9685_LED7_ON_L       0x22
#define PCA9685_LED7_ON_H       0x23
#define PCA9685_LED7_OFF_L      0x24
#define PCA9685_LED7_OFF_H      0x25

#define PCA9685_LED8_ON_L       0x26
#define PCA9685_LED8_ON_H       0x27
#define PCA9685_LED8_OFF_L      0x28
#define PCA9685_LED8_OFF_H      0x29

#define PCA9685_LED9_ON_L       0x2A
#define PCA9685_LED9_ON_H       0x2B
#define PCA9685_LED9_OFF_L      0x2C
#define PCA9685_LED9_OFF_H      0x2D

#define PCA9685_LED10_ON_L      0x2E
#define PCA9685_LED10_ON_H      0x2F
#define PCA9685_LED10_OFF_L     0x30
#define PCA9685_LED10_OFF_H     0x31

#define PCA9685_LED11_ON_L      0x32
#define PCA9685_LED11_ON_H      0x33
#define PCA9685_LED11_OFF_L     0x34
#define PCA9685_LED11_OFF_H     0x35

#define PCA9685_LED12_ON_L      0x36
#define PCA9685_LED12_ON_H      0x37
#define PCA9685_LED12_OFF_L     0x38
#define PCA9685_LED12_OFF_H     0x39

#define PCA9685_LED13_ON_L      0x3A
#define PCA9685_LED13_ON_H      0x3B
#define PCA9685_LED13_OFF_L     0x3C
#define PCA9685_LED13_OFF_H     0x3D

#define PCA9685_LED14_ON_L      0x3E
#define PCA9685_LED14_ON_H      0x3F
#define PCA9685_LED14_OFF_L     0x40
#define PCA9685_LED14_OFF_H     0x41

#define PCA9685_LED15_ON_L      0x42
#define PCA9685_LED15_ON_H      0x43
#define PCA9685_LED15_OFF_L     0x44
#define PCA9685_LED15_OFF_H     0x45

#define PCA9685_ALL_LED_ON_L    0xFA
#define PCA9685_ALL_LED_ON_H    0xFB
#define PCA9685_ALL_LED_OFF_L   0xFC
#define PCA9685_ALL_LED_OFF_H   0xFD

#define PCA9685_PRE_SCALE       0xFE

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

	setFrequency(20);
}

PWM::~PWM() {
	// Put into sleep mode, maybe?
	char buffer[2];
	int bufsize = 2;
	buffer[0] = 0b00000000;
	buffer[1] = 0b00110000; // Auto-Increment (bit 5) | Sleep (bit 4)

	i2c->write(mSlaveAddr, buffer, bufsize);
}

void PWM::setFrequency(unsigned int hertz) {
	char buffer[8];
	int  bufsize;

	// Put to sleep before modifying PRE_SCALE
	bufsize = 2;
	buffer[0] = 0b00000000;
	buffer[1] = 0b00110000; // Auto-Increment (bit 5) | Sleep (bit 4)

	i2c->write(mSlaveAddr, buffer, bufsize);

	usleep(1000);

	bufsize = 4;
	buffer[0] = 0b11111110; // PRE_SCALE
	buffer[1] = (25000000 / (4096 * hertz)) - 1;
//	buffer[1] = 0x0079;     // (25000000Hz / (4096 * freq)) - 1
//	                        // freq = 50Hz, prescale = 121 = 0x79
	buffer[2] = 0b00000000; // MODE1
	buffer[3] = 0b00100000; // Auto-Increment | Not Sleep
	i2c->write(mSlaveAddr, buffer, bufsize);

	// Must be not sleeping for at least 500us before writing to Reset bit
	usleep(1000);

	bufsize = 2;
	buffer[0] = 0b00000000;
	buffer[1] = 0b10100000; // Reset | Auto-Increment
	i2c->write(mSlaveAddr, buffer, bufsize);

	usleep(1000);
}

void setLoad(unsigned int channel, float factor) {

}

void setHighTime(unsigned int channel, float millis) {

}
