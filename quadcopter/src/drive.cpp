/*
	drive.cpp

	Drive class - controls the motors of a quadcopter for stabilization and
		motion. Supports translational and rotational (yaw) motion.
*/

// temporary for debugging
#include <stdio.h>

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <fstream>
#include <limits>

#include "exception.h"
#include "pwm.h"
#include "motor.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "geometry.h"
#include "drive.h"

Drive::Drive(PWM *pwm, Accelerometer *accel, Gyroscope *gyro, int frontleft,
		int frontright, int rearright, int rearleft, int smoothing) {
	mAccelerometer = accel;
	mGyroscope = gyro;

	mSmoothing = smoothing;
	mAccelValue = new Vector3<float>[mSmoothing];
	for (int i = 0; i < mSmoothing; ++i) {
		mAccelValue[i].x = 0.0f;
		mAccelValue[i].y = 0.0f;
		mAccelValue[i].z = 0.0f;
	}
	mGyroValue = new Vector3<float>[mSmoothing];
	for (int i = 0; i < mSmoothing; ++i) {
		mGyroValue[i].x = 0.0f;
		mGyroValue[i].y = 0.0f;
		mGyroValue[i].z = 0.0f;
	}

	mMotors[0] = new Motor(pwm, frontleft, 1.27f, 1.6f);
	mMotors[1] = new Motor(pwm, frontright, 1.27f, 1.6f);
	mMotors[2] = new Motor(pwm, rearright, 1.27f, 1.6f);
	mMotors[3] = new Motor(pwm, rearleft, 1.27f, 1.6f);

	mAccelOffset.x = 0.0f;
	mAccelOffset.y = 0.0f;
	mAccelOffset.z = 0.0f;
	mGyroOffset.x = 0.0f;
	mGyroOffset.y = 0.0f;
	mGyroOffset.z = 0.0f;
	loadCalibration("calibration.ini");

	// Wait for motors to prime
	usleep(3000000);

	mRoll = 0.0f;
	mPitch = 0.0f;
	mYaw = 0.0f;

	// Pre-populate mAccelValue and mGyroValue arrays
	Vector3<float> aval;
	Vector3<float> gval;
	for (int i = 0; i < mSmoothing; ++i) {
		aval = mAccelerometer->read();
		gval = mGyroscope->read();
		mAccelValue[i] = aval;
		mGyroValue[i] = gval;
		usleep(10000); // 10,000us = 100Hz
	}
	mAccelValueCurrent = 0;
	mGyroValueCurrent = 0;

	calculateOrientation();

	for (int i = 0; i < 4; ++i)
		mMotors[i]->setSpeed(0.0f);
}

Drive::~Drive() {
	delete[] mAccelValue;
	delete[] mGyroValue;

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
	updateSensors();
	calculateOrientation();

	/*
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
	*/
}

void Drive::stop() {
	for (int i = 0; i < 4; ++i)
		mMotors[i]->setSpeed(0.0f);
}

float Drive::getRoll() {
	return mRoll;
}

float Drive::getPitch() {
	return mPitch;
}

float Drive::getYaw() {
	return mYaw;
}

void Drive::calibrate(unsigned int millis) {
	Vector3<float> accel_total;
	Vector3<float> gyro_total;
	unsigned int elapsed = 0,
	             num_iterations = 0;

	accel_total.x = 0.0f; accel_total.y = 0.0f; accel_total.z = 0.0f;
	gyro_total.x = 0.0f; gyro_total.y = 0.0f; gyro_total.z = 0.0f;

	while (elapsed < millis) {
		accel_total += mAccelerometer->read();
		gyro_total += mGyroscope->read();
		usleep(10000);
		elapsed += 10;
		++num_iterations;
	}

	mAccelOffset.x = accel_total.x / num_iterations;
	mAccelOffset.y = accel_total.y / num_iterations;
	mAccelOffset.z = accel_total.z / num_iterations;
	mGyroOffset.x = gyro_total.x / num_iterations;
	mGyroOffset.y = gyro_total.y / num_iterations;
	mGyroOffset.z = gyro_total.z / num_iterations;

	saveCalibration("calibration.ini");
}

/*
	Private member functions
*/

void Drive::updateSensors() {
	++mAccelValueCurrent;
	if (mAccelValueCurrent >= mSmoothing)
		mAccelValueCurrent = 0;
	mAccelValue[mAccelValueCurrent] = mAccelerometer->read();

	++mGyroValueCurrent;
	if (mGyroValueCurrent >= mSmoothing)
		mGyroValueCurrent = 0;
	mGyroValue[mGyroValueCurrent] = mGyroscope->read();
}

void Drive::calculateOrientation() {
	Vector3<float> accel = averageAccelerometer();
	Vector3<float> gyro = averageGyroscope();

	mRoll = atan2(-accel.z, accel.x);
	mPitch = atan2(-accel.z, accel.y);

	mYaw = 0.0f;
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

Vector3<float> Drive::averageGyroscope() {
	Vector3<float> avg;
	for (int i = 0; i < mSmoothing; ++i) {
		avg.x += mGyroValue[i].x;
		avg.y += mGyroValue[i].y;
		avg.z += mGyroValue[i].z;
	}
	avg.x /= mSmoothing;
	avg.y /= mSmoothing;
	avg.z /= mSmoothing;

	return avg;
}

void Drive::loadCalibration(const std::string &filename) {
	std::ifstream file(filename.c_str(), std::ios_base::in);
	if (file.fail())
		THROW_EXCEPT(CalibrationException,
				"Calibration file (" + filename + ") could not be loaded");

	char  key[20];
	float value;
	while (!file.eof()) {
		file.getline(key, 20, '=');

		if (!file.fail()) {
			file >> value;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (strcmp(key, "AccelX") == 0) mAccelOffset.x = value;
			else if (strcmp(key, "AccelY") == 0) mAccelOffset.y = value;
			else if (strcmp(key, "AccelZ") == 0) mAccelOffset.z = value;
			else if (strcmp(key, "GyroX") == 0) mGyroOffset.x = value;
			else if (strcmp(key, "GyroY") == 0) mGyroOffset.y = value;
			else if (strcmp(key, "GyroZ") == 0) mGyroOffset.z = value;
		}
	}
}

void Drive::saveCalibration(const std::string &filename) {
	std::ofstream file(filename.c_str(),
			std::ios_base::out | std::ios_base::trunc);
	if (file.fail())
		THROW_EXCEPT(CalibrationException,
				"Calibration file (" + filename + ") could not be written");

	file << "AccelX=" << mAccelOffset.x << std::endl
	     << "AccelY=" << mAccelOffset.y << std::endl
	     << "AccelZ=" << mAccelOffset.z << std::endl
	     << "GyroX=" << mGyroOffset.x << std::endl
	     << "GyroY=" << mGyroOffset.y << std::endl
	     << "GyroZ=" << mGyroOffset.z << std::endl;

	file.close();
}

