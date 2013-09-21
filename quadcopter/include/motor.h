/*
	motor.cpp

	Motor class - abstraction to represent a single motor, controlled by a PWM
		speed controller.
*/

#ifndef MOTOR_H
#define MOTOR_H

#include "pwm.h"

class Motor {
	public:
		/**
			Constructor

			Establishes PWM motor control on the given channel, using the
			provided PWM interface.

				minHighTime : minimum of the PWM signal range. The value is the
				              number of milliseconds that the signal is high.
				maxHighTime : maximum of the PWM signal range.

			When setting the motor speed through setSpeed(), a value of 0.0f
			will cause min_hightime ms to be used as the PWM signal, while a value
			of 1.0f will cause max_hightime ms to be used.
			
			This means that min_hightime should refer to a stopping signal, and
			max_hightime should be the highest allowed speed for the electrical
			system.

			Throws PWMException and I2CException.
		*/
		Motor(PWM *pwm, int channel, float min_hightime, float max_hightime);

		/**
			Destructor
		*/
		~Motor();

		/**
			Set the speed of the motor.
				0.0f : no motion (stopped)
				1.0f : full calibrated speed
			 > 1.0f : faster than calibrated speed (not recommended)

			Throws I2CException.
		*/
		void setSpeed(float speed);

		/**
			Get the current speed of the motor. This is only handled through
			software, so this is strictly the last value passed to setSpeed().
		*/
		float getSpeed();

	private:
		PWM   *mPWM;
		int   mChannel;
		float mMinHighTime,
		      mMaxHighTime,
		      mSpeed;
};

#endif

