/*
	drive.h

	Drive class - controls the motors of a quadcopter.

	Allows for stabilization through accelerometer input, translational motion,
	and rotational motion.

	To use the Drive class, call the move() and turn() functions to set the
	desired amount of translational and rotational motion (respectively).
	
	When constructing the Drive object, set the update rate to a value that
	matches the update rate of the sensors for best performance.

	The update routine can automatically run on a periodic schedule. Call
	startTimer() on a constructed Drive object and the update routine will
	begin. Call stopTimer() to stop automatic updating. It is recommended to
	use the automatic update routine so that the update routine can run
	synchronously with the sensor update rate.

	Don't create more than ONE Drive object. The automatic update routine depends
	on signals to maintain a consistent update rate, and instantiating multiple
	Drive objects will conflict with each other (one might be ignored entirely).

DEPRECATED:
	Then call update(), which will actually calculate and send appropriate
	speeds to each motor in order to achieve the desired motion.
*/

#ifndef DRIVE_H
#define DRIVE_H

#include <string>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

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

class DriveException : public Exception {
	public:
		DriveException(const std::string &msg, const std::string &file,
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

			update_rate is the rate at which the control system is updated, in
			frames/second (Hz). Ideally this should be synchronous with the
			update rate of the sensors and equate to a period of whole nanoseconds.

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
				int update_rate,
				int smoothing = 3);

		/**
			Destructor
		*/
		~Drive();

		/**
			Start the system timer that will send a signal to run the update
			routine. The update routine will run automatically on a schedule
			determined by the update rate passed to the Drive constructor.

			Throws DriveException if the timer could not be started.
		*/
		void startTimer();

		/**
			Stop the timer. The update routine will not run automatically.

			Not tested! Using this may or may not cause odd behavior, like not
			being able to restart the timer or crashing.

			Throws DriveException if the timer could not be stopped.
		*/
		void stopTimer();

		/**
			Returns true if the update is being automatically scheduled by the
			timer. This will be true after a call to startTimer() and false after
			stopTimer().
		*/
		bool isTimedUpdate();

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
		//void update();

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

		/**
			Set the coefficients for the Angle PID controller.

			Also resets any state previously accumulated by the Angle PID
			controller.
		*/
		void setPIDAngle(float p, float i, float d);

		/**
			Set the coefficients for the Rate PID controller.

			Also resets any state previously accumulated by the Rate PID
			controller.
		*/
		void setPIDRate(float p, float i, float d);

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

		/**
			Update the motor speeds, actually applying the values fed through
			move() and turn(). Only the last values sent to these functions are
			actually used.

			If move() or turn() is not called between subsequent update()'s, it
			will use the last inputted value.

			This function is automatically called by the update timer. Only use it
			if you are not using the timer (e.g. never called startTimer() or
			called stopTimer()).
		*/
		void update();

	private:
		
		friend void *updateThreadEntry(void *);
		friend void updateThreadExit(Drive *);

		Accelerometer *mAccelerometer;
		Gyroscope     *mGyroscope;

		// Update routine & signals
		timer_t          mTimerID;
		struct sigaction mTimerAction;
		struct sigevent  mTimerEvent;
		int       mUpdateRate;
		bool      mTimerEnabled;
		pthread_t mUpdateThread;

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

		/*
			Two PID controllers per axis of rotation
			This idea is taken from:
				blog.oscarliang.net/quadcopter-pid-explained-tuning/
			For each axis, there are 2 PIDs in series. The first in the series
			takes the angle of the system, while the second takes the angular
			rate. This means the first one sets the "desired rotational rate" and
			the second one tries to achieve that angular rate.

			This is similar to the cascaded PID system described on Wikipedia,
			except that the second in series takes a different attribute.
		*/
		PIDController // Angle PID (1st in series)
		              *mPIDRollAngle,
		              *mPIDPitchAngle,
		              *mPIDYawAngle,

						  // Rate PID (2nd in series)
		              *mPIDRollRate,
		              *mPIDPitchRate,
		              *mPIDYawRate;

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
			Update the sensor value buffers.

			Does not throw exceptions.
		*/
		void updateSensors();

		/**
			Calculate orientation based on stored sensor values (i.e. call
			updateSensors() before using this). dtime is the change in time since
			the last call to this function.

			Stores results in mRoll, mPitch, and mYaw.
		*/
		void calculateOrientation(float dtime, Vector3<float> accel,
				Vector3<float> gyro);

		/**
			Approach the desired orientation. Uses current values of
			orientation, as set by calculateOrientation(), to approach the
			desired orientation per mRoll/mPitch/mYaw.

			gyro should be the current, averaged gyroscope reading. This is to
			avoid recalculating the average.

			Adjusts motor speeds accordingly.
		*/
		void stabilize(Vector3<float> gyro);

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

