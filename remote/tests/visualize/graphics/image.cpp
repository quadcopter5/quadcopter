/*
	Philip Romano
	1/30/2014
	image.cpp
	
	Image class - facilitates loading and working with images.
	Modified for use outside of SDL Game Engine
*/

#include <cstring>
#include <string>
#include <stdlib.h>

#include "GL/gl.h"
#include "GL/glext.h"

#include "exception.h"            // Modified
#include "graphics/image.h"
//#include "graphics/loadimage.h" // Not for now
//#include "stringmanip.h"        // Not for now

// No image loading for now
/*
Image::Image(const std::string &filename, bool keepdata)
{
	mFilename    = filename;
	mImageData   = 0;
	mNumChannels =
	mID          =
	mWidth       =
	mHeight      =
	mPOTWidth    =
	mPOTHeight   = 0;
	mTexelWidth  =
	mTexelHeight = 0.0;
	
	// Load the image
	bool loaded = false;
	
	enum AttemptType { ATTEMPT_NONE = 0, ATTEMPT_JPEG, ATTEMPT_PNG };
	AttemptType attempted = ATTEMPT_NONE;
	std::string attempted_errmsg;
	
	// Determine file extension for hint on file type
	size_t extpos = filename.rfind('.');
	if (extpos != std::string::npos)
	{
		std::string extension = stringToLower(filename.substr(extpos + 1));
		if (extension.compare("jpg") == 0 || extension.compare("jpeg") == 0)
		{
			try {
				mImageData = loadJPEG(filename, &mWidth, &mHeight);
				mNumChannels = 3;
				loaded = true;
			} catch (GraphicLoadException &e) {
				attempted = ATTEMPT_JPEG;
				attempted_errmsg = ": (JPEG) " + e.getMessage();
			}
		}
		else if (extension.compare("png") == 0)
		{
			try {
				mImageData = loadPNG(filename, &mWidth, &mHeight);
				mNumChannels = 4;
				loaded = true;
			} catch (GraphicLoadException &e) {
				attempted = ATTEMPT_PNG;
				attempted_errmsg = ": (PNG) " + e.getMessage();
			}
		}
	}
	
	// If the hint didn't help, just try all the methods and see if it works
	// using the attempted variable to skip the method already tried.
	if (!loaded)
	{
		try {
			if (attempted != ATTEMPT_PNG)
			{
				mImageData = loadPNG(filename, &mWidth, &mHeight);
				mNumChannels = 4;
				loaded = true;
			}
			else
				THROW_EXCEPT(GraphicLoadException, "");
		} catch (GraphicLoadException &e) {
			try {
				if (attempted != ATTEMPT_JPEG)
				{
					mImageData = loadJPEG(filename, &mWidth, &mHeight);
					mNumChannels = 3;
					loaded = true;
				}
				else
					THROW_EXCEPT(GraphicLoadException, "");
			} catch (GraphicLoadException &e) {
				THROW_EXCEPT(GraphicLoadException, attempted_errmsg);
			}
		}
	}
	
	// Make temporary power-of-two image to load into OpenGL texture
//	unsigned char *potimage = makePOTImage(mImageData, mWidth, mHeight, &mPOTWidth, &mPOTHeight);
//	if (potimage == 0)
//		THROW_EXCEPT(GraphicLoadException, "Failed to make image into power-of-two texture: " + filename);
	
//	mTexelWidth = (float)mWidth / (float)mPOTWidth;
//	mTexelHeight = (float)mHeight / (float)mPOTHeight;
	
	// Generate OpenGL texture
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Important!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Otherwise, non-existent mipmaps
																							// will be used for this texture
	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mPOTWidth, mPOTHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, potimage);
//	delete[] potimage;
	if (mNumChannels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mImageData);
	else if (mNumChannels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mImageData);
	
	if (glGetError() != GL_NO_ERROR)
		THROW_EXCEPT(GraphicLoadException, "Could not generate OpenGL texture");
	
	if (!keepdata)
		freeData();
	
	// Copy data into mImageData if we are keeping it
//	if (keepdata)
//	{
//		mImageData = new unsigned char[mWidth * mHeight * 4];
//		memcpy(mImageData, tempdata, mWidth * mHeight * 4);
//	}
	
	// Free the SOIL image buffer
//	SOIL_free_image_data(tempdata);
}
*/

