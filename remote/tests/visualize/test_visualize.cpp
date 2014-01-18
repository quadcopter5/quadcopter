/*
	test_visualize.cpp

	Test that visualizes the accelerometer readings, as sent from the
	quadcopter over radio.
*/

#include <stdlib.h>
#include <iostream>
#include <list>

#include <math.h>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "radio.h"
#include "radiolinux.h"
#include "radioconnection.h"
#include "packet.h"
#include "packetdiagnostic.h"
#include "geometry.h"

#define PI          3.1415926535

#define MAX_FPS     60

void quit(int code);

struct DataPoint {
	Uint32 timestamp;
	Vector3<float> accel;
};

int main(int argc, char **argv) {
	// Initialize radio
	try {
		/*
		RadioLinux radio("/dev/ttyUSB0", 57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

		std::cout << "Waiting for connection..." << std::endl;
		connection.connect();
		std::cout << "Connected!" << std::endl;
		*/

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
//		SDL_SetRelativeMouseMode(SDL_TRUE);

		// Set GL rendering options
		glOrtho(0, 800, 600, 0, -1.0, 1.0);
//		gluPerspective(70.0, 800.0 / 600.0, 1.0, 1000.0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Main loop

		double cursor_x = 0.0, cursor_y = 0.0;
		double time_scale = 10.0;

		Uint32 frame_start;
		std::list<DataPoint> angles;

		Vector3<double> colors[3];
		colors[0].x = 1.0; colors[0].y = 0.0; colors[0].z = 0.0;
		colors[1].x = 0.0; colors[1].y = 1.0; colors[1].z = 0.0;
		colors[2].x = 0.0; colors[2].y = 0.5; colors[2].z = 1.0;

		SDL_Event evt;
		Packet *pkt = 0;
		bool running = true;
		while (running) {

			frame_start = SDL_GetTicks();

			// Poll user input events
			while (SDL_PollEvent(&evt)) {
				switch (evt.type) {
					case SDL_QUIT:
						running = false;
						break;

					case SDL_KEYDOWN:
						switch (evt.key.keysym.sym) {
							case SDLK_ESCAPE:
							case SDLK_q:
								running = false;
								break;
							case SDLK_LEFT:
								time_scale /= 1.05;
								break;
							case SDLK_RIGHT:
								time_scale *= 1.05;
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

			// Temporary false data to test interface
			DataPoint p;
			p.timestamp = SDL_GetTicks();
			p.accel.x = 90.0 * sin((float)p.timestamp / 2000.0 * PI);
			p.accel.y = 90.0 * cos((float)p.timestamp / 2000.0 * PI);
			p.accel.z = 90.0 * sin((float)p.timestamp / 200.0 * PI);
			angles.push_back(p);

			// Poll for radio packets
			/*
			while (pkt = connection.receive()) {
				switch (pkt->getHeader()) {
					case PKT_DIAGNOSTIC: {
						PacketDiagnostic *pktdiag = (PacketDiagnostic *)pkt;
						DataPoint data;
						data.timestamp = SDL_GetTicks();
						data.accel.x = pktdiag->getAccelX();
						data.accel.y = pktdiag->getAccelY();
						data.accel.z = pktdiag->getAccelZ();
						angles.push_back(data);
						delete pktdiag;
					}	break;
				}
			}
			pkt = 0;
			*/

			// Rendering

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
				// Zero Line
				for (int c = 0; c <= 2; ++c) {
					glColor4d(colors[c].x, colors[c].y, colors[c].z, 0.5);
					glVertex3d(0.5,   100.5 + 200 * c, 0.0);
					glVertex3d(780.5, 100.5 + 200 * c, 0.0);
				}

				// Scale
				for (double place = 780.5; place > 0.0;
						place -= 1000.0 / time_scale) {
					for (int c = 0; c <= 2; ++c) {
						glColor4d(colors[c].x, colors[c].y, colors[c].z, 0.5);
						glVertex3d(place, 97.5 + c * 200,  0.0);
						glVertex3d(place, 103.5 + c * 200, 0.0);
					}
				}
			glEnd();

			// Scale
			glBegin(GL_LINES);
			glEnd();

			// Data curves

			Uint32 currentticks = SDL_GetTicks();
			int i, numpoints = 0;
			std::list<DataPoint>::reverse_iterator iter;

			// X
			glBegin(GL_LINE_STRIP);
				double timepos = 800;
				for (iter = angles.rbegin();
						iter != angles.rend() && timepos > 0;
						++iter) {
					timepos = 780 - (currentticks - iter->timestamp)
					             / time_scale + 0.5;
					glColor4d(colors[0].x, colors[0].y, colors[0].z, 1.0);
					glVertex3d(
						timepos,
						(iter->accel.x / 90.0) * 100.0 + 100.5, 0.0);
					++numpoints;
				}
			glEnd();

			// Y
			glBegin(GL_LINE_STRIP);
				glColor4d(colors[1].x, colors[1].y, colors[1].z, 1.0);
				i = 0;
				for (iter = angles.rbegin();
						iter != angles.rend() && i < numpoints;
						++iter, ++i)
					glVertex3d(
						780 - (currentticks - iter->timestamp)
						      / time_scale + 0.5,
						(iter->accel.y / 90.0) * 100.0 + 300.5, 0.0);
			glEnd();

			// Z
			glBegin(GL_LINE_STRIP);
				glColor4d(colors[2].x, colors[2].y, colors[2].z, 1.0);
				i = 0;
				for (iter = angles.rbegin();
						iter != angles.rend() && i < numpoints;
						++iter, ++i)
					glVertex3d(
						780 - (currentticks - iter->timestamp)
						      / time_scale + 0.5,
						(iter->accel.z / 90.0) * 100.0 + 500.5, 0.0);
			glEnd();

			SDL_GL_SwapWindow(window);

			while (SDL_GetTicks() - frame_start < 1000 / MAX_FPS)
				SDL_Delay(1000 / MAX_FPS - (SDL_GetTicks() - frame_start));
		}

		// Clean-up
		SDL_GL_DeleteContext(glcontext);
		SDL_DestroyWindow(window);
		SDL_Quit();

	} catch (RadioException &e) {
		std::cout << "Failed to initialize radio: "
		          << e.getMessage() << std::endl;
		return -1;
	} catch (Exception &e) {
		std::cout << "Exception: " << e.getDescription() << std::endl;
		return -1;
	} catch (...) {
		std::cout << "Unrecognized exception" << std::endl;
		return -1;
	}

	return 0;
}

void quit(int code) {
	SDL_Quit();
	exit(code);
}

