/*
	motor.cpp

	Motor class - abstraction to represent a single motor, controlled by PWM
		speed controllers.
*/

#include "exception.h"
#include "pwm.h"
#include "motor.h"

Motor::Motor(PWM *pwm, int channel, float min_hightime, float max_hightime) {
	mPWM = pwm;
	mChannel = channel;
	mMinHighTime = min_hightime;
	mMaxHighTime = max_hightime;
	mSpeed = 0.0f;

	if (!pwm)
		THROW_EXCEPT(PWMException, "Provided PWM is not valid");

	setSpeed(mSpeed);
}

Motor::~Motor() {
	setSpeed(0.0f);
}

void Motor::setSpeed(float speed) {
	mSpeed = speed;
	float hightime = mMinHighTime + (mMaxHighTime - mMinHighTime) * mSpeed;
	mPWM->setHighTime(mChannel, hightime);
}

float Motor::getSpeed() {
	return mSpeed;
}

