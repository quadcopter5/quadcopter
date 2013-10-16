/**
	test_stabilize.cpp

	Program for controlling test_stabilize on the quadcopter. Simply sends
	a Z coordinate for translational motion according to keypresses W/S
*/

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>

#include "radiolinux.h"
#include "radioconnection.h"
#include "packet.h"
#include "packetmotion.h"
#include "packetdiagnostic.h"

int main(int argc, char **argv) {

	// Get current console termios attributes (so we can restore it later)
	struct termios in_oldattr, in_newattr;
	tcgetattr(STDIN_FILENO, &in_oldattr);
	memcpy(&in_newattr, &in_oldattr, sizeof(struct termios));

	// Set console attributes
	in_newattr.c_lflag = 0;
	in_newattr.c_cc[VMIN] = 0;
	in_newattr.c_cc[VTIME] = 0;
	in_newattr.c_cc[VINTR] = 3;

	tcsetattr(STDIN_FILENO, TCSANOW, &in_newattr);

	usleep(3000000);

	tcsetattr(STDIN_FILENO, TCSANOW, &in_oldattr);
	return 0;

	try {
		RadioLinux radio("/dev/ttyUSB0", 57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

//		std::cout << "Waiting for connection..." << std::endl;
//		connection.connect();
//		std::cout << "Connected!" << std::endl;

		std::cout << "Use W and S to move up and down" << std::endl;

		char c;
		bool running = true;
		while (running) {
			c = read(STDIN_FILENO, &c, 1);
			switch (c) {
				case 'q': {
					PacketMotion p(0, 0, 0, 64);
					connection.send(&p);
					std::cout << "Sent \"quit\" signal" << std::endl;
					running = false;
				}	break;
				case 'w': {
					PacketMotion p(0, 0, 32, 0);
					connection.send(&p);
					std::cout << "Sent +z signal" << std::endl;
				}	break;
				case 's': {
					PacketMotion p(0, 0, -32, 0);
					connection.send(&p);
					std::cout << "Sent -z signal" << std::endl;
				}	break;
				default:
					break;
			}

			usleep(100000);
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Quitting..." << std::endl;
	tcsetattr(STDIN_FILENO, TCSANOW, &in_oldattr);
	
	usleep(500000);

	return 0;
}

