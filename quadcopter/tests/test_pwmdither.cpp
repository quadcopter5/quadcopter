/*
	test_pwmdither.cpp

	Test for dithering in PWM class

	Primes motors on PWM channels 0-3 and then goes through a loop. On each
	iteration, waits for input for a speed to set (percentage). PWM is updated
	continously even while user is providing (or not) input.
*/

#include <string>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "motor.h"

struct termios in_oldattr, in_newattr;

void quit(int code) {
	tcsetattr(STDIN_FILENO, TCSANOW, &in_oldattr);
	exit(code);
}

void startUnbufferedInput() {
	// Get current console termios attributes (so we can restore it later)
	tcgetattr(STDIN_FILENO, &in_oldattr);
	memcpy(&in_newattr, &in_oldattr, sizeof(struct termios));

	// Set console attributes
	in_newattr.c_lflag = ISIG;
	in_newattr.c_cc[VMIN] = 0;
	in_newattr.c_cc[VTIME] = 0;
	in_newattr.c_cc[VERASE] = _POSIX_VDISABLE;

	tcsetattr(STDIN_FILENO, TCSANOW, &in_newattr);
}

int main(int argc, char **argv) {
	try {
		I2C connection("/dev/i2c-1");
		PWM pwm(&connection, 0x40);
		pwm.setFrequency(50);

		// Prime motors
		std::cout << "Priming..." << std::endl;

		Motor *motors[4];
		motors[0] = new Motor(&pwm, 0, 1.26, 1.4f);
		motors[1] = new Motor(&pwm, 2, 1.26, 1.4f);
		motors[2] = new Motor(&pwm, 5, 1.26, 1.4f);
		motors[3] = new Motor(&pwm, 7, 1.26, 1.4f);
//		for (int i = 0; i < 4; ++i)
//			motors[i] = new Motor(&pwm, i, 1.25f, 1.4f);

		usleep(3000000);
		std::cout << "Primed motors on channels 0, 2, 5, and 7" << std::endl;

		startUnbufferedInput();

		const size_t buffer_len = 50;
		size_t buffer_pos = 0;
		size_t bytes;
		char buffer[buffer_len],
			  c;
		bool completed = false;

		memset(buffer, 0, buffer_len);

		bool running = true;
		double speed = 0.0f;

		for (int i = 0; i < 4; ++i)
			motors[i]->setSpeed(speed / 100.0f);
		std::cout << "Set speed to " << speed << "%" << std::endl;
		std::cout << "Use ^H (CTRL-H) or CTRL-BACKSPACE to backspace"
		          << std::endl;
		std::cout << "Enter new speed (%) : ";
		std::cout.flush();

		while (running) {

			while ((bytes = read(STDIN_FILENO, &c, 1)) > 0) {
				completed = false;
				if (buffer_pos >= buffer_len) {
					buffer[buffer_len - 1] = '\0';
					completed = true;
				} else {
					if (c == '\b' && buffer_pos > 0) {
						std::cout << "\b \b";
						std::cout.flush();
						--buffer_pos;
						buffer[buffer_pos] = '\0';
					} else if (c >= '0' && c <= '9' || c == '.') {
						buffer[buffer_pos] = c;
						std::cout << c;
						std::cout.flush();
						++buffer_pos;
					} else if (c == '\n') {
						std::cout << std::endl;
						buffer[buffer_pos] = '\0';
						completed = true;
					}
				}
			}

			if (completed) {
				if (strlen(buffer) == 0)
					running = false;
				else {
					speed = atof(buffer);
					for (int i = 0; i < 4; ++i)
						motors[i]->setSpeed(speed / 100.0f);
					
					memset(buffer, 0, buffer_len);
					buffer_pos = 0;

					std::cout << "Set speed to " << speed << "%"
							<< std::endl;
					std::cout << "Enter new speed (%) : ";
					std::cout.flush();
				}
			}
			completed = false;

			for (int i = 0; i < 4; ++i)
				motors[i]->update();

			usleep(1000000 / 50);
		}

		std::cout << "Exiting..." << std::endl
				<< "Setting speed to 0%" << std::endl;

		for (int i = 0; i < 4; ++i)
			motors[i]->setSpeed(0.0f);

		usleep(1000000);

		for (int i = 0; i < 4; ++i)
			delete motors[i];

	} catch (PWMException &e) {
		std::cerr << "PWM EXCEPTION: " << e.getDescription() << std::endl;
		quit(-1);
	} catch (I2CException &e) {
		std::cerr << "I2C EXCEPTION: " << e.getDescription() << std::endl;
		quit(-1);
	} catch (Exception &e) {
		std::cerr << "EXCEPTION: " << e.getDescription() << std::endl;
		quit(-1);
	}

	std::cout << "Done!" << std::endl;
	quit(0);
}

