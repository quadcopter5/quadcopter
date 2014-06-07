/*
	motor.h

	Motor class - abstraction to represent a single motor, controlled by a PWM
		speed controller.

	The constructor automatically sends a PWM signal for priming. The motor
	controller will beep when it is ready to start.
	
	For Hobbywing Flyfun 18A motor controllers:
	   min_hightime to 1.270ms for no motion
	   max_hightime to greatest desired value
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
			will cause min_hightime ms to be used as the PWM signal, while a
			value of 1.0f will cause max_hightime ms to be used.
			
			This means that min_hightime should refer to a stopping signal, and
			max_hightime should be the highest allowed speed for the electrical
			system.

			The constructor will set the PWM output to a priming signal. You
			should wait at least 3 seconds after the constructor is called
			before counting on the motors to respond to setSpeed().

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

			Providing a negative value will set the motor to priming mode.

			Throws I2CException.
		*/
		void setSpeed(float speed);

		/**
			Get the current speed of the motor. This is only handled through
			software, so this is strictly the last value passed to setSpeed().

			If this is called before calling setSpeed(), then the value returned
			will be negative, indicating that the motor is in the priming state.
		*/
		float getSpeed();

		/**
			Update the signal sent to the motor. This is important for
			maintaining a more precise speed. Because the PWM controller only
			receives quantized values, this dithers the signal in order to
			simulate intermediate values.

			The effectiveness of the dither is dependent on the update rate
			(how often this function is called).

			PCA9685 PWM controller and Hobbywing Flyfun 18A motor controller
			have noticeable steps when jumping between quantized values.

			Throws PWMException if this Motor's channel is invalid
			       I2CException if I2C communication fails.
		*/
		void update();

	private:
		PWM   *mPWM;
		int   mChannel;
		float mMinHighTime,
		      mMaxHighTime,
		      mSpeed;
};

#endif

