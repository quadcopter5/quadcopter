/*
	drive.h

	Drive class - controls the motors of a quadcopter for stabilization and
		motion. Supports translational and rotational (yaw) motion.
*/

// temporary for debugging
#include <stdio.h>

#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "exception.h"
#include "pwm.h"
#include "motor.h"
#include "accelerometer.h"
#include "geometry.h"
#include "drive.h"

Drive::Drive(PWM *pwm, Accelerometer *accel, int frontleft, int frontright,
		int rearright, int rearleft, int smoothing) {
	mAccelerometer = accel;

	mSmoothing = smoothing;
	mAccelValue = new Vector3<float>[mSmoothing];
	for (int i = 0; i < mSmoothing; ++i) {
		mAccelValue[i].x = 0.0f;
		mAccelValue[i].y = 0.0f;
		mAccelValue[i].z = 0.0f;
	}

	mMotors[0] = new Motor(pwm, frontleft, 1.27f, 1.6f);
	mMotors[1] = new Motor(pwm, frontright, 1.27f, 1.6f);
	mMotors[2] = new Motor(pwm, rearright, 1.27f, 1.6f);
	mMotors[3] = new Motor(pwm, rearleft, 1.27f, 1.6f);

	// Wait for motors to prime
	usleep(3000000);

	// Pre-populate mAccelValue array
	Vector3<float> val = mAccelerometer->read();
	for (int i = 0; i < mSmoothing; ++i)
		mAccelValue[i] = val;
	mAccelValueCurrent = 0;

	for (int i = 0; i < 4; ++i)
		mMotors[i]->setSpeed(0.0f);
}

Drive::~Drive() {
	delete[] mAccelValue;

	for (int i = 0; i < 4; ++i) {
		mMotors[i]->setSpeed(0.0f);
		delete mMotors[i];
	}
	usleep(100000);
}

void Drive::move(Vector3<float> velocity) {
	mTranslate = velocity;
}

void Drive::turn(float speed) {
	mRotate = speed;
}

void Drive::update() {
	updateAccelerometer();
	Vector3<float> accel = averageAccelerometer();

	Vector2<float> xz(accel.x, accel.z);
	Vector2<float> yz(accel.y, accel.z);

	double y_angle = asin(accel.x / magnitude(xz)) * 180.0 / PI;
	double x_angle = asin(accel.y / magnitude(yz)) * 180.0 / PI;

	float motorspeeds[4];
	for (int i = 0; i < 4; ++i)
		motorspeeds[i] = mTranslate.z;

	double d_ends  = sign(x_angle) * pow(x_angle / 90.0, 2.0) * 3.0;
	double d_sides = sign(y_angle) * pow(y_angle / 90.0, 2.0) * 3.0;

	motorspeeds[0] += -d_ends + d_sides;
	motorspeeds[1] += -d_ends - d_sides;
	motorspeeds[2] += d_ends - d_sides;
	motorspeeds[3] += d_ends + d_sides;

//	// Add z-component from translate
//	for (int i = 0; i < 4; ++i)
//		motorspeeds[i] += mTranslate.z;

	// Temporary disable front-left and rear-right motors
//	motorspeeds[0] = 
//	motorspeeds[2] = 0.0f;

	printf("d_ends = %.5f", d_ends);
	printf(" | d_sides = %.5f", d_sides);
	printf(" | Front left = %.5f", motorspeeds[0]);
	printf(" | Front right = %.5f", motorspeeds[1]);
	printf(" | Rear right = %.5f", motorspeeds[2]);
	printf(" | Rear left = %.5f\n", motorspeeds[3]);

	for (int i = 0; i < 4; ++i)
		mMotors[i]->setSpeed(motorspeeds[i]);
}

/*
	Private member functions
*/

void Drive::updateAccelerometer() {
	++mAccelValueCurrent;
	if (mAccelValueCurrent >= mSmoothing)
		mAccelValueCurrent = 0;

	mAccelValue[mAccelValueCurrent] = mAccelerometer->read();
}

Vector3<float> Drive::averageAccelerometer() {
	Vector3<float> avg;
	for (int i = 0; i < mSmoothing; ++i) {
		avg.x += mAccelValue[i].x;
		avg.y += mAccelValue[i].y;
		avg.z += mAccelValue[i].z;
	}
	avg.x /= mSmoothing;
	avg.y /= mSmoothing;
	avg.z /= mSmoothing;

	return avg;
}

