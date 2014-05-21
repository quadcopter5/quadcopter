/*
	setmotor.cpp

	Test for Motor class

	Sets signal on all PWM channels (0-15) to be a motor prime signal. Then goes
	through a loop that lets the user set the speed for all motors equally.
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

		// Prime motors
		std::cout << "Priming..." << std::endl;

		Motor *motors[16];
		for (int i = 0; i < nummotors; ++i)
			motors[i] = new Motor(&pwm, i, 1.25f, 1.4f);

		usleep(3000000);
		std::cout << "Primed motors on all PWM channels" << std::endl;

		std::cout << "Enter a blank line to quit" << std::endl;

		char buffer[10];
		bool running = true;
		double speed = 0.0f;
		while (running) {
			for (int i = 0; i < nummotors; ++i)
				motors[i]->setSpeed(speed / 100.0f);

			std::cout << "Set speed to " << speed << "%" << std::endl;

			std::cout << "Enter new speed (%) : ";
			std::cin.getline(buffer, 10);

			if (strlen(buffer) == 0)
				running = false;
			else
				speed = atof(buffer);
		}

		std::cout << "Exiting..." << std::endl
				<< "Setting speed to 0%" << std::endl;

		for (int i = 0; i < nummotors; ++i)
			motors[i]->setSpeed(0.0f);

		usleep(1000000);

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

	std::cout << "Done!" << std::endl;
	return 0;
}

