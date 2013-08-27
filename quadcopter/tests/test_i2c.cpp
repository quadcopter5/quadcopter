/*
	test_i2c.cpp

	Test for I2C class
*/

#include <iostream>
#include <stdint.h>

#include "i2c.h"

int main(int argc, char **argv) {
	try {
		I2C connection("/dev/i2c-1");

		char data[2];
		data[1] = 0b00000000;
		data[2] = 0b00110000;
		connection.write(0x40, &data, 2);

		connection.write(0x40, &data, 1);
		connection.read(0x40, &data[0], 1);

		std::cout << "Returned value: 0x" << std::hex << (int)data[0]
				<< std::endl;
	} catch (I2CException &e) {
		std::cerr << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

