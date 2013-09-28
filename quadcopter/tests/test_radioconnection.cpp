/*
	test_radioconnection.cpp

	Test for RadioConnection class
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

		while (true) {
			Packet *packet;
			while ((packet = connection.receive()) == 0)
				usleep(100000);

			switch (packet->getHeader()) {
				case PKT_MOTION:
					PacketMotion *p = (PacketMotion *)packet;
					std::cout << "Received PKT_MOTION : "
							<< "x = " << (unsigned int)p->getX()
							<< ", y = " << (unsigned int)p->getY()
							<< ", z = " << (unsigned int)p->getZ()
							<< ", rot = " << (unsigned int)p->getRot()
							<< std::endl;
					break;
			}

			delete packet;
		}

		int num = 1;
		while (num <= 10) {
			PacketMotion p(0, 0, 0, num);
			connection.send(&p);

			usleep(100000);

			++num;
		}

		for (int i = 0; i < 10; ++i) {
			num = 1;
			while (num <= 255) {
				PacketMotion p(i, i, i, num);
				connection.send(&p);
				++num;
				usleep(1000);
			}
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

