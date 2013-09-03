/*
	pwm.h

	PWM class - RaspberryPi PWM control interface via PCA9685 I2C
*/

#ifndef PWM_H
#define PWM_H

#ifndef __cplusplus
#error This header requires C++
#endif

#include <stdint.h>

#include "exception.h"
#include "i2c.h"

class PWMException : public Exception {
	public:
		PWMException(const std::string &msg, const std::string &file, int line)
				: Exception(msg, file, line) { }
};

class PWM {
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
		PWM(I2C *i2c, uint8_t slaveaddr);

		/**
			Destructor
		*/
		~PWM();

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

			factor values outside of this range will be implicitly clipped to the
			outer values of the range.

			Throws PWMException if channel is not a valid channel number.
		*/
		void setLoad(unsigned int channel, float factor);

		/**
			Set PWM load, based on a desired amount of time in HIGH state.

			This function will attempt to set the amount of time in HIGH state
			during each PWM cycle, independent of the PWM frequency selected.

			The amount of time will be implicitly clipped to the maximum number of
			milliseconds in each PWM cycle.

			Throws PWMException if channel is not a valid channel number.
		*/
		void setHighTime(unsigned int channel, float millis);

	private:
		I2C     *mI2C;
		uint8_t mSlaveAddr;

		unsigned int mFrequency; // in Hz
};

#endif

