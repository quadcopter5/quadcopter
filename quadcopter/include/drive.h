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

#include "pwm.h"
#include "motor.h"
#include "accelerometer.h"
#include "geometry.h"

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

			Throws PWMException and I2CException.
		*/
		Drive(PWM *pwm,
				Accelerometer *accel,
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

	private:
		Accelerometer *mAccelerometer;

		/**
			Layout of motors by index:
			
			   (front)

			   0     1
			     \ /
			     / \
			   3     2

			    (rear)

			This is set according to the constructor.
		*/
		Motor *mMotors[4];

		// How many frames of accelerometer values to average
		int mSmoothing;

		// Array holding the [mSmoothing] latest values read from the
		// accelerometer.
		// Most recent is located in mAccelValueCurrent (circular buffer) 
		Vector3<float> *mAccelValue;
		int mAccelValueCurrent;

		// The last inputted values
		float          mRotate;
		Vector3<float> mTranslate;

		/**
			Update the mAccelValue array
		*/
		void updateAccelerometer();

		/**
			Returns the average of the values in mAccelValue
		*/
		Vector3<float> averageAccelerometer();
};

#endif

