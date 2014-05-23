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
		float pid_p = 0.0, pid_i = 0.0, pid_d = 0.0;
		Packet *pkt;
		bool changed_setpoint,
		     changed_pid;

		SDL_Event evt;
		Uint32 frame_start;
		bool running = true;

		while (running) {

			frame_start = SDL_GetTicks();

			changed_setpoint = false;
			changed_pid = false;
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

							case SDLK_u:
								pid_p += 0.01f;
								changed_pid = true;
								break;

							case SDLK_j:
								pid_p -= 0.01f;
								changed_pid = true;
								break;

							case SDLK_i:
								pid_i += 0.01f;
								changed_pid = true;
								break;

							case SDLK_k:
								pid_i -= 0.01f;
								changed_pid = true;
								break;

							case SDLK_o:
								pid_d += 0.01f;
								changed_pid = true;
								break;

							case SDLK_l:
								pid_d -= 0.01f;
								changed_pid = true;
								break;

							case SDLK_p:
								pid_p = 0.0f;
								pid_i = 0.0f;
								pid_d = 0.0f;
								changed_pid = true;
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

			if (changed_pid) {
				PacketDiagnostic p(0, pid_p, pid_i, pid_d);
				connection.send(&p);
				std::cout << "Sent signal (P = " << pid_p
						<< ", I = " << pid_i
						<< ", D = " << pid_d
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

