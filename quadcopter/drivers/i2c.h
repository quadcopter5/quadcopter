/*
	8/14/2013
	RaspberryPi I2C interface
	PWM control interface via PCA9685 I2C
*/

#ifndef I2C_H
#define I2C_H

#ifndef __cplusplus__
#error This header requires C++
#endif

#include <string>

class I2C {
	public:
		/**
			Constructor
			name is the name of the Linux I2C device interface,
			i.e. "/dev/i2c-X", where X is a number

			If there is no such device, append the following to /etc/modules:

			i2c-bcm2708
			i2c-dev

			...and reboot.
		*/
		I2C(const std::string &name);

		/**
			Copy constructor
			Makes this object refer to the same I2C interface as the other. The
			two objects will be interchangeable.
		*/
		I2C(const I2C &other);

		/**
			Destructor
		*/
		~I2C();

		/**
		  Copy assignment
		  Performs the same function as copy constructor
		*/
		I2C &operator=(const I2C &other);

		void write(

	private:
		std::string mFilename;
}

#endif

