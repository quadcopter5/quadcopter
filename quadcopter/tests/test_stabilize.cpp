/*
	test_stabilize.cpp

	Test for quadcopter stabilization. This is partly a test for the physical
	movement of the quadcopter to figure out necessary components of the
	stabilize algorithm.
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>
#include <signal.h>

#include "exception.h"
#include "i2c.h"
#include "pwm.h"
#include "geometry.h"
#include "accelerometer.h"
#include "drive.h"

#include "radiouart.h"
#include "radioconnection.h"
#include "packet.h"
#include "packetmotion.h"
#include "packetdiagnostic.h"

struct termios in_oldattr, in_newattr;

void quit(int code) {
	tcsetattr(STDIN_FILENO, TCSANOW, &in_oldattr);
	exit(code);
}

int main(int argc, char **argv) {

	// Get current console termios attributes (so we can restore it later)
	tcgetattr(STDIN_FILENO, &in_oldattr);
	memcpy(&in_newattr, &in_oldattr, sizeof(struct termios));

	// Set console attributes
	in_newattr.c_lflag = ISIG;
	in_newattr.c_cc[VMIN] = 0;
	in_newattr.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &in_newattr);

	try {

		RadioUART radio(57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		I2C i2c("/dev/i2c-1");
		PWM pwm(&i2c, 0x40);
		pwm.setFrequency(50);
		Accelerometer accel(&i2c, 0x53, Accelerometer::RANGE_2G,
				Accelerometer::SRATE_50HZ);

		std::cout << "Waiting for connection..." << std::endl;
		connection.connect();
		std::cout << "Connected!" << std::endl;

		Drive drive(&pwm, &accel, 0, 2, 5, 7, 10);

		char   c;
		bool   running = true;
		Packet *pkt = 0;

		while (running && read(STDIN_FILENO, &c, 1) == 0) {

			// Read any available packets
			while ((pkt = connection.receive()) != 0) {
				switch (pkt->getHeader()) {
					case PKT_MOTION:
					{
						PacketMotion *p = (PacketMotion *)pkt;
						if (p->getRot()) {
							running = false;
							std::cout << "Received QUIT signal" << std::endl;
						}
						else {
							std::cout << "Received packet, z = "
									<< (int)p->getZ() << std::endl;
							drive.move(Vector3<float>(0.0f, 0.0f,
									(float)p->getZ() / 128.0f));
						}
					}	break;

					default:
						break;
				}
				delete pkt;
				pkt = 0;
			}

			drive.update();
			usleep(20000); // 20,000 us = 0.02 ms = accelerometer update rate
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		quit(-1);
	} catch (...) {
		std::cout << "UNKNOWN EXCEPTION" << std::endl;
		quit(-1);
	}

	std::cout << "Quitting..." << std::endl;
	quit(0);
}

