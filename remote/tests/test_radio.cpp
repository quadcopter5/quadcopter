/*
	test_radio.cpp

	Test for Radio and LinuxRadio classes
*/

#include <iostream>

#include "radio.h"
#include "radio_linux.h"

int main(int argc, char **argv) {
	try {
		Radio *radio = new LinuxRadio("/dev/ttyUSB0", 9600,
				Radio::PARITY_EVEN);

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

