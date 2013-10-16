/*
	drive.h

	Drive class - controls the motors of a quadcopter for stabilization and
		motion. Supports translational and rotational (yaw) motion.
*/

// temporary for debugging
#include <iostream>

#include <math.h>
#include <unistd.h>

#include "exception.h"
#include "pwm.h"
#include "motor.h"
#include "accelerometer.h"
#include "geometry.h"
#include "drive.h"

Drive::Drive(PWM *pwm, Accelerometer *accel, int frontleft, int frontright,
		int rearright, int rearleft) {
	mAccelerometer = accel;

	mMotors[0] = new Motor(pwm, frontleft, 1.27f, 1.5f);
	mMotors[1] = new Motor(pwm, frontright, 1.27f, 1.5f);
	mMotors[2] = new Motor(pwm, rearright, 1.27f, 1.5f);
	mMotors[3] = new Motor(pwm, rearleft, 1.27f, 1.5f);

	usleep(3000000);

	for (int i = 0; i < 4; ++i)
		mMotors[i]->setSpeed(0.0f);
}

Drive::~Drive() {
	for (int i = 0; i < 4; ++i) {
		mMotors[i]->setSpeed(0.0f);
		delete mMotors[i];
	}
}

void Drive::move(Vector3<float> velocity) {
	mTranslate = velocity;
}

void Drive::turn(float speed) {
	mRotate = speed;
}

void Drive::update() {
	mAccelValue = mAccelerometer->read();

	Vector2<float> xz(mAccelValue.x, mAccelValue.z);
	Vector2<float> yz(mAccelValue.y, mAccelValue.z);

	double y_angle = asin(mAccelValue.x / magnitude(xz)) * 180.0 / PI;
	double x_angle = asin(mAccelValue.y / magnitude(yz)) * 180.0 / PI;

	float motorspeeds[4];
	for (int i = 0; i < 4; ++i)
		motorspeeds[i] = 0.5f;

	double d_ends  = (x_angle / 90.0) * 0.3;
	double d_sides = (y_angle / 90.0) * 0.3;

	motorspeeds[0] += -d_ends + d_sides;
	motorspeeds[1] += -d_ends - d_sides;
	motorspeeds[2] += d_ends - d_sides;
	motorspeeds[3] += d_ends + d_sides;

	// Temporary disable front-left and rear-right motors
	motorspeeds[0] = 
	motorspeeds[2] = 0.0f;

	// Add z-component from translate
	for (int i = 0; i < 4; ++i)
		motorspeeds[i] += mTranslate.z;

	std::cout << "Front left = " << motorspeeds[0] << std::endl;
	std::cout << "Front right = " << motorspeeds[1] << std::endl;
	std::cout << "Rear right = " << motorspeeds[2] << std::endl;
	std::cout << "Rear left = " << motorspeeds[3] << std::endl;
	std::cout << std::endl;

	for (int i = 0; i < 4; ++i)
		mMotors[i]->setSpeed(motorspeeds[i]);
}

