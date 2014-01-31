/*
	test_gyroscope.cpp

	Test for Gyroscope class
*/

#include <string>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <stdint.h>

#include <unistd.h>

#include "gyroscope.h"

int main(int argc, char **argv) {
	try {
		I2C connection("/dev/i2c-1");
		Gyroscope gyro(&connection, 0x69, Gyroscope::RANGE_250DPS,
				Gyroscope::SRATE_100HZ);

		int numsamples = 5;
		int i = 1;
		Vector3<float> values;

		std::string input;
		bool running = true;
		while (running) {
			values += gyro.read();
			++i;

			if (i >= numsamples) {
				values.x /= numsamples;
				values.y /= numsamples;
				values.z /= numsamples;
				printf("X : %8d | Y : %8d | Z : %8d\n",
						values.x, values.y, values.z);
				i = 1;

				values.x = 0;
				values.y = 0;
				values.z = 0;
			}

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

