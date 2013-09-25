/*
	test_pwm.cpp

	Test for PWM class
	Controls an analog PWM servo. Sets output to 1.5ms on 20ms (50Hz) frequency.
*/

#include <iostream>
#include <stdint.h>

#include "pwm.h"

int main(int argc, char **argv) {
	try {
		I2C connection("/dev/i2c-1");
		PWM pwm(&connection, 0x40);
		pwm.setFrequency(50);

		pwm.setHighTime(8, 0.6f);
		std::cout << "Set High Time for Channel 8 to 0.6 ms" << std::endl;
		std::cin.ignore();

		pwm.setHighTime(8, 2.6f);
		std::cout << "Set High Time for Channel 8 to 2.6 ms" << std::endl;
		std::cin.ignore();

		pwm.setLoad(8, 0.075f); // 0.075% load of 20ms (= 1.5ms)
		std::cout << "Set Load factor to 0.075%" << std::endl;
		std::cin.ignore();

	} catch (PWMException &e) {
		std::cerr << "PWM EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	} catch (Exception &e) {
		std::cerr << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

