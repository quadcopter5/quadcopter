/*
	test_visualize.cpp

	Test that visualizes the accelerometer readings, as sent from the
	quadcopter over radio.
*/

#include <stdlib.h>
#include <iostream>
#include <list>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "radio.h"
#include "radiolinux.h"
#include "radioconnection.h"
#include "packet.h"
#include "packetdiagnostic.h"
#include "geometry.h"

void quit(int code);

struct DataPoint {
	Uint32 timestamp;
	Vector3<float> accel;
};

int main(int argc, char **argv) {
	// Initialize radio
	try {
		RadioLinux radio("/dev/ttyUSB0", 57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		std::cout << "Waiting for connection..." << std::endl;
		connection.connect();
		std::cout << "Connected!" << std::endl;
	} catch (Exception &e) {
		std::cout << "Failed to initialize radio: "
		          << e.getMessage() << std::endl;
		return -1;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Could not initialize SDL" << std::endl;
		return -1;
	}

	// Set up OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Create window
	SDL_Window *window = SDL_CreateWindow("Visualizer",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			800, 600,
			SDL_WINDOW_OPENGL);// | SDL_WINDOW_INPUT_GRABBED);
	if (window == NULL) {
		std::cout << "Could not create SDL window" << std::endl;
		quit(-1);
	}

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if (glcontext == NULL) {
		std::cout << "Could not create OpenGL context" << std::endl;
		quit(-1);
	}

	SDL_ShowCursor(0);
//	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Set GL rendering options
	glOrtho(0, 800, 600, 0, -1.0, 1.0);
//	gluPerspective(70.0, 800.0 / 600.0, 1.0, 1000.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Main loop

	double cursor_x = 0.0, cursor_y = 0.0;

	Uint32 time_last_update = 0;
	std::list<DataPoint> angle;

	SDL_Event evt;
	Packet *pkt = 0;
	bool running = true;
	while (running) {
		// Poll user input events
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					running = false;
					break;

				case SDL_KEYDOWN:
					switch (evt.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = false;
							break;
						default:
							break;
					}
					break;

				case SDL_MOUSEMOTION:
					cursor_x = evt.motion.x;
					cursor_y = evt.motion.y;
					break;

				default:
					break;
			}
		}

		// Poll for radio packets
		while (pkt = connection.receive()) {
			switch (pkt.getHeader()) {
				case PKT_DIAGNOSTIC: {
					PacketDiagnostic *pktdiag = (PacketDiagnostic *)pkt;
					DataPoint data;
					data.timestamp = SDL_GetTicks();
					data.accel.x = pktdiag->getAccelX();
					data.accel.y = pktdiag->getAccelY();
					data.accel.z = pktdiag->getAccelZ();
				}	break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Cursor
		glBegin(GL_TRIANGLES);
			glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
			glVertex3d(cursor_x,        cursor_y,        0.0);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3d(cursor_x + 10.0, cursor_y,        0.0);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3d(cursor_x,        cursor_y + 10.0, 0.0);
		glEnd();

		// Graphs
		glBegin(GL_LINES);
			glColor4d(1.0, 0.0, 0.0, 0.5);
			glVertex3d(0.5,   100.5, 0.0);
			glVertex3d(800.5, 100.5, 0.0);

			glColor4d(0.0, 1.0, 0.0, 0.5);
			glVertex3d(0.5,   300.5, 0.0);
			glVertex3d(800.5, 300.5, 0.0);

			glColor4d(0.0, 0.0, 1.0, 0.5);
			glVertex3d(0.5,   500.5, 0.0);
			glVertex3d(800.5, 500.5, 0.0);
		glEnd();

		SDL_GL_SwapWindow(window);

		SDL_Delay(1000 / 60);
	}

	// Clean-up
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void quit(int code) {
	SDL_Quit();
	exit(code);
}

