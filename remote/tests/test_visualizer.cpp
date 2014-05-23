#include "SDL2/SDL.h"
#include "visualizer.h"

int main(int argc, char **argv) {
	Visualizer v("Visualizer Test");

	SDL_Event evt;
	Uint32 frame_start;
	bool running = true;
	while (running) {

		frame_start = SDL_GetTicks();

		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}

		v.render(frame_start);

		SDL_Delay(1000 / 60);
	}

	return 0;
}

