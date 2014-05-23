/*
	pwm.h

	PWM class - RaspberryPi PWM control interface via PCA9685 I2C

	A single PWM instance for each PCA9685 should exist throughout the entire
	time that PWM control is needed. The hardware module is taken out of sleep
	mode when the PWM object is instantiated, and it is put into sleep mode when
	the object is destructed.
	
	Note that PWM output continues while the PCA9685 sleeps; however, the output
	cannot be modified.
*/

#ifndef PWM_H
#define PWM_H

#ifndef __cplusplus
#error This header requires C++
#endif

#include <stdint.h>
#include <sys/time.h>

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

			Creates an I2C PWM interface via the given (already instantiated)
			I2C interface. Throughout the lifetime of the object, the given
			slave address is used for communication.

			The class is NOT responsible for destroying i2c! This must be done
			by the user of this class.

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

			Throws I2CException if I2C communication fails.
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

			factor values outside of this range will be implicitly clipped to
			the outer values of the range.

			Throws PWMException if channel is not a valid channel number.
			       I2CException if I2C communication fails.
		*/
		void setLoad(unsigned int channel, float factor);

		/**
			Set PWM load, based on a desired amount of time in HIGH state.

			This function will attempt to set the amount of time in HIGH state
			during each PWM cycle, independent of the PWM frequency selected.

			The amount of time will be implicitly clipped to the maximum number
			of milliseconds in each PWM cycle.

			Throws PWMException if channel is not a valid channel number.
			       I2CException if I2C communication fails.
		*/
		void setHighTime(unsigned int channel, float millis);

		/**
			Set the PWM load using the same precision as the PCA9685. count
			is an integer value in the inclusive range [0,4095]. Any values
			higher than 4095 will be clipped to 4095.
		*/
		void setExactLoad(unsigned int channel, uint16_t count);

		/**
			Update dithering for the PWM signal on the given channel.

			If dither is not desired, simply don't call this function.
		*/
		void update(unsigned int channel);

		/**
			Set the sleep mode for the PCA9685 (sleep mode consumes less power,
			but does not output any PWM signals).

			enabled
			   true  : sleep mode enabled
			   false : sleep mode disabled (running normally)
		*/
		void setSleep(bool enabled);

	private:
		I2C     *mI2C;
		uint8_t mSlaveAddr;

		unsigned int mFrequency; // in Hz
		float    mLoad[16];
//		uint16_t mLoCount[16]; // The low value for the dither
		uint16_t mCount[16]; // The count out of 4095 to switch from ON to OFF

		struct timeval mFrameStart; // start time of the current dither frame
		long mFrameLength; // length of dithering frame, in microseconds

		/**
			Update mFrameLength according to mFrequency, and reset the phase of
			the dithering frame (i.e. set mFrameStart to current time).
		*/
		void resetFrame();
};

#endif

