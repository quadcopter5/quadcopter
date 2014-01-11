/*
	test_radio_alternate.cpp

	Test for RadioConnection class. Alternates between sending and waiting for
	Packets.
*/

#include <string>
#include <iostream>
#include <stdint.h>
#include <unistd.h>

#include "radio.h"
#include "radiolinux.h"
#include "radioconnection.h"
#include "packetmotion.h"

int main(int argc, char **argv) {
	try {

		RadioLinux radio("/dev/ttyUSB0", 57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		std::cout << "Waiting for connection..." << std::endl;
		connection.connect();
		std::cout << "Connected!" << std::endl;

		int num, check;
		while (true) {
			// Receive
			num = 0;
			check = 0;
			while (num != 10) {
				Packet *packet;

				int times = 0;
				while ((packet = connection.receive()) == 0) {
					usleep(10000);
					++times;
					if (times > 300) {
						std::cout << "Waiting..." << std::endl;
						times = 0;
					}
				}

				switch (packet->getHeader()) {
					case PKT_MOTION: {
						PacketMotion *p = (PacketMotion *)packet;
						std::cout << "Received PKT_MOTION : "
						          << "x = " << (unsigned int)p->getX()
						          << ", y = " << (unsigned int)p->getY()
						          << ", z = " << (unsigned int)p->getZ()
						          << ", rot = " << (unsigned int)p->getRot()
						          << std::endl;
						num = p->getRot();
						++check;
						if (num != check) {
							std::cout << "MISSED A PACKET!" << std::endl;
							return 1;
						}
					}	break;

					default:
						std::cout << "Unrecognized packet type" << std::endl;
						break;
				}

				delete packet;
			}

			// Send
			num = 1;
			while (num <= 10) {
				PacketMotion p(0, 0, 0, num);
				connection.send(&p);
				std::cout << "Sent PKT_MOTION : 0, 0, 0, " << num << std::endl;

				usleep(1000);

				++num;
			}
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

