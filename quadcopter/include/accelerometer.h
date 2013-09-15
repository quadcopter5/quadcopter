/*
	accelerometer.h

	Accelerometer class - interface for the ADXL345 (on GY80 board)

	Allows for modifying the sleep state of the ADXL345 (for reduced power
	consumption), setting configuration states, and reading values.
*/

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#ifndef __cplusplus
#error This header requires C++
#endif

#include <stdint.h>

#include "exception.h"
#include "i2c.h"
#include "geometry.h"

class Accelerometer {
	public:
		enum Range {
			RANGE_2G = 0,
			RANGE_4G = 1,
			RANGE_8G = 2,
			RANGE_16G = 3
		};

		enum SampleRate {
			SRATE_12_5HZ = 7,
			SRATE_25HZ = 8,
			SRATE_50HZ = 9,
			SRATE_100HZ = 10,
			SRATE_200HZ = 11,
			SRATE_400HZ = 12
		};

		/**
			Constructor

			Creates an ADXL345 interface via the given (already instantiated)
			I2C interface. Throughout the lifetime of the object, the given
			slave address is used for communication.

			The class is NOT responsible for destroying i2c! This must be done
			by the user of this class.

			Also, the user must ensure that the I2C object remains valid for as
			long as this I2C_PWM object is used.

				i2c       : the I2C connection to communicate through
				slaveaddr : I2C slave address of the ADXL345
				range     : configuration for the range of forces that the
				            ADXL345 will handle before output values saturate
							(e.g. RANGE_4G will cause the minimum output value
							to represent -4G and the max output value +4G
				rate      : configuration for the sampling frequency. This
				            determines often the ADXL345 outputs new data.
							This can also affect power consumption.

			After construction, the accelerometer is put into normal mode
			(not sleeping).

			Throws I2CException if I2C communication fails.
		*/
		Accelerometer(I2C *i2c, uint8_t slaveaddr,
				Range range = RANGE_2G,
				SampleRate rate = SRATE_25HZ);

		/**
			Destructor
		*/
		~Accelerometer();

		/**
			Set the mode of the ADXL345.

				sleep : true  = sleep mode
				                Reduced power consumption; sample rate is much
				                less frequent
				        false = normal mode
				                Sample rate is as specified

			Throws I2CException if I2C communication fails.
		*/
		void setSleep(bool sleep);

		/**
			Calibrates the accelerometer. This adjusts offsets for each axis
			such that an actual force of 1G will result in an output of 1G.

			Note: At the time of calling this function, the accelerometer MUST
			be still and in the upright position, such that the z-axis
			is experiencing 1G and the x- and y-axes are experiencing 0G.

			Throws I2CException if I2C communication fails.
		*/
//		void calibrate();

		/**
			Set the output range.

			Throws I2CException if I2C communication fails.
		*/
		void setRange(Range range);

		/**
			Set the sampling frequency, which determines how often the
			accelerometer will update the output values (obtained using read())

			A higher sample rate generally consumes more power (may not be
			significant)

			Throws I2CException if I2C communication fails.
		*/
		void setSampleRate(SampleRate rate);

		/**
			Read the current output of the accelerometer.

			The value returned is a Vector3 containing float values for each
			component of motion (x, y, z). The value represents the amount of
			force in Gs (1.0f = 1G = the magnitude of force exerted by gravity
			on a non-moving object)

			Throws I2CException if I2C communication fails.
		*/
		Vector3<float> read();

	private:
		I2C     *mI2C;
		uint8_t mSlaveAddr;

		Range   mRange;
};

#endif

