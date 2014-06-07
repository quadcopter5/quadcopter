/*
	primemotors.cpp

	Sets signal on all PWM channels (0-15) to be a motor prime signal. Then waits
	for 3 seconds before quitting. For use at startup during development (until
	production-stage program is ready to run at startup).
*/

#include <string>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "motor.h"

int main(int argc, char **argv) {
	try {
		int nummotors = 16;

		I2C connection("/dev/i2c-1");
		PWM pwm(&connection, 0x40);
		pwm.setFrequency(50);

		std::cout << " == Priming motors (all channels)... ==" << std::endl;

		Motor *motors[16];
		for (int i = 0; i < nummotors; ++i)
			motors[i] = new Motor(&pwm, i, 1.25f, 1.4f);

		usleep(3000000);
		std::cout << " == Primed motors on all PWM channels ==" << std::endl;

		for (int i = 0; i < nummotors; ++i)
			delete motors[i];

	} catch (PWMException &e) {
		std::cerr << "PWM EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	} catch (I2CException &e) {
		std::cerr << "I2C EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	} catch (Exception &e) {
		std::cerr << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	return 0;
}

