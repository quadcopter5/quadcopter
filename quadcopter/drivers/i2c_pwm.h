/*
	8/14/2013

	I2C_PWM class - RaspberryPi PWM control interface via PCA9685 I2C
*/

#ifndef I2C_PWM_H
#define I2C_PWM_H

#ifndef __cplusplus__
#error This header requires C++
#endif

#include "i2c.h"

class I2C_PWM {
	public:
		/**
			Constructor

			Creates an I2C PWM interface via the given (already instantiated) I2C
			interface.

			I2C_PWM is NOT responsible for destroying i2c! This must be done by
			the user of this class.
			
			Also, the user must ensure that the I2C object remains valid for as
			long as this I2C_PWM object is used.
		*/
		I2C_PWM(I2C *i2c);

		/**
			Destructor
		*/
		~I2C_PWM();

		/**
			Set PWM frequency in Hertz. This determines the period of the
			waveform.

			Per limitations of PCA9685, the frequency is the same across all PWM
			channels on the board.
		*/
		void setFrequency(unsigned int hertz);

		/**
			Set PWM load, based on a load factor.

			channel is the PWM channel which is to be set.
			Valid channels are 0 - 15.

			factor will be the amount of the waveform that is HIGH, i.e.
				factor = 0.0f  => No load
				factor = 0.5f  => Half load
				factor = 1.0f  => Full load
		*/
		void setLoad(unsigned int channel, float factor);

		/**
			Set PWM load, based on a desired amount of time in HIGH state.

			This function will attempt to set the amount of time in HIGH state
			during each PWM cycle, independent of the PWM frequency selected.
		*/
		void setHighTime(unsigned int channel, float millis);

	private:
		I2C *mI2C;
}

#endif

