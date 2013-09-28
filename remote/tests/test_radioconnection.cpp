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

		for (int i = 0; i < 10; ++i) {
			PacketMotion packet(i, i, i, i);
			connection.send(&packet);
			std::cout << "Sent packet" << std::endl;
			usleep(100000);
		}

		Packet *packet = 0;

		while (true) {
			// Continue to receive while there are packets to read
			while (packet = connection.receive()) {
				switch (packet->getHeader()) {
					case PKT_MOTION: {
						PacketMotion *p = (PacketMotion *)packet;
						std::cout << "PKT_MOTION : "
						          << "x = " << (unsigned int)p->getX()
						          << ", y = " << (unsigned int)p->getY()
						          << ", z = " << (unsigned int)p->getZ()
						          << ", rot = " << (unsigned int)p->getRot()
						          << std::endl;
					}	break;

					default:
						std::cout << "Unknown packet type" << std::endl;
						break;
				}
				delete packet;
				packet = 0;
			}

			usleep(100000);
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	return 0;
}