Image::Image(const unsigned char *data, int width, int height, int numchannels,
		bool keepdata)
{
	mID          = 0;
	mImageData   = 0;
	mNumChannels = numchannels;
	mWidth       = width;
	mHeight      = height;
	mPOTWidth    = 0;
	mPOTHeight   = 0;
	mTexelWidth  = 0.0;
	mTexelHeight = 0.0;
	
	// Finalize loaded image into power-of-two square
//	unsigned char *potimage = makePOTImage(data, mWidth, mHeight, &mPOTWidth, &mPOTHeight);
//	if (potimage == 0)
//		THROW_EXCEPT(GraphicLoadException, "Failed to make image into power-of-two texture");
	
//	mTexelWidth = (float)mWidth / (float)mPOTWidth;
//	mTexelHeight = (float)mHeight / (float)mPOTHeight;
	
	// Generate OpenGL texture
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);
	
//	if (atof((const char*)glGetString(GL_VERSION)) >= 1.2)
//	{
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	}
//	else
//	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Important!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Otherwise, non-existent mipmaps
	                                                                  // will be used for this texture
	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mPOTWidth, mPOTHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, potimage);
//	delete[] potimage;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	if (glGetError() != GL_NO_ERROR)
		THROW_EXCEPT(ImageException, "Could not generate OpenGL texture");
	
	if (keepdata)
	{
		mImageData = new unsigned char[width * height * 4];
		memcpy(mImageData, data, width * height * 4);
	}
}

Image::~Image()
{
	glDeleteTextures(1, &mID);
	freeData();
}

void Image::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, mID);
}

void Image::render(int x, int y, int width, int height,
						int originx, int originy,
						int subx, int suby, int subwidth, int subheight,
						float angle, float alpha, float red, float green, float blue)
{
	// Set width and height of subimage,
	// checking to make sure it is clamped at edges
	// of original image
	int subimagewidth		= mWidth;
	int subimageheight	= mHeight;
	
	if (subwidth != 0 && subwidth <= mWidth - subx)
		subimagewidth = subwidth;
	else
		subimagewidth = mWidth - subx;
	
	if (subheight != 0 && subheight <= mHeight - suby)
		subimageheight = subheight;
	else
		subimageheight = mHeight - suby;
	
	// Set on-screen width and height
	int drawwidth	= subimagewidth;  // Default is 1:1 pixel ratio
	int drawheight	= subimageheight;
	if (width > 0)                   // Only change draw size if argument > 0
		drawwidth = width;
	if (height > 0)
		drawheight = height;
	
	// Apply transformations for rotation and position, centered about
	// given origin
	int originx_scaled = (float)originx * ((float)drawwidth / (float)subimagewidth);
	int originy_scaled = (float)originy * ((float)drawheight / (float)subimageheight);
	
	glPushMatrix();
	glTranslatef((float)x - (float)originx_scaled, (float)y - (float)originy_scaled, 0.0f);
	// Rotation axis is about image center
	glTranslatef((float)originx_scaled, (float)originy_scaled, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-(float)originx_scaled, -(float)originy_scaled, 0.0f);
	
	// Render the image
	float ratiox = (float)subx / (float)mWidth;
	float ratioy = (float)suby / (float)mHeight;
	float ratiowidth = (float)subimagewidth / (float)mWidth;
	float ratioheight = (float)subimageheight / (float)mHeight;
	
	float	subtexelw = 0.0f,
			subtexelh = 0.0f;
	if (drawwidth > subimagewidth)
		subtexelw = 0.5 / mWidth;
	if (drawheight > subimageheight)
		subtexelh = 0.5 / mHeight;
	
	glBegin(GL_QUADS);
		glColor4f(red, green, blue, alpha);
		
		glTexCoord2f(	ratiox + subtexelw, // ratiox * mTexelWidth + subtexelw,    // Consider removing mTexelWidth....
							ratioy + subtexelh);// ratioy * mTexelHeight + subtexelh);
		glVertex2f(		0.0f, 0.0f);
		
		glTexCoord2f(	ratiox + subtexelw, // ratiox * mTexelWidth + subtexelw,
							(ratioy + ratioheight) - subtexelh); // (ratioy + ratioheight) * mTexelHeight - subtexelh);
		glVertex2f(		0.0f, drawheight);
		
		glTexCoord2f(	(ratiox + ratiowidth) - subtexelw, // (ratiox + ratiowidth) * mTexelWidth - subtexelw,
							(ratioy + ratioheight) - subtexelh); // (ratioy + ratioheight) * mTexelHeight - subtexelh);
		glVertex2f(		drawwidth, drawheight);
		
		glTexCoord2f(	(ratiox + ratiowidth) - subtexelw, // (ratiox + ratiowidth) * mTexelWidth - subtexelw,
							ratioy + subtexelw); // ratioy * mTexelHeight + subtexelh);
		glVertex2f(		drawwidth, 0.0f);
	glEnd();
	
	glPopMatrix();
}

