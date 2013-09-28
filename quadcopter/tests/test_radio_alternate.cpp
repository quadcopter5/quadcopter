/*
	test_radio_alternate.cpp

	Test for RadioConnection class. Alternates between sending and waiting for
	Packets.
*/

#include <string>
#include <iostream>
#include <stdint.h>

#include "radio.h"
#include "radiouart.h"
#include "radioconnection.h"
#include "packetmotion.h"

int main(int argc, char **argv) {
	try {

		RadioUART radio(57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		connection.connect();
		std::cout << "Connected!" << std::endl;

		int num;
		while (true) {
			// Send
			num = 1;
			while (num <= 10) {
				PacketMotion p(0, 0, 0, num);
				connection.send(&p);
				std::cout << "Sent PKT_MOTION : 0, 0, 0, " << num << std::endl;

				usleep(10000);

				++num;
			}

			std::cout << std::endl;

			// Receive
			num = 0;
			while (num != 10) {
				Packet *packet;
				while ((packet = connection.receive()) == 0)
					usleep(10000);
				
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
					}	break;

					default:
						std::cout << "Unrecognized packet type" << std::endl;
						break;
				}

				delete packet;
			}

			std::cout << std::endl;
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

