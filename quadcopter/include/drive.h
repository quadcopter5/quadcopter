/*
	drive.h

	Drive class - controls the motors of a quadcopter.

	Allows for stabilization through accelerometer input, translational motion,
	and rotational motion.

	To use the Drive class, call the move() and turn() functions to set the
	desired amount of translational and rotational motion (respectively).
	Then call update(), which will actually calculate and send appropriate
	speeds to each motor in order to achieve the desired motion.
*/

#ifndef DRIVE_H
#define DRIVE_H

#include <string>
#include <sys/time.h>

#include "exception.h"
#include "pwm.h"
#include "motor.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "geometry.h"
#include "pidcontroller.h"

class CalibrationException : public Exception {
	public:
		CalibrationException(const std::string &msg, const std::string &file,
				int line) : Exception(msg, file, line) { }
};

class Drive {
	public:
		/**
			Constructor

			Establishes four motors on the given channels, using the given PWM
			object. Each channel corresponds to the physical position of the
			motor it controls, as described by the argument name.

			The function will block for 3 seconds, waiting for motors to be
			primed.

			smoothing controls how many accelerometer frames are averaged
			(1 frame corresponds to a call to update())

			The constructor looks for a file named "calibration.ini". If it
			is found, the values contained are used as calibration for the
			sensors. Otherwise, no calibration is used (until a call to
			calibrate()).

			Throws PWMException and I2CException.
		*/
		Drive(PWM *pwm,
				Accelerometer *accel,
				Gyroscope *gyro,
				int frontleft,
				int frontright,
				int rearright,
				int rearleft,
				int smoothing = 3);

		/**
			Destructor
		*/
		~Drive();

		/**
			Move the quadcopter (translational motion).

			Set the amount to move in all three axes. For each component axis:
				-1.0 : full negative velocity
				 0.0 : no velocity in specified axis
				+1.0 : full positive velocity

			The translational vector is independent of the rotational velocity
			(i.e. turn())

			NOTE: Motors and logic are unaffected until update() is called.
		*/
		void move(Vector3<float> velocity);

		/**
			Turn the quadcopter (rotational motion).

			Set the speed to rotate, around z-axis (vertical).
				-1.0 : turns left at full speed
				 0.0 : no rotational motion
				+1.0 : turns right at full speed

			The rotational velocity is independent of the translation vector
			(i.e. move())

			NOTE: Motors and logic are unaffected until update() is called.
		*/
		void turn(float speed);

		/**
			Update the motor speeds, actually applying the values fed through
			move() and turn(). Only the last values sent to these functions are
			actually used.

			If move() or turn() is not called between subsequent update()'s, it
			will use the last inputted value.

			Throws I2CException.
		*/
		void update();

		/*
			Stops all motors immediately. Does not require a subsequent call to
			update(), unlike move() and turn().

			Throws I2CException.
		*/
		void stop();

		/*
			Returns the perceived roll angle, as of the last call to update().
			0 = upright
		*/
		float getRoll();

		/*
			Returns the perceived pitch angle, as of the last call to update().
			0 = upright
		*/
		float getPitch();

		/*
			Returns the perceived yaw angle, as of the last call to update().
			0 = initial angle
		*/
		float getYaw();

		/*
			Calibrate sensors. Reads sensors for the given number of
			milliseconds at 100Hz. Then, averages the readings and uses these
			values as the "zero position" of the sensors in future
			calculations.

			CAUTION: This function should only be called when the sensors are
			completely still (0 linear and rotational motion) and perfectly
			in the upright position.

			Upon completion, the calibrated values are saved to file
			"calibration.ini" to eliminate the need to recalibrate every time.

			Throws I2CException if sensors cannot be read.
			       CalibrationException if the calibration could not be saved
			       to file. This is probably not fatal, but the calibration
			       will not be remembered the next time the program runs.
		*/
		void calibrate(unsigned int millis = 200);

	private:
		Accelerometer *mAccelerometer;
		Gyroscope     *mGyroscope;

		/**
			Layout of motors by index:
			
			   (front)

			   0     1      Y
			     \ /        ^
			     / \        |
			   3     2      |--> X

			    (rear)

			This is set according to the constructor.
		*/
		Motor *mMotors[4];

		// The last inputted target motion values
		float          mRotate;
		Vector3<float> mTranslate;

		// Current perceived orientation
		float mRoll,
		      mPitch,
		      mYaw;

		// Target orientation (to achieve desired movement)
		float mTargetRoll,
		      mTargetPitch,
		      mTargetYaw;

		// One PID controller per axis of rotation
		PIDController *mPIDRoll,
		              *mPIDPitch,
		              *mPIDYaw;

		// How many frames of accelerometer values to average
		int mSmoothing;

		// Array holding the [mSmoothing] latest values read from the
		// accelerometer.
		// Most recent is located in mAccelValueCurrent (circular buffer) 
		Vector3<float> *mAccelValue;
		int mAccelValueCurrent;

		// Same idea as mAccelValue, but for the gyroscope.
		Vector3<float> *mGyroValue;
		int mGyroValueCurrent;

		// Offset values based on sensor calibration
		// These values should be SUBTRACTED from sensor readings to obtain
		// the "zero" value.
		Vector3<float> mAccelOffset;
		Vector3<float> mGyroOffset;

		// Time of the last update of orientation values
		struct timeval mLastUpdate;

		/**
			Update the sensor value buffers
		*/
		void updateSensors();

		/**
			Calculate orientation based on stored sensor values (i.e. call
			updateSensors() before using this). dtime is the change in time since
			the last call to this function.

			Stores results in mRoll, mPitch, and mYaw.
		*/
		void calculateOrientation(float dtime);

		/**
			Approach the desired orientation. Uses current values of
			orientation, as set by calculateOrientation(), to approach the
			desired orientation per mRoll/mPitch/mYaw.

			Adjusts motor speeds accordingly.
		*/
		void stabilize();

		/**
			Returns the average of the values in mAccelValue
		*/
		Vector3<float> averageAccelerometer();

		/**
			Returns the average of the values in mGyroValue
		*/
		Vector3<float> averageGyroscope();

		/**
			Save the values currently used as calibration to disk.

			File format:
				AccelX=###.###
				AccelY=###.###
				AccelZ=###.###
				GyroX=###.###
				GyroY=###.###
				GyroZ=###.###

			Lines may appear in any order, and numbers must be float
			format.

			Throws CalibrationException if the file could not be written.
		*/
		void saveCalibration(const std::string &filename);

		/**
			Loads the values from the calibration file into the calibration
			member variables.

			Throws CalibrationException if the file does not exist.
		*/
		void loadCalibration(const std::string &filename);
};

#endif

