/*
	gyroscope.h

	Gyroscope class - interface for the L3G4200D (on GY80 board)

	Allows for modifying the sleep state of the L3G4200D (for reduced power
	consumption), setting configuration states, and reading values.
*/

#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#ifndef __cplusplus
#error This header requires C++
#endif

#include <stdint.h>

#include "exception.h"
#include "i2c.h"
#include "geometry.h"

class Gyroscope {
	public:
		enum Range {
			RANGE_250DPS = 0,
			RANGE_500DPS = 1,
			RANGE_2000DPS = 2
		};

		enum SampleRate {
			SRATE_100HZ = 0,
			SRATE_200HZ = 1,
			SRATE_400HZ = 2,
			SRATE_800HZ = 3
		};

		/**
			Constructor

			Creates an L3G4200D interface via the given (already instantiated)
			I2C interface. Throughout the lifetime of the object, the given
			slave address is used for communication.

			The class is NOT responsible for destroying i2c! This must be done
			by the user of this class.

			Also, the user must ensure that the I2C object remains valid for as
			long as this Gyroscope object is used.

				i2c       : the I2C connection to communicate through
				slaveaddr : I2C slave address of the L3G4200D
				range     : configuration for the range of rotational velocities
				            that the L3G4200D will handle before output values
				            saturate
				rate      : configuration for the sampling frequency. This
				            determines how often the L3G4200D outputs new data.
				            This can also affect power consumption.

			After construction, the gyroscope is put into normal mode
			(not sleeping).

			Throws I2CException if I2C communication fails.
		*/
		Gyroscope(I2C *i2c, uint8_t slaveaddr,
				Range range = RANGE_250DPS,
				SampleRate rate = SRATE_100HZ);

		/**
			Destructor
			
			Puts the gyroscope into power down mode.
		*/
		~Gyroscope();

		/**
			Set the mode of the L3G4200D.

				sleep : true  = sleep mode
				                Reduced power consumption; sample rate is much
				                less frequent
				        false = normal mode
				                Sample rate is as specified

			Throws I2CException if I2C communication fails.
		*/
		void setSleep(bool sleep);

		/**
			Set the output range in degrees per second (dps). Must be a value
			from the Range enum.

			Throws I2CException if I2C communication fails.
		*/
		void setRange(Range range);

		/**
			Set the sampling frequency, which determines how often the
			gyroscope will update the output values (obtained using read())

			A higher sample rate generally consumes more power (may not be
			significant)

			Throws I2CException if I2C communication fails.
		*/
		void setSampleRate(SampleRate rate);

		/**
			Read the current output of the gyroscope.

			The value returned is a Vector3 containing float values for each
			component of rotation (x, y, z). The value represents the amount of
			dps rotation about the corresponding axis.

			Throws I2CException if I2C communication fails.
		*/
		Vector3<int> read();

	private:
		I2C     *mI2C;
		uint8_t mSlaveAddr;

		bool       mSleep;
		Range      mRange;
		SampleRate mRate;

		/**
			Set sleep mode and sample rate at same time (since they are part of
			the same hardware register). Uses values from mSleep and mRate.
		*/
		void setSleepAndRate();
};

#endif

