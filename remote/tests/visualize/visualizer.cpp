/*
	visualizer.cpp

	A graph visualization of three variables over time.
	
	The class assumes that SDL and OpenGL have not been initialized. This
	initializes SDL events such that, once the Visualizer has been constructed,
	input can be managed outside of Visualizer.
*/

#include <stdlib.h>
#include <iostream>
#include <list>

#include <math.h>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "geometry.h"
#include "exception.h"
#include "graphics/font.h"
#include "graphics/image.h"
#include "visualizer.h"

Visualizer::Visualizer(const std::string &title) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		THROW_EXCEPT(Exception, "Could not initialize SDL");

	Font::initializeSystem();

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
	mWindow = SDL_CreateWindow(title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			800, 600,
			SDL_WINDOW_OPENGL);// | SDL_WINDOW_INPUT_GRABBED);
	if (mWindow == NULL)
		THROW_EXCEPT(Exception, "Could not create SDL window");

	mGLContext = SDL_GL_CreateContext(mWindow);
	if (mGLContext == NULL)
		THROW_EXCEPT(Exception, "Could not create OpenGL context");

	//SDL_ShowCursor(0);

	// Set GL rendering options
	glOrtho(0, 800, 600, 0, -100.0, 100.0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load resources
	mFont = boost::shared_ptr<Font>(new Font("fonts/wqy-microhei.ttc"));
	mFont->loadSize(10);

	// Initialize graph scaling/positioning
	mTimeScale  = 10.0;
	mValueScale = 90.0;
	mRightSide  = 780;

	mColors[0].x = 1.0; mColors[0].y = 0.0; mColors[0].z = 0.0;
	mColors[1].x = 0.0; mColors[1].y = 1.0; mColors[1].z = 0.0;
	mColors[2].x = 0.0; mColors[2].y = 0.5; mColors[2].z = 1.0;
}

