/*
	test_radioconnection.cpp

	Test for RadioConnection class
*/

#include <string>
#include <iostream>

#include <unistd.h>

#include "radiolinux.h"
#include "radioconnection.h"
#include "packetmotion.h"

int main(int argc, char **argv) {
	try {

		RadioLinux radio("/dev/ttyUSB0", 57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		Packet *packet = 0;

		while (true) {
			// Continue to receive while there are packets to read
			while (packet = connection.receive()) {
				switch (packet->getHeader()) {
					case PKT_MOTION: {
						PacketMotion *p = (PacketMotion *)packet;
						std::cout << "PKT_MOTION" << std::endl;
						std::cout << p->getX() << std::endl;
					}	break;

					default:
						std::cout << "Unknown packet type" << std::endl;
						break;
				}
				delete packet;
			}

			usleep(100000);
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	return 0;
}

