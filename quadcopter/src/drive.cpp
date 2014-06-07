/*
	drive.cpp

	Drive class - controls the motors of a quadcopter for stabilization and
		motion. Supports translational and rotational (yaw) motion.
*/

#include <iostream> // Remove this for production!

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <fstream>
#include <limits>
#include <sys/time.h>
#include <sys/syscall.h>
#include <signal.h>
#include <time.h>

#include <pthread.h>

#include "exception.h"
#include "pwm.h"
#include "motor.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "geometry.h"
#include "pidcontroller.h"
#include "drive.h"

// Linux headers don't seem to define this
#ifndef sigev_notify_thread_id
#define sigev_notify_thread_id _sigev_un._tid
#endif

// glibc doesn't define gettid
pid_t gettid() {
	return syscall(SYS_gettid);
}

/**
	Synchronous update routine. Simply calls update() on the Drive object
	passed through the siginfo value pointer
*/
static void timerUpdate(int signal, siginfo_t *info, void *context);

/**
	Entry point for the automatic update thread.

	drv is expected to be a pointer to the Drive class that this thread will
	be handling.
*/
void *updateThreadEntry(void *drv);

/**
	Exit the update thread. Only to be called within the update thread.
*/
void updateThreadExit(Drive *drv);

Drive::Drive(PWM *pwm, Accelerometer *accel, Gyroscope *gyro, int frontleft,
		int frontright, int rearright, int rearleft, int update_rate,
		int smoothing) {
	mAccelerometer = accel;
	mGyroscope = gyro;

	mUpdateRate = update_rate;
	mTimerID = 0;
	mTimerEnabled = false;

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

	mRotate = 0.0f;
	mTranslate.x = 0.0f;
	mTranslate.y = 0.0f;
	mTranslate.z = 0.0f;

	mRoll  = 0.0f;
	mPitch = 0.0f;
	mYaw   = 0.0f;

	mTargetRoll  = 0.0f;
	mTargetPitch = 0.0f;
	mTargetYaw   = 0.0f;

	mPIDRollAngle  = new PIDController(mTargetRoll,  0.00f, 0.00f, 0.00f, 5);
	mPIDPitchAngle = new PIDController(mTargetPitch, 0.00f, 0.00f, 0.00f, 5);
	mPIDYawAngle   = new PIDController(mTargetYaw,   0.00f, 0.00f, 0.00f, 5);
	mPIDRollRate   = new PIDController(0.0f, 0.00f, 0.00f, 0.00f, 5);
	mPIDPitchRate  = new PIDController(0.0f, 0.00f, 0.00f, 0.00f, 5);
	mPIDYawRate    = new PIDController(0.0f, 0.00f, 0.00f, 0.00f, 5);

	mMotors[0] = new Motor(pwm, frontleft, 1.26f, 1.6f);
	mMotors[1] = new Motor(pwm, frontright, 1.26f, 1.6f);
	mMotors[2] = new Motor(pwm, rearright, 1.26f, 1.6f);
	mMotors[3] = new Motor(pwm, rearleft, 1.26f, 1.6f);

	mAccelOffset.x = 0.0f;
	mAccelOffset.y = 0.0f;
	mAccelOffset.z = 0.0f;
	mGyroOffset.x = 0.0f;
	mGyroOffset.y = 0.0f;
	mGyroOffset.z = 0.0f;
	try {
		loadCalibration("calibration.ini");
	} catch (CalibrationException &e) {
		std::cout << "WARNING: Continuimg without calibration" << std::endl;
	}

	// Wait for motors to prime
	usleep(3000000);

	gettimeofday(&mLastUpdate, NULL);

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

	Vector3<float> accelAvg = averageAccelerometer();
	Vector3<float> gyroAvg = averageGyroscope();
	calculateOrientation(0.0f, accelAvg, gyroAvg);

	// Make sure the motors are resting to start
	stop();
}

Drive::~Drive() {
	try {
		stopTimer();
	} catch (Exception &e)
		{ /* Don't care. Can't do anything about it anyway */ }

	delete[] mAccelValue;
	delete[] mGyroValue;

	delete mPIDRollAngle;
	delete mPIDPitchAngle;
	delete mPIDYawAngle;
	delete mPIDRollRate;
	delete mPIDPitchRate;
	delete mPIDYawRate;

	stop();
	usleep(100000);
	for (int i = 0; i < 4; ++i)
		delete mMotors[i];
}

