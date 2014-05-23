/*
	pidcontroller.cpp

	PIDController class - accepts a single input feed and gives output 
*/

#include <stddef.h>
#include <sys/time.h>
#include <list>

#include "pidcontroller.h"

PIDController::PIDController(float target, float proportional, float integral,
		float derivative, size_t accum_size)
		: mTarget(target), mProportional(proportional), mIntegral(integral),
		  mDerivative(derivative), mAccumulatorSize(accum_size) {
	mOutput = 0.0f;
	mTimeCurrent = 0.0f;
	mSumError = 0.0f;
	gettimeofday(&mLastUpdate, NULL);
}

void PIDController::feed(float value) {
	struct timeval current;
	gettimeofday(&current, NULL);

	float dtime = current.tv_sec - mLastUpdate.tv_sec
			+ (float)(current.tv_usec - mLastUpdate.tv_usec) / 1000000.0f;
	mTimeCurrent += dtime;
	mLastUpdate = current;

	mAccumulator.push_back(Event(value, mTimeCurrent));
	while (mAccumulator.size() > mAccumulatorSize)
		mAccumulator.pop_front();

	// Proportional error
	float p = mTarget - value;

	// Integral error
	mSumError += (mTarget - value) * dtime;
	float i = mSumError;

	// Derivative of process value
	// Average slope of past Process Values
	float d = 0.0f;
	if (mAccumulator.size() > 0) {
		float preverror = mAccumulator.front().error,
		      prevtime = mAccumulator.front().time;

		std::list<Event>::iterator it = mAccumulator.begin();
		for (++it; it != mAccumulator.end(); ++it)
			d += (it->error - preverror) / (it->time - prevtime);
		d /= mAccumulator.size();
	}

	mOutput = (mProportional * p) + (mIntegral * i) - (mDerivative * d);
}

float PIDController::output() {
	return mOutput;
}

void PIDController::setTarget(float target) {
	mTarget = target;
}

void PIDController::setP(float p) {
	mProportional = p;
}

void PIDController::setI(float i) {
	mIntegral = i;
}

void PIDController::setD(float d) {
	mDerivative = d;
}

void PIDController::setPID(float p, float i, float d) {
	mProportional = p;
	mIntegral = i;
	mDerivative = d;
}

void PIDController::reset() {
	mAccumulator.clear();
	mSumError = 0.0f;
	mOutput = 0.0f;
	mTimeCurrent = 0.0f;
	gettimeofday(&mLastUpdate, NULL);
}

