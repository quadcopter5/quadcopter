/*
	test_radio.cpp

	Test for Radio and RadioLinux classes
*/

#include <string>
#include <iostream>

#include <unistd.h>

#include "radio.h"
#include "radiolinux.h"

int main(int argc, char **argv) {
	try {
		Radio *radio = new RadioLinux("/dev/ttyUSB0", 57600,
				Radio::PARITY_EVEN);

		radio->write("Hello!");

		std::string message;
		uint32_t number;
		while (true) {
//			size_t bytes = radio->read(message, 0);
			size_t bytes = radio->readUBE32(&number);
			std::cout << bytes << "B : " << number << std::endl;
			usleep(100000);
		}

		delete radio;
	} catch (RadioException &e) {
		std::cout << "RADIO EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}
	return 0;
}

