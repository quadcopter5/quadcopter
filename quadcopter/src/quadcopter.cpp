/*
	Quadcopter Main

	This is the starting point for the resident program on the RaspberryPi. It
	is meant to start during boot time.
*/

#include <iostream>
#include <string.h>

#include <unistd.h>
#include <termios.h>

#include "exception.h"
#include "i2c.h"
#include "pwm.h"
#include "accelerometer.h"
#include "drive.h"

int main(int argc, char **argv) {

	// Get current console termios attributes (so we can restore it later)
	struct termios in_oldattr, in_newattr;
	tcgetattr(STDIN_FILENO, &in_oldattr);
	memcpy(&in_newattr, &in_oldattr, sizeof(struct termios));

	// Set console attributes
	in_newattr.c_lflag = 0;
	in_newattr.c_cc[VMIN] = 0;
	in_newattr.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &in_newattr);

	try {

		I2C i2c("/dev/i2c-1");
		PWM pwm(&i2c, 0x40);
		pwm.setFrequency(50);
		Accelerometer accel(&i2c, 0x53, Accelerometer::RANGE_2G,
				Accelerometer::SRATE_50HZ);

		Drive drive(&pwm, &accel, 0, 2, 5, 7);

		char c;
		while (read(STDIN_FILENO, &c, 1) == 0) {
			drive.update();
			usleep(100000);
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	} catch (...) {
		std::cout << "UNKNOWN EXCEPTION" << std::endl;
		return -1;
	}

	std::cout << "Quitting..." << std::endl;
	usleep(1000000);

	tcsetattr(STDIN_FILENO, TCSANOW, &in_oldattr);

	return 0;
}

