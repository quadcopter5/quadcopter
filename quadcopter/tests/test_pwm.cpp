/*
	test_i2c.cpp

	Test for I2C class
	Using ADXL345 Accelerometer (on GY-80 chip)
*/

#include <iostream>
#include <stdint.h>

#include "pwm.h"

int main(int argc, char **argv) {
	try {
		I2C connection("dev/i2c-1");
		PWM pwm(&connection, 0x40);

		pwm.setFrequency(50);
		pwm.setHighTime(0, 1.5f);

		std::cout << "Set High Time for Channel 0 to 1.5 ms" << std::endl;
		std::cin.ignore();

		pwm.setLoad(0, 0.1f); // 10% load of 20ms

		std::cout << "Set Load factor to 10%" << std::endl;

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