void Drive::startTimer() {
	if (!mTimerEnabled) {
		mTimerEnabled = true;
		if (pthread_create(&mUpdateThread, NULL, updateThreadEntry, this) != 0) {
			mTimerEnabled = false;
			THROW_EXCEPT(DriveException, "Failed to start update thread");
		}
	}

	/*
	// Establish signal handler
	mTimerAction.sa_sigaction = timerUpdate;
	mTimerAction.sa_flags = SA_SIGINFO;
	sigemptyset(&mTimerAction.sa_mask);

	if (sigaction(SIGALRM, &mTimerAction, NULL) != 0)
		THROW_EXCEPT(DriveException,
				"Could not set signal handler for update routine\n");

	// Create timer that will send signal
	if (mTimerID == 0) {
		mTimerEvent.sigev_notify           = SIGEV_THREAD_ID | SIGEV_SIGNAL;
		mTimerEvent.sigev_signo            = SIGALRM;
		mTimerEvent.sigev_value.sival_ptr  = this;
		mTimerEvent.sigev_notify_thread_id = gettid();

		if (timer_create(CLOCK_REALTIME, &mTimerEvent, &mTimerID) != 0)
			THROW_EXCEPT(DriveException, "Could not create timer");
	}

	// Set timer

	struct itimerspec time;
	time.it_interval.tv_sec  = (mUpdateRate == 1 ? 1 : 0);
	time.it_interval.tv_nsec =
			(mUpdateRate == 1 ? 0 : 1000000000 / mUpdateRate);
	time.it_value.tv_sec  = time.it_interval.tv_sec;
	time.it_value.tv_nsec = time.it_interval.tv_nsec;

	if (timer_settime(mTimerID, 0, &time, NULL) != 0)
		THROW_EXCEPT(DriveException, "Could not start timer");
	*/
}

void Drive::stopTimer() {
	if (mTimerEnabled) {
		mTimerEnabled = false;
		pthread_join(mUpdateThread, NULL);
	}

	/*
	if (mTimerID == 0)
		THROW_EXCEPT(DriveException, "Update timer not running");

	// Ignore signal
	mTimerAction.sa_handler = SIG_IGN;
	mTimerAction.sa_flags = 0;
	sigemptyset(&mTimerAction.sa_mask);
	if (sigaction(SIGALRM, &mTimerAction, NULL) != 0)
		THROW_EXCEPT(DriveException, "Could not disable signal handler\n");

	// Disarm timer
	struct itimerspec time;
	time.it_interval.tv_sec  = 0;
	time.it_interval.tv_nsec = 0;
	time.it_value.tv_sec     = 0;
	time.it_value.tv_nsec    = 0;
	if (timer_settime(mTimerID, 0, &time, NULL) != 0)
		THROW_EXCEPT(DriveException, "Could not stop timer");
	*/
}

bool Drive::isTimedUpdate() {
	return mTimerEnabled;
}

void Drive::move(Vector3<float> velocity) {
	mTranslate = velocity;

	mTargetRoll = -mTranslate.x;
	mTargetPitch = mTranslate.y;
}

void Drive::turn(float speed) {
	mRotate = speed;
}

void Drive::stop() {
	for (int i = 0; i < 4; ++i)
		mMotors[i]->setSpeed(0.0f);
}

float Drive::getRoll() {
	//return mPIDRoll->output();
	return mRoll;
}

float Drive::getPitch() {
	//return mPIDPitch->output();
	return mPitch;
}

float Drive::getYaw() {
	//return mMotors[0]->getSpeed();
	//return mPIDYaw->output();
	return mYaw;
}

void Drive::setPIDAngle(float p, float i, float d) {
	mPIDRollAngle->setPID(p, i, d);
	mPIDPitchAngle->setPID(p, i, d);
	mPIDYawAngle->setPID(p, i, d);

	mPIDRollAngle->reset();
	mPIDPitchAngle->reset();
	mPIDYawAngle->reset();
}

void Drive::setPIDRate(float p, float i, float d) {
	mPIDRollRate->setPID(p, i, d);
	mPIDPitchRate->setPID(p, i, d);
	mPIDYawRate->setPID(p, i, d);

	mPIDRollRate->reset();
	mPIDPitchRate->reset();
	mPIDYawRate->reset();
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
	mAccelOffset.z = accel_total.z / num_iterations + 1.0f;
	mGyroOffset.x = gyro_total.x / num_iterations;
	mGyroOffset.y = gyro_total.y / num_iterations;
	mGyroOffset.z = gyro_total.z / num_iterations;

	saveCalibration("calibration.ini");
}

