/**
	test_stabilize.cpp

	Program for controlling test_stabilize on the quadcopter. Simply sends
	a Z coordinate for translational motion according to keypresses W/S
*/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "radiolinux.h"
#include "radioconnection.h"
#include "packet.h"
#include "packetmotion.h"
#include "packetdiagnostic.h"

struct termios in_oldattr = { 0 },
               in_newattr = { 0 };

void quit(int code) {
	tcsetattr(STDIN_FILENO, TCSANOW, &in_oldattr);
	exit(code);
}

void startUnbufferedInput() {
	// Get current console termios attributes (so we can restore it later)
	tcgetattr(STDIN_FILENO, &in_oldattr);
	memcpy(&in_newattr, &in_oldattr, sizeof(struct termios));

	// Set console attributes
	in_newattr.c_lflag = ISIG;
	in_newattr.c_cc[VMIN] = 0;
	in_newattr.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &in_newattr);
}

int main(int argc, char **argv) {

	try {
		RadioLinux radio("/dev/ttyUSB0", 57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		std::cout << "Waiting for connection..." << std::endl;
		connection.connect();
		std::cout << "Connected!" << std::endl;

		std::cout << "Use W and S to move up and down" << std::endl;

		startUnbufferedInput();

		char c;
		int bytes;

		char value = 0;
		bool running = true;
		while (running) {
			while ((bytes = read(STDIN_FILENO, &c, 1)) > 0) {
				switch (c) {
					case 'q': {
						PacketMotion p(0, 0, 0, 64);
						connection.send(&p);
						std::cout << "Sent \"quit\" signal" << std::endl;
						running = false;
					}	break;
					case 'w': {
						value += 4;
						PacketMotion p(0, 0, value, 0);
						connection.send(&p);
						std::cout << "Sent z signal = " << (int)value
								<< std::endl;
					}	break;
					case 's': {
						value -= 4;
						PacketMotion p(0, 0, value, 0);
						connection.send(&p);
						std::cout << "Sent z signal = " << (int)value
								<< std::endl;
					}	break;
					default:
						break;
				}
			}

			usleep(50000);
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		quit(-1);
	}

	std::cout << "Quitting..." << std::endl;
	quit(0);
}

