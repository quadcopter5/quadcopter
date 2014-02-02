/*
	pidcontroller.h

	PIDController class - accepts a single input feed and gives output 
*/

#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <sys/time.h>
#include <list>

class PIDController {
	public:
		/**
			Initialize a software PID controller with given target value and
			coefficients. What the controller does:

			Error is how much the system's value is away from the target
			value.

			Proportional - the current instantaneous error
			Integral     - the accumulation of past errors
			Derivative   - prediction of future error

			These values are calculated, and then the coefficients provided to
			this constructor are multiplied correspondingly to get the final
			output of the controller.

			accum_size determines how many inputs are retained for the
			purposes of calculating the Derivative term.
		*/
		PIDController(float target,
		              float proportional,
		              float integral,
		              float derivative,
					  size_t accum_size = 3);

		/**
			Feed an input value to the controller.
		*/
		void feed(float value);

		/**
			Get the current output of the controller. Consecutive calls to
			this function will be the same between calls to feed().

			The output of the controller is the "correction" value, as in the
			opposite direction of the error.
		*/
		float output();

		/**
			Set the target to a new value.
		*/
		void setTarget(float target);

		float getSumError() {
			return mSumError;
		}

	private:
		float mTarget,
		      mProportional,
		      mIntegral,
		      mDerivative;

		struct Event {
			Event(float e, float t) : error(e), time(t) { }
			float error;
			float time;  // in seconds
		};

		std::list<Event> mAccumulator;     // Past inputs
		size_t           mAccumulatorSize; // How many past inputs to track
		float            mSumError;        // Sum of past errors (Integral)
		float mOutput;      // Calculated output (avoid repeated calculations)

		struct timeval mLastUpdate;
		float mTimeCurrent;
};

#endif