void Drive::update() {

	updateSensors();

	// Determine elapsed time since last update()
	struct timeval currenttime;
	gettimeofday(&currenttime, NULL);
	float dtime = currenttime.tv_sec - mLastUpdate.tv_sec
			+ (currenttime.tv_usec - mLastUpdate.tv_usec) / 1000000.0f;
	mLastUpdate = currenttime;

	mTargetYaw += mRotate * dtime;

	// Calculate average sensor readings over time
	Vector3<float> accel = averageAccelerometer();
	Vector3<float> gyro = averageGyroscope();

	// Adjust for calibration
	accel.x -= mAccelOffset.x;
	accel.y -= mAccelOffset.y;
	accel.z -= mAccelOffset.z;
	gyro.x -= mGyroOffset.x;
	gyro.y -= mGyroOffset.y;
	gyro.z -= mGyroOffset.z;

	calculateOrientation(dtime, accel, gyro);
	stabilize(gyro);

	try {
		for (int i = 0; i < 4; ++i)
			mMotors[i]->update();
	} catch (Exception &e) {
		std::cout << "Motor update failure." << std::endl;
	}
}

/*
	Private member functions
*/

void Drive::updateSensors() {

	try {
		// Fix exception handling for production
		mAccelValue[mAccelValueCurrent] = mAccelerometer->read();

		++mAccelValueCurrent;
		if (mAccelValueCurrent >= mSmoothing)
			mAccelValueCurrent = 0;

	} catch (Exception &e) {
		std::cout << " == ACCELEROMETER READ FAILURE ==" << std::endl;
		// MUST DELETE ME!
		// Probably just resort back to previous accelerometer reading, or don't
		// update the array at all. (gyro and accel have separate counters, could
		// take advantage)
//		stop();
//		char c;
//		while (read(STDIN_FILENO, &c, 1) == 0)
//			usleep(10000);
	}

	try {
		// Fix exception handling for production
		mGyroValue[mGyroValueCurrent] = mGyroscope->read();

		++mGyroValueCurrent;
		if (mGyroValueCurrent >= mSmoothing)
			mGyroValueCurrent = 0;

	} catch (Exception &e) {
		std::cout << " == GYROSCOPE READ FAILURE ==" << std::endl;
//		stop();
//		char c;
//		while (read(STDIN_FILENO, &c, 1) == 0)
//			usleep(10000);
	}
}

void Drive::calculateOrientation(float dtime, Vector3<float> accel,
		Vector3<float> gyro) {

	Vector3<float> orient(mRoll, mPitch, mYaw);
	// -gyro.y because Gyro has opposite direction to Accelerometer in y axis
	orient += Vector3<float>(gyro.x * dtime, -gyro.y * dtime, gyro.z * dtime);

	if (orient.x > 180.0f)  orient.x -= 360.0f;
	if (orient.x < -180.0f) orient.x += 360.0f;
	if (orient.y > 180.0f)  orient.y -= 360.0f;
	if (orient.y < -180.0f) orient.y += 360.0f;
	if (orient.z > 180.0f)  orient.z -= 360.0f;
	if (orient.z < -180.0f) orient.z += 360.0f;

	float accelroll = atan2(accel.x, -accel.z) * 180.0 / PI;
	float accelpitch = atan2(accel.y, -sign(accel.z)
			* sqrt(accel.x * accel.x + accel.z * accel.z)) * 180.0 / PI;

	float accelmag = magnitude(accel);
	float factor = 1.0f - sign(1.0f - accelmag) * (1.0f - accelmag);
	if (factor < 0.0f)
		factor = 0.0f;

	mRoll  = orient.x * (1.0f - factor) + accelroll * factor;
	mPitch = orient.y * (1.0f - factor) + accelpitch * factor;
	mYaw   = orient.z;
}

void Drive::stabilize(Vector3<float> gyro) {
	// Adjust Angle PID setpoints
	mPIDRollAngle->setTarget(mTargetRoll);
	mPIDPitchAngle->setTarget(mTargetPitch);
	//mPIDYawAngle->setTarget(mTargetYaw);

	// Feed current angle to Angle PID controllers
	mPIDRollAngle->feed(mRoll);
	mPIDPitchAngle->feed(mPitch);
	//mPIDYawAngle->feed(mYaw);

	// Adjust Rate PID setpoints based on Angle PID outputs
	mPIDRollRate->setTarget(mPIDRollAngle->output());
	mPIDPitchRate->setTarget(mPIDPitchAngle->output());
	//mPIDYawRate->setTarget(mPIDYawAngle->output());

	// Feed angular rate to Angle PID controllers
	mPIDRollRate->feed(gyro.x);
	mPIDPitchRate->feed(gyro.y);
	//mPIDYawRate->feed(gyro.z);

	// Assign motor values based on PID outputs
	float motorspeeds[4];
//	for (int i = 0; i < 4; ++i)
//		motorspeeds[i] = mTranslate.z;

	// Temporary isolation of opposing motors
	for (int i = 0; i < 4; ++i)
		motorspeeds[i] = 0.0f;
	motorspeeds[0] = mTranslate.z;
	motorspeeds[2] = mTranslate.z;

	double d_ends  = mPIDPitchRate->output() / 100.0f;
	double d_sides = mPIDRollRate->output() / 100.0f;
	//printf("%f | %f\n", mPIDPitchRate->output(), mPIDRollRate->output());

	motorspeeds[0] += d_ends - d_sides;
	//motorspeeds[1] += d_ends + d_sides;
	motorspeeds[2] += -d_ends + d_sides;
	//motorspeeds[3] += -d_ends - d_sides;

	// Need to add in rotational motion on top of this!

	try {
		// Remove try block in production... or make it useful somehow for
		// handling errors
		for (int i = 0; i < 4; ++i) {
			if (motorspeeds[i] < 0.0f)
				motorspeeds[i] = 0.0f;
			mMotors[i]->setSpeed(motorspeeds[i]);
		}
	} catch (Exception &e) {
		std::cout << "Motor set failure. " << motorspeeds[0] << ", "
				<< motorspeeds[1] << ", " << motorspeeds[2] << ", "
				<< motorspeeds[4] << std::endl;
		throw e;
	}
}

