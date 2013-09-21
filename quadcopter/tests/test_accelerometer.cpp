/*
	test_accelerometer.cpp

	Test for Accelerometer class
*/

#include <string>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <stdint.h>

#include <unistd.h>

#include "accelerometer.h"

int main(int argc, char **argv) {
	try {
		I2C connection("/dev/i2c-1");
		Accelerometer accel(&connection, 0x53, Accelerometer::RANGE_2G,
				Accelerometer::SRATE_100HZ);

		Vector3<float> values;

		std::string input;
		bool running = true;
		while (running) {
			values = accel.read();
			printf("X : %+08f | Y : %+08f | Z : %+08f\n",
					values.x, values.y, values.z);

			usleep(10000);
		}

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