void Image::renderColor(int x, int y, int width, int height,
							int originx, int originy,
							float alpha, float red, float green, float blue)
{
	// Set on-screen width and height
	int drawwidth  = mWidth,
	    drawheight = mHeight;
	if (width > 0)                   // Only change draw size if argument > 0
		drawwidth = width;
	if (height > 0)
		drawheight = height;
	
	// Apply transformations for rotation and position, centered about
	// given origin
	int originx_scaled = (float)originx * ((float)drawwidth / (float)mWidth);
	int originy_scaled = (float)originy * ((float)drawheight / (float)mHeight);
	
	// Render the image
	float	subtexelw = 0.0f,
			subtexelh = 0.0f;
	if (drawwidth > mWidth)
		subtexelw = 0.5 / mWidth;
	if (drawheight > mHeight)
		subtexelh = 0.5 / mHeight;
	
	glPushMatrix();
	glTranslatef((float)x - (float)originx_scaled, (float)y - (float)originy_scaled, 0.0f);
	
	glBegin(GL_TRIANGLE_FAN);
		glColor4f(red, green, blue, alpha);
		
		glTexCoord2f(	subtexelw,
							subtexelh);
		glVertex2f(		0.0f, 0.0f);
		
		glTexCoord2f(	subtexelw,
							1.0f - subtexelh); //mTexelHeight - subtexelh);
		glVertex2f(		0.0f, drawheight);
		
		glTexCoord2f(	1.0f - subtexelw,  //mTexelWidth - subtexelw,
							1.0f - subtexelh); //mTexelHeight - subtexelh);
		glVertex2f(		drawwidth, drawheight);
		
		glTexCoord2f(	1.0f - subtexelw,  //mTexelWidth - subtexelw,
							subtexelh);
		glVertex2f(		drawwidth, 0.0f);
	glEnd();
	
	glPopMatrix();
}


void Image::renderTiled(int x, int y, int width, int height,
							bool tilex, bool tiley,
							float alpha, float red, float green, float blue)
{
	float numwidth = 1.0f,
	      numheight = 1.0f;
	
	// Set width and height if <= 0
	if (width <= 0)
		width = mWidth;
	if (height <= 0)
		height = mHeight;
	
	if (tilex)
		numwidth = (float)width / (float)mWidth;
	if (tiley)
		numheight = (float)height / (float)mHeight;
	
	glPushMatrix();
	glTranslatef((float)x, (float)y, 0.0f);
	
	float	subtexelw = 0.0f,
			subtexelh = 0.0f;
	if (width > mWidth)
		subtexelw = 0.5 / mWidth;
	if (height > mHeight)
		subtexelh = 0.5 / mHeight;
	
	glBegin(GL_QUADS);
		glColor4f(red, green, blue, alpha);
		
		glTexCoord2f(	subtexelw,
							subtexelh);
		glVertex2f(		0.0f, 0.0f);
		
		glTexCoord2f(	subtexelw,
							numheight - subtexelh);// * mTexelHeight);// - subtexelh);
		glVertex2f(		0.0f, height);
		
		glTexCoord2f(	numwidth - subtexelw,// * mTexelWidth,// - subtexelw,
							numheight - subtexelh);// * mTexelHeight);// - subtexelh);
		glVertex2f(		width, height);
		
		glTexCoord2f(	numwidth - subtexelw,// * mTexelWidth,// - subtexelw,
							subtexelh);
		glVertex2f(		width, 0.0f);
	glEnd();
	
	glPopMatrix();
	
//	// Tile in both directions
//	int   xpos, ypos;
//	
//	if (tilex && tiley)
//	{
//		for (ypos = 0; ypos + mHeight <= height; ypos += mHeight)
//		{
//			for (xpos = 0; xpos + mWidth <= width; xpos += mWidth)
//				render(x + xpos, y + ypos, 0, 0, 0, 0, 0, 0,
//						0, 0,
//						0, alpha, red, green, blue);
//			if (xpos < width)
//				render(x + xpos, y + ypos, 0, 0, 0, 0, 0, 0,
//						width - xpos, 0,
//						0, alpha, red, green, blue);
//		}
//		
//		// Render bottom row, which is partly clipped off
//		if (ypos < height)
//			for (xpos = 0; xpos + mWidth <= width; xpos += mWidth)
//				render(x + xpos, y + ypos, 0, 0, 0, 0, 0, 0,
//						0, height - ypos,
//						0, alpha, red, green, blue);
//		
//		// Render bottom-right corner, if needed
//		if (width - xpos > 0 && height - ypos > 0)
//			render(x + xpos, y + ypos, 0, 0, 0, 0, 0, 0,
//					width - xpos, height - ypos,
//					0, alpha, red, green, blue);
//	}
//	
//	// Tile in x-direction
//	else if (tilex && !tiley)
//	{
//		for (xpos = 0; xpos + mWidth <= width; xpos += mWidth)
//			render(x + xpos, y, 0, height, 0, 0, 0, 0,
//					0, 0,
//					0, alpha, red, green, blue);
//		if (xpos < width)
//			render(x + xpos, y, 0, height, 0, 0, 0, 0,
//					width - xpos, 0,
//					0, alpha, red, green, blue);
//	}
//	
//	// Tile in y-direction
//	else if (!tilex && tiley)
//	{
//		for (ypos = 0; ypos + mHeight <= height; ypos += mHeight)
//			render(x, y + ypos, width, 0, 0, 0, 0, 0,
//					0, 0,
//					0, alpha, red, green, blue);
//		if (ypos < height)
//			render(x, y + ypos, width, 0, 0, 0, 0, 0,
//					0, height - ypos,
//					0, alpha, red, green, blue);
//	}
//	
//	// No tiling in either direction; just pass arguments to render()
//	else
//		render(x, y, width, height, 0, 0, 0, 0, 0, 0, 0,
//				alpha, red, green, blue);
}