Vector3<float> Drive::averageAccelerometer() {
	Vector3<float> avg(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < mSmoothing; ++i)
		avg += mAccelValue[i];
	avg.x /= mSmoothing;
	avg.y /= mSmoothing;
	avg.z /= mSmoothing;

	return avg;
}

Vector3<float> Drive::averageGyroscope() {
	Vector3<float> avg(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < mSmoothing; ++i)
		avg += mGyroValue[i];
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

/*
	Timer thread update routines
*/

static void timerUpdate(int signal, siginfo_t *info, void *context) {
	Drive *drive = (Drive *)info->si_value.sival_ptr;
	if (drive->isTimedUpdate())
		drive->update();
}

void updateThreadExit(Drive *drv) {
	// Ignore signal
	drv->mTimerAction.sa_handler = SIG_IGN;
	drv->mTimerAction.sa_flags = 0;
	sigemptyset(&drv->mTimerAction.sa_mask);
	sigaction(SIGALRM, &drv->mTimerAction, NULL);
//	if (sigaction(SIGALRM, &mTimerAction, NULL) != 0)
//		THROW_EXCEPT(DriveException, "Could not disable signal handler\n");

	// Disarm timer
	struct itimerspec time;
	time.it_interval.tv_sec  = 0;
	time.it_interval.tv_nsec = 0;
	time.it_value.tv_sec     = 0;
	time.it_value.tv_nsec    = 0;
	timer_settime(drv->mTimerID, 0, &time, NULL);
//	if (timer_settime(mTimerID, 0, &time, NULL) != 0)
//		THROW_EXCEPT(DriveException, "Could not stop timer");

	// Indicate to main thread that the thread is no longer running
	drv->mTimerEnabled = false;
	drv->mTimerID = 0;
	pthread_exit(NULL);
}

/**
	Entry point for the automatic update thread.

	drv is expected to be a pointer to the Drive class that this thread will
	be handling.
*/
void *updateThreadEntry(void *drv) {
	Drive *drive = (Drive *)drv;
	
//	struct sigaction drive->mTimerAction;
//	struct sigevent drive->mTimerEvent;

	// Establish signal handler
	drive->mTimerAction.sa_sigaction = timerUpdate;
	drive->mTimerAction.sa_flags = SA_SIGINFO;
	sigemptyset(&drive->mTimerAction.sa_mask);

	if (sigaction(SIGALRM, &drive->mTimerAction, NULL) != 0)
		updateThreadExit(drive);

	// Create timer that will send signal
	if (drive->mTimerID == 0) {
		drive->mTimerEvent.sigev_notify = SIGEV_THREAD_ID | SIGEV_SIGNAL;
		drive->mTimerEvent.sigev_signo = SIGALRM;
		drive->mTimerEvent.sigev_value.sival_ptr = drive;
		drive->mTimerEvent.sigev_notify_thread_id = gettid();

		if (timer_create(CLOCK_REALTIME, &drive->mTimerEvent,
				&drive->mTimerID) != 0)
			updateThreadExit(drive);
	}

	// Set timer
	struct itimerspec time;
	time.it_interval.tv_sec  = (drive->mUpdateRate == 1 ? 1 : 0);
	time.it_interval.tv_nsec =
			(drive->mUpdateRate == 1 ? 0 : 1000000000 / drive->mUpdateRate);
	time.it_value.tv_sec  = time.it_interval.tv_sec;
	time.it_value.tv_nsec = time.it_interval.tv_nsec;

	if (timer_settime(drive->mTimerID, 0, &time, NULL) != 0)
		updateThreadExit(drive);


	// Continually wait for timer signal until Drive indicates to stop
	while (drive->isTimedUpdate())
		pause();


	updateThreadExit(drive);
}

