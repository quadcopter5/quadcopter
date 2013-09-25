/*
	drive.h

	Drive class - controls the motors of a quadcopter.
*/

#include "exception.h"
#include "pwm.h"
#include "motor.h"
#include "accelerometer.h"
#include "geometry.h"
#include "drive.h"

Drive::Drive(PWM *pwm, Accelerometer *accel, int frontleft, int frontright,
		int rearright, int rearleft) {

}

Drive::~Drive() {

}

void move(Vector3<float> velocity) {
	mTranslate = velocity;
}

void turn(float speed) {
	mRotate = speed;
}

void update() {

}

