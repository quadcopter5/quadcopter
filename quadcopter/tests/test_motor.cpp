/*
	test_motor.cpp

	Test for Motor class

	Primes motor on PWM channel 0 and then goes through a loop increasing the
	speed by 10% each time, until 100% speed.
*/

#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "motor.h"

int main(int argc, char **argv) {
	try {
		I2C connection("/dev/i2c-1");
		PWM pwm(&connection, 0x40);
		
		Motor motor1(&pwm, 0, 2.0f, 3.0f);

		// Prime motor
		std::cout << "Priming..." << std::endl;
		motor1.setSpeed(0.5f);
		usleep(1000);
		motor1.setSpeed(0.0f);
		std::cout << "Primed motor on channel 0" << std::endl;

		int speed = 0;
		while (speed <= 100) {
			motor1.setSpeed((float)speed / 100.0f);
			std::cout << "Set speed to " << speed << "%" << std::endl;
			std::cin.ignore();

			speed += 10;
		}

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

	std::cout << "Done!" << std::endl;
	return 0;
}