GLuint Image::getID()
{
	return mID;
}

std::string Image::getFilename()
{
	return mFilename;
}

int Image::getWidth()
{
	return mWidth;
}

int Image::getHeight()
{
	return mHeight;
}

void Image::freeData()
{
	delete[] mImageData;
	mImageData = 0;
}

Image *Image::subImage(int x, int y, int width, int height, bool keepsubdata)
{
	if (!mImageData)
		return 0;
	
	// If dimensions are invalid, return 0
	if (x < 0 || y < 0 || x >= mWidth || y >= mHeight
			|| width == 0 || height == 0)
		return 0;
	
	// If size oversteps bounds of the image, truncate the size
	// and continue
	int subwidth = width;
	int subheight = height;
	if (x + width > mWidth)
		subwidth = mWidth - x;
	if (y + height > mHeight)
		subheight = mHeight - y;
	
	unsigned char *subdata = new unsigned char[subwidth * subheight * 4];
	unsigned char *current = mImageData + (y * mWidth * 4) + x * 4;
	for (int row = 0; row < subheight; row++)
	{
		memcpy(&subdata[row * subwidth * 4], current, subwidth * 4);
		current += mWidth * 4;
	}
	
	Image *img = new Image(subdata, subwidth, subheight, keepsubdata);
	delete[] subdata;
	return img;
}

/*
	Private Member Functions
*/

unsigned char *Image::makePOTImage(const unsigned char *data, int width, int height, int *potwidth, int *potheight)
{
	// Check for invalid arguments
	if (data == 0 || width == 0 || height == 0)
		return 0;
	
	unsigned char *potimage = 0;
	int numbits_width			= 0;
	int numbits_height		= 0;
	int greatestbit_width	= 0;
	int greatestbit_height	= 0;
	int potsize_width			= 0;
	int potsize_height		= 0;
	
	// Find the number of bits and the position of
	// the greatest hi bit in both the width and height integers
	for (unsigned int i = 0; i < sizeof(width) * 8; i++)
	{
		if ((width & ((int)1 << i)) == ((int)1 << i))
		{
			numbits_width++;
			greatestbit_width = i;
		}
	}
	for (unsigned int i = 0; i < sizeof(height) * 8; i++)
	{
		if ((height & ((int)1 << i)) == ((int)1 << i))
		{
			numbits_height++;
			greatestbit_height = i;
		}
	}
	
	// Determine the lowest power of two greater than
	// the given width and height
	if (numbits_width > 1)
		potsize_width = (int)1 << (greatestbit_width + 1);
	else
		potsize_width = (int)1 << greatestbit_width;
	
	if (numbits_height > 1)
		potsize_height = (int)1 << (greatestbit_height + 1);
	else
		potsize_height = (int)1 << greatestbit_height;
	
	// Create the new POT image buffer
	unsigned int size = potsize_width * potsize_height * 4; // 4 bytes per pixel
	potimage = new unsigned char[size];
	memset(potimage, 0, size);
	
	// Copy data into the top left corner
	unsigned char *current = potimage;
	int potsize_bytewidth = potsize_width * 4,
	    bytewidth = width * 4;
	for (int row = 0; row < height; row++)
	{
		memcpy(current, data + row * bytewidth, bytewidth);
		current += potsize_bytewidth;
	}
	
	*potwidth = potsize_width;
	*potheight = potsize_height;
	return potimage;
}

