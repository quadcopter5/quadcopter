/**
	test_stabilize_visual.cpp

	Program for controlling test_stabilize on the quadcopter.

	Adds an additional Visualizer component to test_stabilize to better
	evaluate the state of the stabilization loop.
*/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <SDL2/SDL.h>

#include "radiolinux.h"
#include "radioconnection.h"
#include "packet.h"
#include "packetmotion.h"
#include "packetdiagnostic.h"

#include "visualizer.h"

#define MAX_FPS 60

int main(int argc, char **argv) {

	try {
		RadioLinux radio("/dev/ttyUSB0", 57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		std::cout << "Waiting for connection..." << std::endl;
		connection.connect();
		std::cout << "Connected!" << std::endl;

		std::cout << "Use W and S to move up and down" << std::endl;

		Visualizer visualizer("Stabilization Test");

		char x = 0, y = 0, z = 0;
		float pidangle_p = 0.0, pidangle_i = 0.0, pidangle_d = 0.0,
		      pidrate_p = 0.0, pidrate_i = 0.0, pidrate_d = 0.0;
		Packet *pkt;
		bool changed_setpoint,
		     changed_pidangle,
		     changed_pidrate;

		enum WhichPID { PID_ANGLE, PID_RATE };
		int  modifying = PID_ANGLE;

		SDL_Event evt;
		Uint32 frame_start;
		bool running = true;

		while (running) {

			frame_start = SDL_GetTicks();

			changed_setpoint = false;
			changed_pidangle = false;
			changed_pidrate = false;
			while (SDL_PollEvent(&evt)) {
				switch (evt.type) {
					case SDL_QUIT: {
						PacketMotion p(0, 0, 0, 64);
						connection.send(&p);
						std::cout << "Sent \"quit\" signal" << std::endl;
						running = false;
					}	break;

					case SDL_KEYDOWN:
						switch (evt.key.keysym.sym) {
							case SDLK_ESCAPE: {
								PacketMotion p(0, 0, 0, 64);
								connection.send(&p);
								std::cout << "Sent \"quit\" signal"
									<< std::endl;
								running = false;
							}	break;

							case SDLK_q: {
								PacketMotion p(0, 0, 0, 64);
								connection.send(&p);
								std::cout << "Sent \"quit\" signal"
									<< std::endl;
							}	break;

							// Setpoint adjustment

							case SDLK_w:
								z += 4;
								changed_setpoint = true;
								break;

							case SDLK_s:
								z -= 4;
								changed_setpoint = true;
								break;

							case SDLK_a:
								x -= 4;
								y -= 4;
								changed_setpoint = true;
								break;

							case SDLK_d:
								x += 4;
								y += 4;
								changed_setpoint = true;
								break;

							case SDLK_MINUS:
								x = -x;
								y = -y;
								changed_setpoint = true;
								break;

							// PID adjustment

							case SDLK_h:
								if (modifying == PID_ANGLE) {
									modifying = PID_RATE;
									std::cout << "PID adjustments will now "
											<< "affect Rate PID" << std::endl;
								} else {
									modifying = PID_ANGLE;
									std::cout << "PID adjustments will now "
											<< "affect Angle PID" << std::endl;
								}
								break;

							case SDLK_u:
								if (modifying == PID_ANGLE) {
									pidangle_p += 0.01f;
									changed_pidangle = true;
								} else if (modifying == PID_RATE) {
									pidrate_p += 0.01f;
									changed_pidrate = true;
								}
								break;

							case SDLK_j:
								if (modifying == PID_ANGLE) {
									pidangle_p -= 0.01f;
									changed_pidangle = true;
								} else if (modifying == PID_RATE) {
									pidrate_p -= 0.01f;
									changed_pidrate = true;
								}
								break;

							case SDLK_i:
								if (modifying == PID_ANGLE) {
									pidangle_i += 0.01f;
									changed_pidangle = true;
								} else if (modifying == PID_RATE) {
									pidrate_i += 0.01f;
									changed_pidrate = true;
								}
								break;

							case SDLK_k:
								if (modifying == PID_ANGLE) {
									pidangle_i -= 0.01f;
									changed_pidangle = true;
								} else if (modifying == PID_RATE) {
									pidrate_i -= 0.01f;
									changed_pidrate = true;
								}
								break;

							case SDLK_o:
								if (modifying == PID_ANGLE) {
									pidangle_d += 0.01f;
									changed_pidangle = true;
								} else if (modifying == PID_RATE) {
									pidrate_d += 0.01f;
									changed_pidrate = true;
								}
								break;

							case SDLK_l:
								if (modifying == PID_ANGLE) {
									pidangle_d -= 0.01f;
									changed_pidangle = true;
								} else if (modifying == PID_RATE) {
									pidrate_d -= 0.01f;
									changed_pidrate = true;
								}
								break;

							case SDLK_p:
								pidangle_p = 0.0f;
								pidangle_i = 0.0f;
								pidangle_d = 0.0f;
								pidrate_p = 0.0f;
								pidrate_i = 0.0f;
								pidrate_d = 0.0f;
								changed_pidangle = true;
								changed_pidrate = true;
								break;

							// Graph scale adjustment

							case SDLK_UP:
								visualizer.setValueScale(
										visualizer.getValueScale() * 1.05);
								break;

							case SDLK_DOWN:
								visualizer.setValueScale(
										visualizer.getValueScale() / 1.05);
								break;

							case SDLK_LEFT:
								visualizer.setTimeScale(
										visualizer.getTimeScale() * 1.05);
								break;

							case SDLK_RIGHT:
								visualizer.setTimeScale(
										visualizer.getTimeScale() / 1.05);
								break;
						}
				} // switch evt.type
			} // SDL_PollEvent

			if (changed_setpoint) {
				PacketMotion p(x, y, z, 0);
				connection.send(&p);
				std::cout << "Sent signal (x = " << (int)x
						<< ", y = " << (int)y
						<< ", z = " << (int)z
						<< ")" << std::endl;
			}

			if (changed_pidangle) {
				PacketDiagnostic p(0, pidangle_p, pidangle_i, pidangle_d);
				connection.send(&p);
				std::cout << "Sent signal (Affect Angle, P = " << pidangle_p
						<< ", I = " << pidangle_i
						<< ", D = " << pidangle_d
						<< ")" << std::endl;
			}

			if (changed_pidrate) {
				PacketDiagnostic p(1, pidrate_p, pidrate_i, pidrate_d);
				connection.send(&p);
				std::cout << "Sent signal (Affect Rate, P = " << pidrate_p
						<< ", I = " << pidrate_i
						<< ", D = " << pidrate_d
						<< ")" << std::endl;
			}

			// Poll for radio packets
			while (pkt = connection.receive()) {
				switch (pkt->getHeader()) {
					case PKT_DIAGNOSTIC: {
						PacketDiagnostic *pktdiag = (PacketDiagnostic *)pkt;
						visualizer.feedValues(Vector3<float>(
								pktdiag->getAccelX(),
								pktdiag->getAccelY(),
								pktdiag->getAccelZ()),
								frame_start);
						delete pktdiag;
					}	break;
				}
			}
			pkt = 0;

			visualizer.render(frame_start);

			while (SDL_GetTicks() - frame_start < 1000 / MAX_FPS)
				SDL_Delay(1000 / MAX_FPS - (SDL_GetTicks() - frame_start));

		} // Main loop

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Quitting..." << std::endl;
	return 0;
}

