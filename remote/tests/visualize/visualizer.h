/*
	visualizer.h
	
	A graph visualization of three variables over time.
	
	The class assumes that SDL and OpenGL have not been initialized. This
	initializes SDL events such that, once the Visualizer has been constructed,
	input can be managed outside of Visualizer.
*/

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include "geometry.h"
#include "graphics/font.h"

class Visualizer {
	public:
		/**
			Start the visualizer. Starting multiple Visualizers has undefined
			behavior.
		*/
		Visualizer(const std::string &title);

		~Visualizer();

		/**
			Feed values into the visualization. millis is the time of
			measurement, used as the horizontal axis. Usually, just feed the
			current value of SDL_GetTicks().
		*/
		void feedValues(Vector3<float> values, int millis);

		/**
			This function renders a new frame of the visualization and updates
			the viewport.

			time is the right-most (most recent) time to render, in
			milliseconds.
		*/
		void render(int time);

		/**
			Set the scale used for the time axis.
		*/
		void setTimeScale(double scale);

		/**
			Get the scale currently used for the time axis.
		*/
		double getTimeScale();

		/**
			Set the scale used for the value axis.
		*/
		void setValueScale(double scale);

		/**
			Get the scale currently used for the value axis.
		*/
		double getValueScale();

	private:

		struct DataPoint {
			Uint32 timestamp;
			Vector3<float> accel;
		};

		enum TextAlign {
			TEXT_LEFT   = 0,
			TEXT_CENTER = 1,
			TEXT_RIGHT  = 2
		};

		SDL_Window    *mWindow;
		SDL_GLContext mGLContext;

		boost::shared_ptr<Font> mFont;

		std::list<DataPoint> mData;
		Vector3<double> mColors[3];
		double mTimeScale;
		double mValueScale;
		int    mRightSide;

		/**
			Initialize the graphics system. This should only be called once
			in the lifetime of the program.
		*/
//		void initializeGraphics();


		/**
			Draw text on the canvas
		*/
		void drawText(const std::string &text, boost::shared_ptr<Font> fnt,
				int size, int x, int y, TextAlign align = TEXT_LEFT,
				double red = 1.0, double green = 1.0, double blue = 1.0,
				double alpha = 1.0);
};

#endif

