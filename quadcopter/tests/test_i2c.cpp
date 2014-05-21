/*
	test_i2c.cpp

	Test for I2C class
	Using ADXL345 Accelerometer (on GY-80 chip)
*/

#include <iostream>
#include <stdint.h>
#include <unistd.h>

#include "i2c.h"

int main(int argc, char **argv) {
	try {
		I2C connection("/dev/i2c-1");

		char data[2];
		data[0] = 0b00000000;
		connection.write(0x53, &data[0], 1);
		connection.read(0x53, &data[1], 1);

		std::cout << "Returned value: 0x" << std::hex << (int)data[1]
				<< std::endl;

		for (int i = 0; i < 10000; ++i) {
			std::cout << "Doing queued transaction" << std::endl;

			connection.enqueueWrite(0x53, &data[0], 1);
			connection.enqueueRead(0x53, &data[1], 1);
			connection.sendTransaction();

			std::cout << "Returned value: 0x" << std::hex << (int)data[1]
					<< std::endl;

			usleep(100);
		}

	} catch (I2CException &e) {
		std::cerr << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

