/*
	test_radiouart.cpp

	Test for RadioUART class
*/

#include <string>
#include <iostream>
#include <stdint.h>

#include "radio.h"
#include "radiouart.h"

int main(int argc, char **argv) {
	try {

		RadioUART radio(57600, Radio::PARITY_EVEN);

		int  done = 0,
			 bytes;
		char *pos;
		std::string buffer;

		while (!done) {
			// Receive
			bytes = radio.read(buffer, 50);
			if (bytes > 0) {
				// Convert CR to LF
				size_t index;
				while ((index = buffer.find('\r')) != std::string::npos)
					buffer[index] = '\n';
				std::cout << "Rx: " << buffer << std::endl;
			}
			bytes = radio.getInputQueueSize();
			std::cout << "(" << bytes << " bytes in queue)" << std::endl;

			if (buffer.find('`') != std::string::npos)
				done = 1;
			else {
				// Transmit
				std::cout << "Tx: ";
				std::getline(std::cin, buffer);

				if (buffer.length() > 0) {
					radio.write(buffer);
					if (buffer.find('`') != std::string::npos)
						done = 1;
				}
			}
		}

		std::cout << "Attempting to read a UBE16..." << std::endl;
		uint16_t number16;
		if (radio.readUBE16(&number16))
			std::cout << "  Received: " << number16 << std::endl;
		else {
			std::cout << "  Not enough data to read" << std::endl
			          << "  (Size of queue: " << radio.getInputQueueSize()
					  << ")" << std::endl;
		}

		std::cout << "Attempting to read a UBE32..." << std::endl;
		uint32_t number32;
		if (radio.readUBE32(&number32))
			std::cout << "  Received: " << number32 << std::endl;
		else {
			std::cout << "  Not enough data to read" << std::endl
			          << "  (Size of queue: " << radio.getInputQueueSize()
					  << ")" << std::endl;
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