Visualizer::~Visualizer() {
	SDL_GL_DeleteContext(mGLContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Visualizer::feedValues(Vector3<float> values, int millis) {
	DataPoint data;
	data.timestamp = millis;
	data.accel = values;
	mData.push_back(data);
}

void Visualizer::render(int time) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Background
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
		for (int c = 0; c <= 2; ++c) {
			glColor4d(mColors[c].x, mColors[c].y, mColors[c].z, 0.05);
			glVertex3d(0.5,   200.5 * c,       -10.0);
			glVertex3d(0.5,   200.5 * (c + 1), -10.0);
			glVertex3d(mRightSide + 0.5, 200.5 * (c + 1), -10.0);
			glVertex3d(mRightSide + 0.5, 200.5 * c,       -10.0);
		}
	glEnd();

	// Graphs
	glBegin(GL_LINES);
		// Zero Line
		for (int c = 0; c <= 2; ++c) {
			glColor4d(mColors[c].x, mColors[c].y, mColors[c].z, 0.5);
			glVertex3d(0.5,   100.5 + 200 * c, 0.0);
			glVertex3d(mRightSide + 0.5, 100.5 + 200 * c, 0.0);
		}

		// Separators
		glColor4d((mColors[0].x + mColors[1].x) / 2.0,
				(mColors[0].y + mColors[1].y) / 2.0,
				(mColors[0].z + mColors[1].z) / 2.0, 1.0);
		glVertex3d(0.5, 200.5, 0.0);
		glVertex3d(mRightSide + 0.5, 200.5, 0.0);

		glColor4d((mColors[1].x + mColors[2].x) / 2.0,
				(mColors[1].y + mColors[2].y) / 2.0,
				(mColors[1].z + mColors[2].z) / 2.0, 1.0);
		glVertex3d(0.5, 400.5, 0.0);
		glVertex3d(mRightSide + 0.5, 400.5, 0.0);

		// Scale
		for (double place = mRightSide + 0.5; place > 0.0;
				place -= 1000.0 / mTimeScale) {
			for (int c = 0; c <= 2; ++c) {
				glColor4d(mColors[c].x, mColors[c].y, mColors[c].z, 0.5);
				glVertex3d(place, 97.5 + c * 200,  0.0);
				glVertex3d(place, 103.5 + c * 200, 0.0);
			}
		}
	glEnd();

	// Horizontal Scale text
	for (int c = 0; c <= 2; ++c) {
		drawText("0s", mFont, 8, mRightSide + 5, 105 + c * 200,
				TEXT_LEFT, mColors[c].x, mColors[c].y, mColors[c].z);
	}

	int num = 1;
	for (double place = mRightSide + 0.5 - 1000.0 / mTimeScale;
			place > 0.0;
			place -= 1000.0 / mTimeScale) {
		for (int c = 0; c <= 2; ++c) {
			drawText(boost::lexical_cast<std::string>(num),
					mFont, 8,
					(int)place, 117 + c * 200,
					TEXT_CENTER,
					mColors[c].x, mColors[c].y, mColors[c].z, 0.3);
		}
		++num;
	}
	
	// Vertical Scale text
	std::string text = boost::lexical_cast<std::string>(
			(int)mValueScale);
	std::string decimal = boost::lexical_cast<std::string>(
			(int)(mValueScale * 100.0) % 100);
	if (decimal.size() < 2)
		decimal = "0" + decimal;
	text += "." + decimal;

	for (int c = 0; c <= 2; ++c) {
		drawText(text,
				mFont, 8, 798, 12 + 200 * c, TEXT_RIGHT,
				mColors[c].x, mColors[c].y, mColors[c].z, 0.5);
		drawText("-" + text,
				mFont, 8, 798, 198 + 200 * c, TEXT_RIGHT,
				mColors[c].x, mColors[c].y, mColors[c].z, 0.5);
	}

	// Data curves
	int i, numpoints = 0;
	std::list<DataPoint>::reverse_iterator iter;

	glBindTexture(GL_TEXTURE_2D, 0);

	// X
	glBegin(GL_LINE_STRIP);
		double timepos = 800;
		for (iter = mData.rbegin();
				iter != mData.rend() && timepos > 0;
				++iter) {
			timepos = mRightSide - (time - iter->timestamp)
						 / mTimeScale + 0.5;
			glColor4d(mColors[0].x, mColors[0].y, mColors[0].z, 1.0);
			glVertex3d(
				timepos,
				100.5 - (iter->accel.x / mValueScale) * 100.0, 5.0);
			++numpoints;
		}
	glEnd();

	// Y
	glBegin(GL_LINE_STRIP);
		glColor4d(mColors[1].x, mColors[1].y, mColors[1].z, 1.0);
		i = 0;
		for (iter = mData.rbegin();
				iter != mData.rend() && i < numpoints;
				++iter, ++i) {
			glVertex3d(
				mRightSide - (time - iter->timestamp)
					  / mTimeScale + 0.5,
				300.5 - (iter->accel.y / mValueScale) * 100.0, 5.0);
		}
	glEnd();

	// Z
	glBegin(GL_LINE_STRIP);
		glColor4d(mColors[2].x, mColors[2].y, mColors[2].z, 1.0);
		i = 0;
		for (iter = mData.rbegin();
				iter != mData.rend() && i < numpoints;
				++iter, ++i) {
			glVertex3d(
				mRightSide - (time - iter->timestamp)
					  / mTimeScale + 0.5,
				500.5 - (iter->accel.z / mValueScale) * 100.0, 5.0);
		}
	glEnd();

	SDL_GL_SwapWindow(mWindow);
}

void Visualizer::setTimeScale(double scale) {
	mTimeScale = scale;
}

double Visualizer::getTimeScale() {
	return mTimeScale;
}

void Visualizer::setValueScale(double scale) {
	mValueScale = scale;
}

double Visualizer::getValueScale() {
	return mValueScale;
}

/*
	Private member functions
*/

void Visualizer::drawText(const std::string &text, boost::shared_ptr<Font> fnt,
		int size, int x, int y, TextAlign align, double red, double green,
		double blue, double alpha) {
	int px, py;
	Font::Glyph glyph;
	std::string::const_iterator iter;

	switch (align) {
		case TEXT_LEFT:
		default:
			px = x;
			py = y;
			break;

		case TEXT_CENTER:
		case TEXT_RIGHT: {
			px = x;
			py = y;
			
			int width = 0;
			for (iter = text.begin(); iter != text.end(); ++iter) {
				width += fnt->getGlyph(size, *iter).advancex;
			}

			if (align == TEXT_CENTER)
				px -= width / 2;
			else
				px -= width;

		}	break;
	}

	for (iter = text.begin(); iter != text.end(); ++iter) {
		glyph = fnt->getGlyph(size, *iter);
		if (glyph.image) {
			glyph.image->bindTexture();
			glyph.image->renderColor(px + glyph.bearingx, py + glyph.bearingy,
					0, 0, 0, 0, alpha, red, green, blue);
		}
		px += glyph.advancex;
		py += glyph.advancey;
	}
}

