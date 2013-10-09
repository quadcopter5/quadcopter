/*
	test_packets.cpp

	Test for the various Packet-type classes.
*/

#include <string>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/time.h>

#include "radio.h"
#include "radiouart.h"
#include "radioconnection.h"
#include "packetmotion.h"
#include "packetdiagnostic.h"

int main(int argc, char **argv) {
	try {

		RadioUART radio(57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		std::cout << "Waiting for connection..." << std::endl;
		connection.connect();
		std::cout << "Connected!" << std::endl;

		while (true) {
			// Receive
			Packet *packet = 0;

			int times = 0;
			while (times <= 300 && (packet = connection.receive()) == 0) {
				usleep(10000);
				++times;
			}

			if (times > 300)
				; // std::cout << "Timeout... Switching to send" << std::endl;
			else {
				switch (packet->getHeader()) {
					case PKT_MOTION: {
						PacketMotion *p = (PacketMotion *)packet;
						std::cout << "Received PKT_MOTION : "
								  << "x = " << (int)p->getX()
								  << ", y = " << (int)p->getY()
								  << ", z = " << (int)p->getZ()
								  << ", rot = " << (int)p->getRot()
								  << std::endl;
					}	break;

					case PKT_DIAGNOSTIC: {
						PacketDiagnostic *p = (PacketDiagnostic *)packet;
						std::cout << "Received PKT_DIAGNOSTIC : "
							<< "battery = " << (unsigned int)p->getBattery()
							<< ", accel_x = " << p->getAccelX()
							<< ", accel_y = " << p->getAccelY()
							<< ", accel_z = " << p->getAccelZ()
							<< std::endl;
					}	break;

					default:
						std::cout << "Unrecognized packet type" << std::endl;
						break;
				}

				delete packet;
			}

			// Send
			struct timeval t;
			gettimeofday(&t, NULL);
			srand(t.tv_sec + t.tv_usec);

			int type = rand() % 2;
			switch (type) {
				// PKT_MOTION
				default:
				case 0: {
					int8_t x = (rand() % 255) - 128,
					       y = (rand() % 255) - 128,
					       z = (rand() % 255) - 128,
					       rot = (rand() % 255) - 128;
					packet = new PacketMotion(x, y, z, rot);
					connection.send(packet);
					std::cout << "Sent PKT_MOTION : " << (int)x << ", "
							<< (int)y << ", " << (int)z << ", " << (int)rot
							<< std::endl;

					delete packet;
					packet = 0;
				}	break;

				// PKT_DIAGNOSTIC
				case 1: {
					int8_t batt = (rand() % 255) - 128;
					float  x = (float)(rand() % 255) / 255.0f,
					       y = (float)(rand() % 255) / 255.0f,
					       z = (float)(rand() % 255) / 255.0f;

					packet = new PacketDiagnostic(batt, x, y, z);
					connection.send(packet);
					std::cout << "Sent PKT_DIAGNOSTIC : " << (int)batt << ", "
							<< x << ", " << y << ", " << z << std::endl;

					delete packet;
					packet = 0;
				}	break;
			}
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}

